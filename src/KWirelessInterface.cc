//
// @date: 08-11-2015
// @author: Asanga Udugama (adu@comnets.uni-bremen.de)
//

#include "KWirelessInterface.h"

#include "KBaseNodeInfo.h"


Define_Module(KWirelessInterface);

void KWirelessInterface::initialize(int stage)
{
    if (stage == 0) {

        // get parameters
        ownMACAddress = par("ownMACAddress").stringValue();
        wirelessRange = par("wirelessRange");
        expectedNodeTypes = par("expectedNodeTypes").stringValue();
        neighbourScanInterval = par("neighbourScanInterval");
        bandwidthBitRate = par("bandwidthBitRate");
        wirelessHeaderSize = par("wirelessHeaderSize");

        // set other parameters
        broadcastMACAddress = "FF:FF:FF:FF:FF:FF";


    } else if (stage == 1) {


        // get own module info
        ownNodeInfo = new KBaseNodeInfo();
        ownNodeInfo->nodeModule = getParentModule();
        for (cModule::SubmoduleIterator it(getParentModule()); !it.end(); ++it) {
            ownNodeInfo->nodeMobilityModule = dynamic_cast<inet::IMobility*>(*it);
            if (ownNodeInfo->nodeMobilityModule != NULL) {
                break;
            }
        }
        ownNodeInfo->nodeWirelessIfcModule = this;


    } else if (stage == 2) {

        // get module info of all other nodes in network
        for (int id = 0; id <= getSimulation()->getLastComponentId(); id++) {
            cModule *unknownModule = getSimulation()->getModule(id);
            if (unknownModule == NULL) {
                continue;
            }

            // has to be a node type module given in expectedNodeTypes parameter
            if(strstr(expectedNodeTypes.c_str(), unknownModule->getModuleType()->getName()) == NULL) {
                continue;
            }

            // if module is a KNode or KHeraldNode but is yourself
            if (unknownModule == ownNodeInfo->nodeModule) {
                continue;
            }

            KBaseNodeInfo *nodeInfo = new KBaseNodeInfo();
            nodeInfo->nodeModule = unknownModule;
            // EV_INFO <<  KWIRELESSINTERFACE_SIMMODULEINFO << " :: not me :: " << nodeInfo->nodeModule->getFullName() << "\n";

            // find the wireless ifc module & mobility module
            for (cModule::SubmoduleIterator it(unknownModule); !it.end(); ++it) {

                if (dynamic_cast<inet::IMobility*>(*it) != NULL) {
                    nodeInfo->nodeMobilityModule = dynamic_cast<inet::IMobility*>(*it);
                }
                if (dynamic_cast<KWirelessInterface*>(*it) != NULL) {
                    nodeInfo->nodeWirelessIfcModule = dynamic_cast<KWirelessInterface*>(*it);
                }
            }

            // wireless ifc module & mobility module must be present, else something wrong
            if (nodeInfo->nodeMobilityModule == NULL || nodeInfo->nodeWirelessIfcModule == NULL) {
                delete nodeInfo;
                continue;
            }

            allNodeInfoList.push_back(nodeInfo);
        }


        // setup first event to build neighbourhood node list and send to forwarding layer
        cMessage *sendNeighEvent = new cMessage("Send Neighbourhood Event");
        sendNeighEvent->setKind(KWIRELESSINTERFACE_NEIGH_EVENT_CODE);
        scheduleAt(simTime() + neighbourScanInterval, sendNeighEvent);

        // setup pkt send event message
        sendNextPacketTimeoutEvent = new cMessage("Send Next Packet Timeout Event");
        sendNextPacketTimeoutEvent->setKind(KWIRELESSINTERFACE_PKTSEND_EVENT_CODE);


    } else {
        EV_FATAL <<  KWIRELESSINTERFACE_SIMMODULEINFO << "Something is radically wrong\n";
    }

}

int KWirelessInterface::numInitStages() const
{
    return 3;
}

void KWirelessInterface::handleMessage(cMessage *msg)
{

    // find and send neighbour list to upper layer
    if (msg->isSelfMessage() && msg->getKind() == KWIRELESSINTERFACE_NEIGH_EVENT_CODE) {
        list<KBaseNodeInfo*> neighbourNodeInfoList;

        inet::Coord ownCoord = ownNodeInfo->nodeMobilityModule->getCurrentPosition();

        // check which nodes are neighbours and if so, add to list
        list<KBaseNodeInfo*>::iterator iteratorAllNodeInfo = allNodeInfoList.begin();
        while (iteratorAllNodeInfo != allNodeInfoList.end()) {
            KBaseNodeInfo *nodeInfo = *iteratorAllNodeInfo;
            inet::Coord neighCoord = nodeInfo->nodeMobilityModule->getCurrentPosition();

            // EV_INFO << KWIRELESSINTERFACE_SIMMODULEINFO << " :: Own Coord (x, y) :: " <<  ownCoord.x << ", " << ownCoord.y << " :: Neighbour Coord (x, y): "
            //     <<  neighCoord.x << ", " << neighCoord.y << "\n";

            double l = ((neighCoord.x - ownCoord.x) * (neighCoord.x - ownCoord.x))
                + ((neighCoord.y - ownCoord.y) * (neighCoord.y - ownCoord.y));
            double r = wirelessRange * wirelessRange;
            if (l <= r) {
                neighbourNodeInfoList.push_back(nodeInfo);
            }
            iteratorAllNodeInfo++;
        }

        // if there are neighbours, send message
        if (neighbourNodeInfoList.size() > 0) {

            // build message
            int neighCount = 0;

            KNeighbourListMsg *neighListMsg = new KNeighbourListMsg("Neighbour List Msg");
            neighListMsg->setNeighbourNameListArraySize(neighbourNodeInfoList.size());
            neighListMsg->setNeighbourNameCount(neighbourNodeInfoList.size());

            list<KBaseNodeInfo*>::iterator iteratorNeighNodeInfo = neighbourNodeInfoList.begin();
            while (iteratorNeighNodeInfo != neighbourNodeInfoList.end()) {
                KBaseNodeInfo *nodeInfo = *iteratorNeighNodeInfo;

                string nodeAddress = nodeInfo->nodeModule->par("ownAddress").stringValue();
                neighListMsg->setNeighbourNameList(neighCount, nodeAddress.c_str());

                EV_INFO << KWIRELESSINTERFACE_SIMMODULEINFO << " :: Neigbourhood Info :: " << ownMACAddress << " :: " << nodeAddress << " :: " << nodeInfo->nodeModule->getFullName() << "\n";

                neighCount++;
                iteratorNeighNodeInfo++;
            }

            // send msg to upper layer
            send(neighListMsg, "upperLayerOut");

        }

        // setup next event to build neighbourhood node list and send to forwarding layer
        cMessage *sendNeighEvent = new cMessage("Send Neighbourhood Event");
        sendNeighEvent->setKind(KWIRELESSINTERFACE_NEIGH_EVENT_CODE);
        scheduleAt(simTime() + neighbourScanInterval, sendNeighEvent);

        delete msg;

    // trigger to send next queued packet
    } else if (msg->isSelfMessage() && msg->getKind() == KWIRELESSINTERFACE_PKTSEND_EVENT_CODE) {

        if (!packetQueue.empty()) {
            cMessage *nextMsg = packetQueue.front();

            string destinationAddress = getDestinationAddress(nextMsg);

            // cout << simTime().dbl() << " " << getParentModule()->getFullName() << " queued packet sent - size " << ((cPacket*) nextMsg)->getByteLength() << "\n";

            // send broadcast message
            if (destinationAddress == broadcastMACAddress) {

                sendBroadcastMsg(nextMsg);

            // send unicast message
            } else {

                sendUnicastMsg(nextMsg);

            }

            packetQueue.pop();

        }

    // process a packet (arriving from upper or lower layers)
    } else {

        cGate *gate;
        char gateName[32];

       // get message arrival gate name
        gate = msg->getArrivalGate();
        strcpy(gateName, gate->getName());

        // msg from upper layer
        if (strstr(gateName, "upperLayerIn") != NULL) {

            // if currently there is a pending msg, then queue this msg
            if (sendNextPacketTimeoutEvent->isScheduled()) {
                
                // cout << simTime().dbl() << " " << getParentModule()->getFullName() << " packet queued to send - size " << ((cPacket*)msg)->getByteLength() << "\n";
                
                
                packetQueue.push(msg);

            // no queued msgs, so send immediately and start next message timer
            } else {
                string destinationAddress = getDestinationAddress(msg);


                // cout << simTime().dbl() << " " << getParentModule()->getFullName() << " packet sent immediately - size " << ((cPacket*)msg)->getByteLength() << "\n";

                // send broadcast message
                if (destinationAddress == broadcastMACAddress) {

                    sendBroadcastMsg(msg);

                // send unicast message
                } else {

                    sendUnicastMsg(msg);

                }
            }

        // from lowerLayerIn
        } else {

            // send msg to upper layer
            send(msg, "upperLayerOut");

        }
    }
}

void KWirelessInterface::sendBroadcastMsg(cMessage *msg)
{
    inet::Coord ownCoord = ownNodeInfo->nodeMobilityModule->getCurrentPosition();
    simtime_t txDuration = 0.0;

    list<KBaseNodeInfo*>::iterator iteratorAllNodeInfo = allNodeInfoList.begin();
    while (iteratorAllNodeInfo != allNodeInfoList.end()) {
        KBaseNodeInfo *nodeInfo = *iteratorAllNodeInfo;
        inet::Coord neighCoord = nodeInfo->nodeMobilityModule->getCurrentPosition();

        double l = ((neighCoord.x - ownCoord.x) * (neighCoord.x - ownCoord.x))
            + ((neighCoord.y - ownCoord.y) * (neighCoord.y - ownCoord.y));
        double r = wirelessRange * wirelessRange;
        if (l <= r) {

            // node in transmission range, so send to node
            cPacket *outPktCopy =  dynamic_cast<cPacket*>(msg->dup());

            // compute transmission duration
            double bitsToSend = (outPktCopy->getByteLength() * 8) + (wirelessHeaderSize * 8);
            txDuration = bitsToSend / bandwidthBitRate;

            sendDirect(outPktCopy, NULL, txDuration, nodeInfo->nodeModule, "radioIn");

            // cout <<  KWIRELESSINTERFACE_SIMMODULEINFO << " Sending (bcast) pkt to " << nodeInfo->nodeModule->getFullName() << "\n";
        }
        iteratorAllNodeInfo++;
    }

    // remove msg
    delete msg;

    // setup the the next pkt send
    if (txDuration > 0.0) {
        
        // setup timer based on last sent packet tx duration
        scheduleAt(simTime() + txDuration, sendNextPacketTimeoutEvent);
        
        // cout <<  KWIRELESSINTERFACE_SIMMODULEINFO << " " << getParentModule()->getFullName() << " bcast - duration " << txDuration << "\n";
        
    } else if (!packetQueue.empty()) {
        
        // if a pkt was unable to be sent, then try sending another
        // but after a short duration (1/10th of a millisecond later)
        scheduleAt(simTime() + 0.0001, sendNextPacketTimeoutEvent);
        
        // cout <<  KWIRELESSINTERFACE_SIMMODULEINFO << " " << getParentModule()->getFullName() << " bcast - duration == 0, queue not empty \n";
        
    } else {
        // timer not set

        // cout <<  KWIRELESSINTERFACE_SIMMODULEINFO << " " << getParentModule()->getFullName() << " bcast - duration == 0, queue empty \n";

    }

}

void KWirelessInterface::sendUnicastMsg(cMessage *msg)
{
    inet::Coord ownCoord = ownNodeInfo->nodeMobilityModule->getCurrentPosition();
    string destinationAddress = getDestinationAddress(msg);
    simtime_t txDuration = 0.0;
    bool nodeFound = FALSE;
    KBaseNodeInfo *nodeInfo = NULL;

    // find if the node to unicast is in range
    list<KBaseNodeInfo*>::iterator iteratorAllNodeInfo = allNodeInfoList.begin();
    while (iteratorAllNodeInfo != allNodeInfoList.end()) {
        nodeInfo = *iteratorAllNodeInfo;
        string nodeAddress = nodeInfo->nodeModule->par("ownAddress").stringValue();
        if (destinationAddress == nodeAddress) {

            inet::Coord neighCoord = nodeInfo->nodeMobilityModule->getCurrentPosition();

            double l = ((neighCoord.x - ownCoord.x) * (neighCoord.x - ownCoord.x))
                + ((neighCoord.y - ownCoord.y) * (neighCoord.y - ownCoord.y));
            double r = wirelessRange * wirelessRange;

            if (l <= r) {
                nodeFound = TRUE;
            }
            break;
        }
        iteratorAllNodeInfo++;
    }

    // if node in range, then send message
    if (nodeFound) {
        // node in transmission range, so send to node
        cPacket *outPktCopy =  dynamic_cast<cPacket*>(msg->dup());

        // compute transmission duration
        double bitsToSend = (outPktCopy->getByteLength() * 8) + (wirelessHeaderSize * 8);
        txDuration = bitsToSend / bandwidthBitRate;

        sendDirect(outPktCopy, NULL, txDuration, nodeInfo->nodeModule, "radioIn");

        // cout <<  KWIRELESSINTERFACE_SIMMODULEINFO << " Sending pkt to " << nodeInfo->nodeModule->getFullName() << "\n";

    }

    // remove msg
    delete msg;

    // setup the the next pkt send
    if (txDuration > 0.0) {
        
        // setup timer based on last sent packet tx duration
        scheduleAt(simTime() + txDuration, sendNextPacketTimeoutEvent);
        
        // cout <<  KWIRELESSINTERFACE_SIMMODULEINFO << " " << getParentModule()->getFullName() << " ucast - duration " << txDuration << "\n";
        
    } else if (!packetQueue.empty()) {
        
        // if a pkt was unable to be sent, then try sending another
        // but after a short duration (1/10th of a millisecond later)
        scheduleAt(simTime() + 0.0001, sendNextPacketTimeoutEvent);
        
        // cout <<  KWIRELESSINTERFACE_SIMMODULEINFO << " " << getParentModule()->getFullName() << " ucast - duration == 0, queue not empty \n";

    } else {
        // timer not set

        // cout <<  KWIRELESSINTERFACE_SIMMODULEINFO << " " << getParentModule()->getFullName() << " ucast - duration == 0, queue empty \n";

    }

}

string KWirelessInterface::getDestinationAddress(cMessage *msg)
{
    KDataMsg *dataMsg = dynamic_cast<KDataMsg*>(msg);
    if (dataMsg) {
        return dataMsg->getDestinationAddress();
    }

    KFeedbackMsg *feedbackMsg = dynamic_cast<KFeedbackMsg*>(msg);
    if (feedbackMsg) {
        return feedbackMsg->getDestinationAddress();
    }

    KSummaryVectorMsg *summaryVectorMsg = dynamic_cast<KSummaryVectorMsg*>(msg);
    if (summaryVectorMsg) {
        return summaryVectorMsg->getDestinationAddress();
    }

    KDataRequestMsg *dataRequestMsg = dynamic_cast<KDataRequestMsg*>(msg);
    if (dataRequestMsg) {
        return dataRequestMsg->getDestinationAddress();
    }

    EV_FATAL <<  KWIRELESSINTERFACE_SIMMODULEINFO << " :: Unknown message type. Check \"string KWirelessInterface::getDestinationAddress(cMessage *msg)\"\n";

    throw cRuntimeError("Unknown message type in KWirelessnterface");

    return string();
}

void KWirelessInterface::finish()
{
    // remove send msg timeout
    if (sendNextPacketTimeoutEvent->isScheduled()) {
        cancelEvent(sendNextPacketTimeoutEvent);
    }
    delete sendNextPacketTimeoutEvent;

}


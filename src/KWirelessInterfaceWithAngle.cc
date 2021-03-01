//
// @date: 08-11-2015
// @author: Asanga Udugama (adu@comnets.uni-bremen.de)
//

#include "KWirelessInterfaceWithAngle.h"

#include "KBaseNodeInfo.h"


Define_Module(KWirelessInterfaceWithAngle);

void KWirelessInterfaceWithAngle::initialize(int stage)
{
    if (stage == 0) {

        // get parameters
        ownMACAddress = par("ownMACAddress").stringValue();
        wirelessRange = par("wirelessRange");
        calculateSpeedInterval = par("calculateSpeedInterval");
        neighbourScanInterval = par("neighbourScanInterval");
        bandwidthBitRate = par("bandwidthBitRate");
        wirelessHeaderSize = par("wirelessHeaderSize");

        // set other parameters
        broadcastMACAddress = "FF:FF:FF:FF:FF:FF";

    } else if (stage == 1) {

        // get own module info
        ownNodeInfo = new KBaseNodeInfo();
        ownNodeInfo->nodeModule = getParentModule();
        std::cout << "Nodemodule: " << ownNodeInfo->nodeModule << "\n";
        cModule *unknownModule = getParentModule()->getSubmodule("mobility");
        ownNodeInfo->nodeMobilityModule = check_and_cast<inet::IMobility*>(unknownModule);
        //ownNodeInfo->nodeWirelessIfcModule = this;
        ownNodeInfo->firstTimePosition = true;
        ownNodeInfo->nodeSpeed = 0.0;
        ownNodeInfo->nodeAngle = 0.0;

    } else if (stage == 2) {

        // get module info of all other nodes in network
        for (int id = 0; id <= getSimulation()->getLastComponentId(); id++) {
            cModule *unknownModule = getSimulation()->getModule(id);
            if (unknownModule == NULL) {
                continue;
            }

            // if module is a KNode or KHeraldNode but is yourself
            if (unknownModule == ownNodeInfo->nodeModule) {
                continue;
            }

            // find the wireless ifc module & mobility module
            cModule *unknownMobilityModule = NULL;
            cModule *unknownWirelessIfcModule = NULL;

            for (cModule::SubmoduleIterator it(unknownModule); !it.end(); ++it) {

                if (dynamic_cast<inet::IMobility*>(*it) != NULL) {
                    unknownMobilityModule = *it;
                }
                if (dynamic_cast<KWirelessInterfaceWithAngle*>(*it) != NULL) {
                    unknownWirelessIfcModule = *it;
                }
            }

            if (unknownMobilityModule != NULL && unknownWirelessIfcModule != NULL) {
                KBaseNodeInfo *nodeInfo = new KBaseNodeInfo();
                nodeInfo->nodeModule = unknownModule;
                nodeInfo->nodeMobilityModule = dynamic_cast<inet::IMobility*>(unknownMobilityModule);
                //nodeInfo->nodeWirelessIfcModule = dynamic_cast<KWirelessInterfaceWithAngle*>(unknownWirelessIfcModule);
                nodeInfo->firstTimePosition = true;
                nodeInfo->nodeAngle = 0.0;
                nodeInfo->nodeSpeed = 0.0;
                allNodeInfoList.push_back(nodeInfo);
            }
        }

        // setup first event to calculate speed and angle and place to neighbourhood node list
        speedCheckTimer = new cMessage("Compute SpeedAngle Event");
        speedCheckTimer->setKind(KWIRELESSINTERFACEWITHANGLE_SPEEDCHECK_EVENT_CODE);
        scheduleAt(simTime() + calculateSpeedInterval, speedCheckTimer);

        // setup first event to build neighbourhood node list and send to forwarding layer
        cMessage *sendNeighEvent = new cMessage("Send Neighbourhood Event");
        sendNeighEvent->setKind(KWIRELESSINTERFACEWITHANGLE_NEIGH_EVENT_CODE);
        scheduleAt(simTime() + neighbourScanInterval, sendNeighEvent);

        // setup pkt send event message
        sendPacketTimeoutEvent = new cMessage("Send Packet Timeout Event");
        sendPacketTimeoutEvent->setKind(KWIRELESSINTERFACEWITHANGLE_PKTSEND_EVENT_CODE);

        // setup statistics signals
        neighSizeSignal = registerSignal("linkNeighSize");
        neighSizeCountSignal = registerSignal("linkNeighSizeCount");
        contactDurationSignal = registerSignal("linkContactDuration");
        contactDurationCountSignal = registerSignal("linkContactDurationCount");
        contactCountSignal = registerSignal("linkContactCount");

        simpleNeighSizeSignal = registerSignal("linkSimpleNeighSize");

        packetsSentSignal = registerSignal("linkPacketsSent");
        packetsSentBytesSignal = registerSignal("linkPacketsSentBytes");
        packetsDeliveredSignal = registerSignal("linkPacketsDelivered");
        packetsDeliveredBytesSignal = registerSignal("linkPacketsDeliveredBytes");
        packetsDroppedSignal = registerSignal("linkPacketsDropped");
        packetsDroppedBytesSignal = registerSignal("linkPacketsDroppedBytes");
        packetsReceivedSignal = registerSignal("linkPacketsReceived");
        packetsReceivedBytesSignal = registerSignal("linkPacketsReceivedBytes");



    } else {
        EV_FATAL <<  KWIRELESSINTERFACEWITHANGLE_SIMMODULEINFO << "Something is radically wrong\n";
    }
}

int KWirelessInterfaceWithAngle::numInitStages() const
{
    return 3;
}

void KWirelessInterfaceWithAngle::handleMessage(cMessage *msg)
{

    // find and send neighbour list to upper layer
    if (msg->isSelfMessage() && msg->getKind() == KWIRELESSINTERFACEWITHANGLE_NEIGH_EVENT_CODE) {

        // init current neighbor list and neighbor range category list
        while (currentNeighbourNodeInfoList.size() > 0) {
            list<KBaseNodeInfo*>::iterator iteratorCurrentNeighbourNodeInfo = currentNeighbourNodeInfoList.begin();
            KBaseNodeInfo *nodeInfo = *iteratorCurrentNeighbourNodeInfo;
            currentNeighbourNodeInfoList.remove(nodeInfo);
        }
        currentNeighbourRangeCatList.clear();

        // get current position of self
        inet::Coord ownCoord = ownNodeInfo->nodeMobilityModule->getCurrentPosition();

        // check which nodes are neighbours and if so, add to list
        list<KBaseNodeInfo*>::iterator iteratorAllNodeInfo = allNodeInfoList.begin();
        while (iteratorAllNodeInfo != allNodeInfoList.end()) {
            KBaseNodeInfo *nodeInfo = *iteratorAllNodeInfo;
            inet::Coord neighCoord = nodeInfo->nodeMobilityModule->getCurrentPosition();

#ifdef KWIRELESSINTERFACEWITHANGLE_EUCLIDEAN_DISTANCE
            // check using euclidean distances
            double l = ((neighCoord.x - ownCoord.x) * (neighCoord.x - ownCoord.x))
                + ((neighCoord.y - ownCoord.y) * (neighCoord.y - ownCoord.y));
            double r = wirelessRange * wirelessRange;

            if (l <= r) {
                currentNeighbourNodeInfoList.push_back(nodeInfo);

                // add range category of neighbour to list
                bool found = false;
                for (int i = 0; i < catVals.totalCats; i++) {
                    if (l >= (catVals.rangeStart[i] * catVals.rangeStart[i]) && l <= (catVals.rangeEnd[i] * catVals.rangeEnd[i])) {
                        currentNeighbourRangeCatList.push_back(catVals.rangeCatNum[i]);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    currentNeighbourRangeCatList.push_back(999);
                }

            }
#else
            // check using chebyshev and euclidean distances
            double l = MAX(POSITIVE(neighCoord.x - ownCoord.x), POSITIVE(neighCoord.y - ownCoord.y));
            if (l <= wirelessRange) {
                l = ((neighCoord.x - ownCoord.x) * (neighCoord.x - ownCoord.x))
                    + ((neighCoord.y - ownCoord.y) * (neighCoord.y - ownCoord.y));
                double r = wirelessRange * wirelessRange;
                if (l <= r) {
                    currentNeighbourNodeInfoList.push_back(nodeInfo);

                    // add range category of neighbour to list
                    bool found = false;
                    for (int i = 0; i < catVals.totalCats; i++) {
                        if (l >= (catVals.rangeStart[i] * catVals.rangeStart[i]) && l <= (catVals.rangeEnd[i] * catVals.rangeEnd[i])) {
                            currentNeighbourRangeCatList.push_back(catVals.rangeCatNum[i]);
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        currentNeighbourRangeCatList.push_back(999);
                    }
                }
            }
#endif
            iteratorAllNodeInfo++;
        }

        // compute and emit stats
#ifdef KWIRELESSINTERFACEWITHANGLE_COMPUTE_STATS
        generateNeighStats();
#endif
        // if there are neighbours, send message
        if (currentNeighbourNodeInfoList.size() > 0) {

            // build message
            int neighCount = 0;

            KNeighbourListMsg *neighListMsg = new KNeighbourListMsg("Neighbour List Msg");
            neighListMsg->setNeighbourNameListArraySize(currentNeighbourNodeInfoList.size());
            neighListMsg->setNeighbourNameCount(currentNeighbourNodeInfoList.size());
            neighListMsg->setNeighbourSpeedListArraySize(currentNeighbourNodeInfoList.size());
            neighListMsg->setNeighbourAngleListArraySize(currentNeighbourNodeInfoList.size());
            neighListMsg->setNeighbourRangeCatListArraySize(currentNeighbourNodeInfoList.size());

            list<KBaseNodeInfo*>::iterator iteratorCurrentNeighbourNodeInfo = currentNeighbourNodeInfoList.begin();
            while (iteratorCurrentNeighbourNodeInfo != currentNeighbourNodeInfoList.end()) {
                KBaseNodeInfo *nodeInfo = *iteratorCurrentNeighbourNodeInfo;

                string nodeAddress = nodeInfo->nodeModule->par("ownAddress").stringValue();
                neighListMsg->setNeighbourNameList(neighCount, nodeAddress.c_str());
                neighListMsg->setNeighbourSpeedList(neighCount, nodeInfo->nodeSpeed);
                neighListMsg->setNeighbourAngleList(neighCount, nodeInfo->nodeAngle);

                list<int>::iterator itRangeCat = currentNeighbourRangeCatList.begin();
                advance(itRangeCat, neighCount);
                neighListMsg->setNeighbourRangeCatList(neighCount, *itRangeCat);

                //edited Vishnu

                neighCount++;
                iteratorCurrentNeighbourNodeInfo++;
            }

            neighListMsg->setMySpeed(ownNodeInfo->nodeSpeed);
            neighListMsg->setMyAngle(ownNodeInfo->nodeAngle);

            // send msg to upper layer
            send(neighListMsg, "upperLayerOut");

        }

        // setup next event to build neighbourhood node list and send to forwarding layer
        cMessage *sendNeighEvent = new cMessage("Send Neighbourhood Event");
        sendNeighEvent->setKind(KWIRELESSINTERFACEWITHANGLE_NEIGH_EVENT_CODE);
        scheduleAt(simTime() + neighbourScanInterval, sendNeighEvent);

        delete msg;


//########################## vishnu mobility vectors edited Feb 12, 2020

        // Calculate speed and angle based on timer - update one's mobility vectors and find mobility vectors of neighbors
    } else if (msg->isSelfMessage() && msg->getKind() == KWIRELESSINTERFACEWITHANGLE_SPEEDCHECK_EVENT_CODE) {

        if (ownNodeInfo->firstTimePosition) {
            ownNodeInfo->firstTimePosition = false;
            ownNodeInfo->lastPosition = ownNodeInfo->nodeMobilityModule->getCurrentPosition();
        }
        else {

            inet::Coord myOwnCoord = ownNodeInfo->nodeMobilityModule->getCurrentPosition();
            //calculate speed
            double distance = ((myOwnCoord.x - ownNodeInfo->lastPosition.x) * 
                (myOwnCoord.x - ownNodeInfo->lastPosition.x) 
                    + (myOwnCoord.y - ownNodeInfo->lastPosition.y) *
                        (myOwnCoord.y - ownNodeInfo->lastPosition.y));
            double speed = sqrt(distance/calculateSpeedInterval);
            double angle = 0.0;

            if (speed <= 0.0) {
                angle = 0.0;
            }
            else {
                double positionX = myOwnCoord.x - ownNodeInfo->lastPosition.x;
                double positionY = myOwnCoord.y - ownNodeInfo->lastPosition.y;
                double posRatio = positionY/positionX;
                double angleRadians = atan (positionY/positionX);
                angle = angleRadians * (180/3.14);

                // calculate angle
                if (positionX < 0 && positionY > 0) {
                    // Quadrant: II
                    angle = angle + 180;
                }
                else if (positionX < 0 && positionY < 0) {
                    //Quadrant : III
                    angle = angle + 180;
                }
                else if (positionX > 0 && positionY < 0) {
                    //Quadrant: IV
                    angle = angle + 360;
                }
            }
            ownNodeInfo->nodeSpeed = speed;
            ownNodeInfo->nodeAngle = angle;
            ownNodeInfo->lastPosition = myOwnCoord;
        }

        // calculate mobility vectors of all nodes
        list<KBaseNodeInfo*>::iterator iteratorAllNodeInfo = allNodeInfoList.begin();
        while (iteratorAllNodeInfo != allNodeInfoList.end()) {
            KBaseNodeInfo *nodeInfo = *iteratorAllNodeInfo;
            inet::Coord neighCoord = nodeInfo->nodeMobilityModule->getCurrentPosition();

            if (nodeInfo->firstTimePosition) {
                nodeInfo->firstTimePosition = false;
                nodeInfo->lastPosition = nodeInfo->nodeMobilityModule->getCurrentPosition();
            }

            else {
                //calculate speed
                double distance = ((neighCoord.x - nodeInfo->lastPosition.x) 
                    * (neighCoord.x - nodeInfo->lastPosition.x) + (neighCoord.y - nodeInfo->lastPosition.y) *
                            (neighCoord.y - nodeInfo->lastPosition.y));
                double speed = sqrt(distance/calculateSpeedInterval);
                double angle = 0;

                if (speed <= 0){
                    angle = 0;
                }

                else {
                    double positionX = neighCoord.x - nodeInfo->lastPosition.x;
                    double positionY = neighCoord.y - nodeInfo->lastPosition.y;
                    double posRatio = positionY/positionX;
                    double angleRadians = atan (positionY/positionX);
                    angle = angleRadians * (180/3.14);

                    // calculate angle
                    if (positionX < 0 && positionY > 0) {
                        // Quadrant: II
                        angle = angle + 180;
                    }
                    else if (positionX < 0 && positionY < 0) {
                        //Quadrant : III
                        angle = angle + 180;
                    }
                    else if (positionX > 0 && positionY < 0){
                        //Quadrant: IV
                        angle = angle + 360;
                    }
                }
                nodeInfo->nodeSpeed = speed;
                nodeInfo->nodeAngle = angle;
            }
            nodeInfo->lastPosition = neighCoord;
            iteratorAllNodeInfo++;
        }

        // setup first event to calculate speed and angle and place to neighbourhood node list
        speedCheckTimer = new cMessage("Compute SpeedAngle Event");
        speedCheckTimer->setKind(KWIRELESSINTERFACEWITHANGLE_SPEEDCHECK_EVENT_CODE);
        scheduleAt(simTime() + calculateSpeedInterval, speedCheckTimer);

        delete msg;

//######################### - vishnu mobility vectors edited

    // trigger to send pending packet and setup new send
    } else if (msg->isSelfMessage() && msg->getKind() == KWIRELESSINTERFACEWITHANGLE_PKTSEND_EVENT_CODE) {

        // send the pending packet out
        sendPendingMsg();

        // if there are queued packets, setup for sending the next one at top of queue
        if (!packetQueue.empty()) {

            // get next at the top of queue
            cMessage *nextMsg = packetQueue.front();
            packetQueue.pop();

            // setup for next message sending and start timer
            setupSendingMsg(nextMsg);
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
            if (sendPacketTimeoutEvent->isScheduled()) {

                packetQueue.push(msg);

                // no queued msgs
            } else {

                // so setup for next message sending and start timer
                setupSendingMsg(msg);

            }

        // from lowerLayerIn
        } else {

            // update receive packet stats
            emit(packetsReceivedSignal, 1);
            cPacket *pkt = dynamic_cast<cPacket*>(msg);
            emit(packetsReceivedBytesSignal, (long) pkt->getByteLength());

            // send msg to upper layer
            send(msg, "upperLayerOut");
        }
    }
}

void KWirelessInterfaceWithAngle::setupSendingMsg(cMessage *msg)
{
    string destinationAddress = getDestinationAddress(msg);
    bool isBroadcastMsg = FALSE;
    if (destinationAddress == broadcastMACAddress) {
        isBroadcastMsg = TRUE;
    }

    // make the neighbour list at begining of msg tx (to check later if those neighbours are still there)
    list<KBaseNodeInfo*>::iterator iteratorCurrentNeighbourNodeInfo = currentNeighbourNodeInfoList.begin();
    while (iteratorCurrentNeighbourNodeInfo != currentNeighbourNodeInfoList.end()) {
        KBaseNodeInfo *nodeInfo = *iteratorCurrentNeighbourNodeInfo;
        string nodeAddress = nodeInfo->nodeModule->par("ownAddress").stringValue();

        // if broadcast, add all addresses to tx time neighbour list
        // if unicast, add only the specific address
        if (isBroadcastMsg || destinationAddress == nodeAddress) {
            atTxNeighbourNodeInfoList.push_back(nodeInfo);
        }

        iteratorCurrentNeighbourNodeInfo++;
    }

    // save the msg to send
    currentPendingMsg = msg;

    // compute transmission duration
    cPacket *currentPendingPkt = dynamic_cast<cPacket*>(currentPendingMsg);
    double bitsToSend = (currentPendingPkt->getByteLength() * 8) + (wirelessHeaderSize * 8);
    double txDuration = bitsToSend / bandwidthBitRate;

    // setup timer to trigger at tx duration
    scheduleAt(simTime() + txDuration, sendPacketTimeoutEvent);

    // update sent packets stats
    emit(packetsSentSignal, 1);
    emit(packetsSentBytesSignal, (long) currentPendingPkt->getByteLength());

}

void KWirelessInterfaceWithAngle::sendPendingMsg()
{
    // check if nodes to deliver are still in neighbourhood, if so send the packet
    list<KBaseNodeInfo*>::iterator iteratorAtTxNeighbourNodeInfo = atTxNeighbourNodeInfoList.begin();
    while (iteratorAtTxNeighbourNodeInfo != atTxNeighbourNodeInfoList.end()) {
        KBaseNodeInfo *atTxNeighbourNodeInfo = *iteratorAtTxNeighbourNodeInfo;
        string atTxNeighbourNodeAddress = atTxNeighbourNodeInfo->nodeModule->par("ownAddress").stringValue();

        bool nodeStillInNeighbourhood = false;
        bool badConnectivity = false;
        int i = 0;
        list<KBaseNodeInfo*>::iterator iteratorCurrentNeighbourNodeInfo = currentNeighbourNodeInfoList.begin();
        while (iteratorCurrentNeighbourNodeInfo != currentNeighbourNodeInfoList.end()) {
            KBaseNodeInfo *currentNeighbourNodeInfo = *iteratorCurrentNeighbourNodeInfo;
            string currentNeighbourNodeAddress = currentNeighbourNodeInfo->nodeModule->par("ownAddress").stringValue();

            // check if node is still in neighbourhood
            if (atTxNeighbourNodeAddress == currentNeighbourNodeAddress) {

                nodeStillInNeighbourhood = true;

                // get range cat and corresponding PER
                list<int>::iterator itRangeCat = currentNeighbourRangeCatList.begin();
                advance(itRangeCat, i);
                int rangeCatNum = *itRangeCat;
                double rangePER = 0.0;
                for (int j = 0; j < catVals.totalCats; j++) {
                    if (catVals.rangeCatNum[j] == rangeCatNum) {
                        rangePER = catVals.rangePER[j];
                        break;
                    }
                }

                // drop based on PER
                double rndVal = uniform(0.0, 1.0);
                if (rndVal < rangePER) {
                    badConnectivity = true;
                }

                if (!badConnectivity) {

                    // make duplicate of packet
                    cPacket *outPktCopy =  dynamic_cast<cPacket*>(currentPendingMsg->dup());

                    // send to node
                    sendDirect(outPktCopy, currentNeighbourNodeInfo->nodeModule, "radioIn");

                    // update delivered packets stats
                    emit(packetsDeliveredSignal, 1);
                    cPacket *currentPendingPkt = dynamic_cast<cPacket*>(currentPendingMsg);
                    emit(packetsDeliveredBytesSignal, (long) currentPendingPkt->getByteLength());
                }

                break;
            }

            iteratorCurrentNeighbourNodeInfo++;
            i++;
        }

        // update dropped packets stats
        if (!nodeStillInNeighbourhood || badConnectivity) {
            emit(packetsDroppedSignal, 1);
            cPacket *currentPendingPkt = dynamic_cast<cPacket*>(currentPendingMsg);
            emit(packetsDroppedBytesSignal, (long) currentPendingPkt->getByteLength());

        }

        iteratorAtTxNeighbourNodeInfo++;
    }

    // remove original message
    delete currentPendingMsg;
    currentPendingMsg = NULL;

    // remove entries in list used to check neighbour list at begining of msg tx
    while (atTxNeighbourNodeInfoList.size() > 0) {
        list<KBaseNodeInfo*>::iterator iteratorAtTxNeighbourNodeInfo = atTxNeighbourNodeInfoList.begin();
        KBaseNodeInfo *nodeInfo = *iteratorAtTxNeighbourNodeInfo;
        atTxNeighbourNodeInfoList.remove(nodeInfo);
    }

}

string KWirelessInterfaceWithAngle::getDestinationAddress(cMessage *msg)
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

    KDPtableRequestMsg *dpTableRequestMsg = dynamic_cast<KDPtableRequestMsg*>(msg);
    if (dpTableRequestMsg) {
        return dpTableRequestMsg->getDestinationAddress();
    }

    KDPtableDataMsg *dpTableDataMsg = dynamic_cast<KDPtableDataMsg*>(msg);
    if (dpTableDataMsg) {
        return dpTableDataMsg->getDestinationAddress();
    }


    EV_FATAL <<  KWIRELESSINTERFACEWITHANGLE_SIMMODULEINFO << ">!<Unknown message type. Check \"string KWirelessInterfaceWithAngle::getDestinationAddress(cMessage *msg)\"\n";

    throw cRuntimeError("Unknown message type in KWirelessnterface");

    return string();
}


void KWirelessInterfaceWithAngle::generateNeighStats()
{

    emit(neighSizeSignal, (long) currentNeighbourNodeInfoList.size());
    emit(neighSizeCountSignal, 1);

    emit(simpleNeighSizeSignal, (long) currentNeighbourNodeInfoList.size());

    // check and remove left neighbourhood
    list<KBaseNodeInfo*>::iterator itPrevNeighNodeInfo = previousNeighbourNodeInfoList.begin();
    while (itPrevNeighNodeInfo != previousNeighbourNodeInfoList.end()) {
        KBaseNodeInfo *prevNodeInfo = *itPrevNeighNodeInfo;

        list<KBaseNodeInfo*>::iterator itCurrNeighNodeInfo = currentNeighbourNodeInfoList.begin();
        bool found = FALSE;
        while (itCurrNeighNodeInfo != currentNeighbourNodeInfoList.end()) {
            KBaseNodeInfo *currNodeInfo = *itCurrNeighNodeInfo;

            if (currNodeInfo->nodeAddress == prevNodeInfo->nodeAddress) {
                found = TRUE;
                break;
            }

            itCurrNeighNodeInfo++;
        }
        if (!found) {
            simtime_t duration = simTime() - prevNodeInfo->neighbourStartTime;
            emit(contactDurationSignal, duration);
            emit(contactDurationCountSignal, 1);
            emit(contactCountSignal, 1);
            prevNodeInfo->neighbourStartTime = -1.0;
        }

        itPrevNeighNodeInfo++;
    }
    bool found = TRUE;
    while (previousNeighbourNodeInfoList.size() > 0 && found) {
        found = FALSE;
        list<KBaseNodeInfo*>::iterator itPrevNeighNodeInfo = previousNeighbourNodeInfoList.begin();
        while (itPrevNeighNodeInfo != previousNeighbourNodeInfoList.end()) {
            KBaseNodeInfo *prevNodeInfo = *itPrevNeighNodeInfo;
            if(prevNodeInfo->neighbourStartTime == -1.0) {
                previousNeighbourNodeInfoList.remove(prevNodeInfo);
                found = TRUE;
                break;
            }
            itPrevNeighNodeInfo++;
        }
    }

    // insert new neighbours
    list<KBaseNodeInfo*>::iterator itCurrNeighNodeInfo = currentNeighbourNodeInfoList.begin();
    while (itCurrNeighNodeInfo != currentNeighbourNodeInfoList.end()) {
        KBaseNodeInfo *currNodeInfo = *itCurrNeighNodeInfo;

        list<KBaseNodeInfo*>::iterator itPrevNeighNodeInfo = previousNeighbourNodeInfoList.begin();
        bool found = FALSE;
        while (itPrevNeighNodeInfo != previousNeighbourNodeInfoList.end()) {
            KBaseNodeInfo *prevNodeInfo = *itPrevNeighNodeInfo;

            if (prevNodeInfo->nodeAddress == currNodeInfo->nodeAddress) {
                found = TRUE;
                break;
            }

            itPrevNeighNodeInfo++;
        }
        if (!found) {
            currNodeInfo->neighbourStartTime = simTime();
            previousNeighbourNodeInfoList.push_back(currNodeInfo);
        }

        itCurrNeighNodeInfo++;
    }
}

void KWirelessInterfaceWithAngle::finish()
{
    // remove send msg timeout
    if (sendPacketTimeoutEvent->isScheduled()) {
        cancelEvent(sendPacketTimeoutEvent);
    }
    delete sendPacketTimeoutEvent;

    // remove all messages
    while(!packetQueue.empty()) {
        cMessage *nextMsg = packetQueue.front();
        packetQueue.pop();
        delete nextMsg;
    }
    if (currentPendingMsg != NULL) {

        // TODO: delete is commented out as it gives
        // a OMNeT error saying "use deleteModule()
        // instead of C++ delete
        // delete currentPendingMsg;
        currentPendingMsg = NULL;
    }
}

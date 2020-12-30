//
// Wireless interface used in Keetchi with functionality
// to handle sending of packets considering wireless range
// and broadcast of packets. It should have access to the
// mobility management components.
//
// @date: 08-11-2015
// @author: Asanga Udugama (adu@comnets.uni-bremen.de)
//
// Modified to send also unicast packets in addition to
// broadcast packets
//
// @date: 08-05-2017
// @author: Asanga Udugama (adu@comnets.uni-bremen.de)
//
// Standard WirelessInterface module extended to use contact
// traces.
//
// @date: 01-06-2019
// @author: Thenuka Karunatilake (thenukaramesh@gmail.com)
//

#include "KWirelessInterfaceWithContactTraces.h"

#include "KBaseNodeInfo.h"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <regex>


int nodeIdx = 0;
std::map<std::string, std::ifstream*> fileMap;
std::map<std::string, std::list<string>> nodeRowsMap;
std::map<std::string, int> mytime;
ifstream* filesToBeRead = NULL;
int numOfLines=10;
bool initilizeAtFirst = true;


Define_Module(KWirelessInterfaceWithContactTraces);

void KWirelessInterfaceWithContactTraces::initialize(int stage)
{
    if (initilizeAtFirst) {
        int totalNumNodes = getParentModule()->getParentModule()->par("numNodes");
        filesToBeRead = new ifstream[totalNumNodes];

        initilizeAtFirst = false;
    }

    if (stage == 0) {

        // get parameters
        ownMACAddress = par("ownMACAddress").stringValue();
        wirelessRange = par("wirelessRange");
        neighbourScanInterval = par("neighbourScanInterval");
        bandwidthBitRate = par("bandwidthBitRate");
        wirelessHeaderSize = par("wirelessHeaderSize");
        contactTracesPath = par("contactTracesPath").stringValue();

        // set other parameters
        broadcastMACAddress = "FF:FF:FF:FF:FF:FF";

    } else if (stage == 1) {

        // get own module info
        ownNodeInfo = new KBaseNodeInfo();
        ownNodeInfo->nodeModule = getParentModule();
        cModule *unknownModule = getParentModule()->getSubmodule("mobility");
        ownNodeInfo->nodeMobilityModule = check_and_cast<inet::IMobility*>(unknownModule);
        // ownNodeInfo->nodeWirelessIfcModule = this;

        string nodeName = ownNodeInfo->nodeModule->getFullName();
        string hostnumber = nodeName.substr(nodeName.find("[")+1);
        hostnumber = hostnumber.substr(0, hostnumber.find("]"));
        string fileName = contactTracesPath + "/" + "Result_node[" + hostnumber + "].txt";
        filesToBeRead[nodeIdx].open(fileName, std::ifstream::in);
        if (!filesToBeRead[nodeIdx].is_open()) {
            EV_FATAL <<  KWIRELESSINTERFACEWITHCONTACTTRACES_SIMMODULEINFO << "Contact trace path problem - check contactTracesPath parameter\n";
            endSimulation();
        }
        fileMap[nodeName] = &filesToBeRead[nodeIdx];
        nodeIdx++;

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
                if (dynamic_cast<KWirelessInterfaceWithContactTraces*>(*it) != NULL) {
                    unknownWirelessIfcModule = *it;
                }
            }

            if (unknownMobilityModule != NULL && unknownWirelessIfcModule != NULL) {
                KBaseNodeInfo *nodeInfo = new KBaseNodeInfo();
                nodeInfo->nodeModule = unknownModule;
                nodeInfo->nodeMobilityModule = dynamic_cast<inet::IMobility*>(unknownMobilityModule);
                //nodeInfo->nodeWirelessIfcModule =
                //             dynamic_cast<KWirelessInterfaceWithContactTraces*>(unknownWirelessIfcModule);
                allNodeInfoList.push_back(nodeInfo);
                string nodeFullName=nodeInfo->nodeModule->getFullName();
                string nodeID2 = nodeFullName.substr(nodeFullName.find("[")+1);
                nodeID2 = nodeID2.substr(0, nodeID2.find("]"));
                allNodeInfoList1[stoi(nodeID2)]=nodeInfo;
            }
        }

        // setup first event to build neighbourhood node list and send to forwarding layer
        cMessage *sendNeighEvent = new cMessage("Send Neighbourhood Event");
        sendNeighEvent->setKind(KWIRELESSINTERFACEWITHCONTACTTRACES_NEIGH_EVENT_CODE);
        scheduleAt(simTime() + neighbourScanInterval, sendNeighEvent);

        // setup pkt send event message
        sendPacketTimeoutEvent = new cMessage("Send Packet Timeout Event");
        sendPacketTimeoutEvent->setKind(KWIRELESSINTERFACEWITHCONTACTTRACES_PKTSEND_EVENT_CODE);

        // setup statistics signals
        neighSizeSignal = registerSignal("linkNeighSize");
        neighSizeCountSignal = registerSignal("linkNeighSizeCount");
        contactDurationSignal = registerSignal("linkContactDuration");
        contactDurationCountSignal = registerSignal("linkContactDurationCount");
        contactCountSignal = registerSignal("linkContactCount");

        simpleNeighSizeSignal = registerSignal("linkSimpleNeighSize");

    } else {
        EV_FATAL <<  KWIRELESSINTERFACEWITHCONTACTTRACES_SIMMODULEINFO << "Something is radically wrong\n";
    }
}

int KWirelessInterfaceWithContactTraces::numInitStages() const
{
    return 3;
}

void KWirelessInterfaceWithContactTraces::handleMessage(cMessage *msg)
{

    // find and send neighbour list to upper layer
    if (msg->isSelfMessage() && msg->getKind() == KWIRELESSINTERFACEWITHCONTACTTRACES_NEIGH_EVENT_CODE) {
        if(simTime().dbl() == 1 ||simTime().dbl()==triggerTime1.front()) {

            while (currentNeighbourNodeInfoList1.size() > 0) {
                list<KBaseNodeInfo*>::iterator iteratorCurrentNeighbourNodeInfo1 = currentNeighbourNodeInfoList1.begin();
                KBaseNodeInfo *nodeInfo = *iteratorCurrentNeighbourNodeInfo1;
                currentNeighbourNodeInfoList1.remove(nodeInfo);
            }

            string line;
            string row;
            regex regex("\\ ");
     
            int lastSimTime=mytime[ownNodeInfo->nodeModule->getFullName()];  
    
            if(simTime().dbl() == 1 || lastSimTime == simTime().dbl()) {

                ifstream* infile = fileMap[ownNodeInfo->nodeModule->getFullName()];

                for(int i = 0 ; i < numOfLines ; i++) {
                    if(getline(*infile, row)) {
                        nodeRowsMap[ownNodeInfo->nodeModule->getFullName()].push_back(row);
                        int time11;
                        int time21;
                        vector <string> out1(sregex_token_iterator(row.begin(), row.end(), regex, -1),
                                                sregex_token_iterator());

                        time11 = stod(out1.at(0));
                        time21 = stod(out1.at(2));

                        triggerTime1.push_back(time11);
                        triggerTime1.push_back(time21 + 1);
        
                    } else {
                        break;
                    }
                }
                sort(triggerTime1.begin(), triggerTime1.end());
                triggerTime1.erase( unique( triggerTime1.begin(),triggerTime1.end()),triggerTime1.end());
      
                if(nodeRowsMap[ownNodeInfo->nodeModule->getFullName()].size() > 0) {
                    //get last row (simtime)
                    string lastLine = nodeRowsMap[ownNodeInfo->nodeModule->getFullName()].back();
                    mytime[ownNodeInfo->nodeModule->getFullName()]=stod(lastLine.substr(0, lastLine.find(" ")));  
                }
            }
            if(triggerTime1.size()>1 && simTime().dbl()==triggerTime1.front()) {
                triggerTime1.erase(triggerTime1.begin());
            }

            int trigger1=triggerTime1.front();

            std::list<string>::iterator it = nodeRowsMap[ownNodeInfo->nodeModule->getFullName()].begin();
            while(it != nodeRowsMap[ownNodeInfo->nodeModule->getFullName()].end()) {
                line = it->c_str();

                int nodeName;
                int time1;
                int time2;

                vector <string> out(sregex_token_iterator(line.begin(), line.end(), regex, -1),
                                        sregex_token_iterator());

                time1 = stod(out.at(0));
                time2 = stod(out.at(2));
                nodeName = stoi(out.at(1));

                if(simTime().dbl() < time1){
                    break;
                }

                if(simTime().dbl() > (time2)){
                    it = nodeRowsMap[ownNodeInfo->nodeModule->getFullName()].erase(it);
                } else {
                    ++it;
                }

                //break while loop if relevant node is met
                if(time1 <= simTime().dbl() && simTime().dbl() <= (time2)) {
                    if (find(selectedNodes.begin(), selectedNodes.end(), nodeName) == selectedNodes.end()) {
                        selectedNodes.push_back(nodeName);
                    }
                }
            }

            sort(selectedNodes.begin(), selectedNodes.end());
            for(auto const& value: selectedNodes) {
          
                currentNeighbourNodeInfoList1.push_back(allNodeInfoList1[value]);   
            }
            selectedNodes.clear();
        }

        // compute and emit stats
#ifdef KWIRELESSINTERFACEWITHCONTACTTRACES_COMPUTE_STATS
        generateStats();
#endif
        // if there are neighbours, send message
        if (currentNeighbourNodeInfoList1.size() > 0) {

            // build message
            int neighCount = 0;

            KNeighbourListMsg *neighListMsg = new KNeighbourListMsg("Neighbour List Msg");
            neighListMsg->setNeighbourNameListArraySize(currentNeighbourNodeInfoList1.size());
            neighListMsg->setNeighbourNameCount(currentNeighbourNodeInfoList1.size());

            list<KBaseNodeInfo*>::iterator iteratorCurrentNeighbourNodeInfo = currentNeighbourNodeInfoList1.begin();
            while (iteratorCurrentNeighbourNodeInfo != currentNeighbourNodeInfoList1.end()) {
                KBaseNodeInfo *nodeInfo = *iteratorCurrentNeighbourNodeInfo;

                string nodeAddress = nodeInfo->nodeModule->par("ownAddress").stringValue();
                neighListMsg->setNeighbourNameList(neighCount, nodeAddress.c_str());

                neighCount++;
                iteratorCurrentNeighbourNodeInfo++;
            }

            // send msg to upper layer
            send(neighListMsg, "upperLayerOut");

        }

        // setup next event to build neighbourhood node list and send to forwarding layer
        cMessage *sendNeighEvent = new cMessage("Send Neighbourhood Event");
        sendNeighEvent->setKind(KWIRELESSINTERFACEWITHCONTACTTRACES_NEIGH_EVENT_CODE);
        scheduleAt(simTime() + neighbourScanInterval, sendNeighEvent);

        delete msg;

    // trigger to send pending packet and setup new send
    } else if (msg->isSelfMessage() 
                && msg->getKind() == KWIRELESSINTERFACEWITHCONTACTTRACES_PKTSEND_EVENT_CODE) {

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

            // send msg to upper layer
            send(msg, "upperLayerOut");

        }
    }
}

void KWirelessInterfaceWithContactTraces::setupSendingMsg(cMessage *msg)
{
    string destinationAddress = getDestinationAddress(msg);
    bool isBroadcastMsg = FALSE;
    if (destinationAddress == broadcastMACAddress) {
        isBroadcastMsg = TRUE;
    }

    // make the neighbour list at begining of msg tx (to check later if those neighbours are still there)
    list<KBaseNodeInfo*>::iterator iteratorCurrentNeighbourNodeInfo = currentNeighbourNodeInfoList1.begin();
    while (iteratorCurrentNeighbourNodeInfo != currentNeighbourNodeInfoList1.end()) {
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

}

void KWirelessInterfaceWithContactTraces::sendPendingMsg()
{
    // check if nodes to deliver are still in neighbourhood, if so send the packet
    list<KBaseNodeInfo*>::iterator iteratorAtTxNeighbourNodeInfo = atTxNeighbourNodeInfoList.begin();
    while (iteratorAtTxNeighbourNodeInfo != atTxNeighbourNodeInfoList.end()) {
        KBaseNodeInfo *atTxNeighbourNodeInfo = *iteratorAtTxNeighbourNodeInfo;
        string atTxNeighbourNodeAddress = atTxNeighbourNodeInfo->nodeModule->par("ownAddress").stringValue();

        list<KBaseNodeInfo*>::iterator iteratorCurrentNeighbourNodeInfo = currentNeighbourNodeInfoList1.begin();
        while (iteratorCurrentNeighbourNodeInfo != currentNeighbourNodeInfoList1.end()) {
            KBaseNodeInfo *currentNeighbourNodeInfo = *iteratorCurrentNeighbourNodeInfo;
            string currentNeighbourNodeAddress = currentNeighbourNodeInfo->nodeModule->par("ownAddress").stringValue();

            // check if node is still in neighbourhood
            if (atTxNeighbourNodeAddress == currentNeighbourNodeAddress) {

                // make duplicate of packet
                cPacket *outPktCopy =  dynamic_cast<cPacket*>(currentPendingMsg->dup());

                // send to node
                sendDirect(outPktCopy, currentNeighbourNodeInfo->nodeModule, "radioIn");

                break;
            }

            iteratorCurrentNeighbourNodeInfo++;
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

string KWirelessInterfaceWithContactTraces::getDestinationAddress(cMessage *msg)
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


    EV_FATAL <<  KWIRELESSINTERFACEWITHCONTACTTRACES_SIMMODULEINFO << ">!<Unknown message type. Check \"string KWirelessInterfaceWithContactTraces::getDestinationAddress(cMessage *msg)\"\n";

    throw cRuntimeError("Unknown message type in KWirelessInterfaceWithContactTraces");

    return string();
}


void KWirelessInterfaceWithContactTraces::generateStats()
{

    emit(neighSizeSignal, (long) currentNeighbourNodeInfoList1.size());
    emit(neighSizeCountSignal, 1);

    emit(simpleNeighSizeSignal, (long) currentNeighbourNodeInfoList1.size());

    // check and remove left neighbourhood
    list<KBaseNodeInfo*>::iterator itPrevNeighNodeInfo = previousNeighbourNodeInfoList.begin();
    while (itPrevNeighNodeInfo != previousNeighbourNodeInfoList.end()) {
        KBaseNodeInfo *prevNodeInfo = *itPrevNeighNodeInfo;

        list<KBaseNodeInfo*>::iterator itCurrNeighNodeInfo = currentNeighbourNodeInfoList1.begin();
        bool found = FALSE;
        while (itCurrNeighNodeInfo != currentNeighbourNodeInfoList1.end()) {
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
    list<KBaseNodeInfo*>::iterator itCurrNeighNodeInfo = currentNeighbourNodeInfoList1.begin();
    while (itCurrNeighNodeInfo != currentNeighbourNodeInfoList1.end()) {
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

void KWirelessInterfaceWithContactTraces::finish()
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
    string nodeName = ownNodeInfo->nodeModule->getFullName();
    string fileName = "Result_" + nodeName + ".txt";

    ifstream* toBeClosed = fileMap[nodeName];
    if(toBeClosed->is_open()) {
        toBeClosed->close();
    }
}


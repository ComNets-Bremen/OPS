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

    // process a packet (arriving from upper or lower layers)
    } else {
		
        cGate *gate;
        char gateName[32];
		
       // get message arrival gate name
        gate = msg->getArrivalGate();
        strcpy(gateName, gate->getName());

        if (strstr(gateName, "upperLayerIn") != NULL) {
 			inet::Coord ownCoord = ownNodeInfo->nodeMobilityModule->getCurrentPosition();
			
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
                    simtime_t txDuration = bitsToSend / bandwidthBitRate;

                    sendDirect(outPktCopy, NULL, txDuration, nodeInfo->nodeModule, "radioIn");
					
					// cout <<  KWIRELESSINTERFACE_SIMMODULEINFO << " Sending pkt to " << nodeInfo->nodeModule->getFullName() << "\n";
	 			}
	 			iteratorAllNodeInfo++;
			}
			 
			// remove msg
			delete msg;

         } else { //  from lowerLayerIn

             // send msg to upper layer
			 send(msg, "upperLayerOut");

         }
    }
}

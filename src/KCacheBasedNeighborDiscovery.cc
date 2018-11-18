//
// @date: 08-11-2015
// @author: Asanga Udugama (adu@comnets.uni-bremen.de)
//

#include "KCacheBasedNeighborDiscovery.h"
#include "KWirelessInterface.h"
#include "KBaseNodeInfo.h"
#include "KInternalMsg_m.h"
#include "INeighborCache.h"


Define_Module(KCacheBasedNeighborDiscovery);

KCacheBasedNeighborDiscovery::~KCacheBasedNeighborDiscovery()
{
    cancelAndDelete(neighborScanEvent);
}

void KCacheBasedNeighborDiscovery::initialize(int stage)
{
    if (stage == 0) {
        // get parameters
        wirelessRange = par("wirelessRange");
        neighbourScanInterval = par("neighbourScanInterval");
        neighborCache = check_and_cast<INeighborCache*>(getSystemModule()->getSubmodule("neighborCache"));
    }
    else if (stage == 1) {
        // get own module info
        ownNodeInfo = new KBaseNodeInfo();
        ownNodeInfo->nodeModule = getParentModule();
        ownNodeInfo->nodeWirelessIfcModule = ownNodeInfo->nodeModule->getSubmodule("link");
        ownNodeInfo->nodeMobilityModule = dynamic_cast<inet::IMobility*>(ownNodeInfo->nodeModule->getSubmodule("mobility"));
    }
    else if (stage == 2) {
        // setup first event to build neighbourhood node list and send to forwarding layer
        neighborScanEvent = new cMessage("Send Neighbourhood Event");
        scheduleAt(simTime() + neighbourScanInterval, neighborScanEvent);
    }
    else {
        ASSERT(false);
    }
}

void KCacheBasedNeighborDiscovery::handleMessage(cMessage *msg)
{
    ASSERT(msg == neighborScanEvent);

    currentNeighbourNodeInfoList.clear();
    for (auto nodeInfo : neighborCache->getNeighbors(ownNodeInfo, wirelessRange)) // TODO list <--> vector :-(
        currentNeighbourNodeInfoList.push_back(const_cast<KBaseNodeInfo*>(nodeInfo)); // TODO const mismatch

    // if there are neighbours, send message
    int neighborCount = currentNeighbourNodeInfoList.size();
    if (neighborCount > 0) {
        getParentModule()->bubble((to_string(neighborCount)+" neighbors").c_str());

        // build message
        KNeighbourListMsg *neighListMsg = new KNeighbourListMsg("Neighbour List Msg");
        neighListMsg->setNeighbourNameListArraySize(neighborCount);
        neighListMsg->setNeighbourNameCount(neighborCount);

        int i = 0;
        for (auto nodeInfo : currentNeighbourNodeInfoList) {
            string nodeAddress = nodeInfo->nodeModule->par("ownAddress").stringValue();
            neighListMsg->setNeighbourNameList(i++, nodeAddress.c_str());
        }

        // send msg to upper layer
        send(neighListMsg, "upperLayerOut");
    }

    scheduleAt(simTime() + neighbourScanInterval, neighborScanEvent);
}

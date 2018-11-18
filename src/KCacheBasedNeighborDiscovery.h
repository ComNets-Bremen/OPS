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

#ifndef KCACHEBASEDNEIGHBORDISCOVERY_H_
#define KCACHEBASEDNEIGHBORDISCOVERY_H_

#define TRUE                            1
#define FALSE                           0

#include "inet/mobility/contract/IMobility.h"

#include <omnetpp.h>
#include <cstdlib>
#include <string>
#include <queue>

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

#include "IKNeighborDiscovery.h"

class INeighborCache;

using namespace std;

class KCacheBasedNeighborDiscovery: public cSimpleModule, public IKNeighborDiscovery
{
    protected:
        virtual ~KCacheBasedNeighborDiscovery();
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const { return 3; }

    private:
        double wirelessRange;
        double neighbourScanInterval;
        cMessage *neighborScanEvent = nullptr;
        KBaseNodeInfo *ownNodeInfo = nullptr;
        std::string ownAddress;
        list<KBaseNodeInfo*> currentNeighbourNodeInfoList;
        INeighborCache *neighborCache = nullptr;

    public:
        virtual const list<KBaseNodeInfo*>& getCurrentNeighbourNodeInfoList() const {return currentNeighbourNodeInfoList;}
};

#endif

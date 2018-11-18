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

#ifndef KNEIGHBORDISCOVERY_H_
#define KNEIGHBORDISCOVERY_H_

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

#include "KInternalMsg_m.h"
#include "IKNeighborDiscovery.h"

using namespace std;

class KNeighborDiscovery: public cSimpleModule, public IKNeighborDiscovery
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const { return 3; }
        virtual void refreshDisplay() const;

    private:
        double wirelessRange;
        string expectedNodeTypes;
        double neighbourScanInterval;
        KBaseNodeInfo *ownNodeInfo;
        list<KBaseNodeInfo*> allNodeInfoList;
        list<KBaseNodeInfo*> currentNeighbourNodeInfoList;

    public:
        virtual const list<KBaseNodeInfo*>& getCurrentNeighbourNodeInfoList() const {return currentNeighbourNodeInfoList;}
};

#endif

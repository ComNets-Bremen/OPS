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

#ifndef IKNEIGHBORDISCOVERY_H_
#define IKNEIGHBORDISCOVERY_H_

#include <omnetpp.h>

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;

class KBaseNodeInfo;

class IKNeighborDiscovery
{
    public:
        virtual ~IKNeighborDiscovery() {}
        virtual const list<KBaseNodeInfo*>& getCurrentNeighbourNodeInfoList() const = 0;
};

#endif

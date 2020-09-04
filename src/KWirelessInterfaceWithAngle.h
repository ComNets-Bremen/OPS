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

#ifndef KWIRELESSINTERFACEWITHANGLE_H_
#define KWIRELESSINTERFACEWITHANGLE_H_

#define TRUE                            1
#define FALSE                           0
#define POSITIVE(n)                     ((n) < 0 ? 0 - (n) : (n))
#define MAX(a, b)                       (((a) > (b)) ? (a) :(b))

#include "inet/mobility/contract/IMobility.h"

#include <omnetpp.h>
#include <cstdlib>
#include <string>
#include <queue>

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

using namespace std;

class KBaseNodeInfo;

class KWirelessInterfaceWithAngle: public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();

    private:
        string ownMACAddress;
        double wirelessRange;
        double neighbourScanInterval;
        double bandwidthBitRate;
        int wirelessHeaderSize;
        int calculateSpeedInterval;

        string broadcastMACAddress;
        KBaseNodeInfo *ownNodeInfo;

        list<KBaseNodeInfo*> allNodeInfoList;
        queue<cMessage*> packetQueue;
        cMessage *speedCheckTimer;
        cMessage *sendPacketTimeoutEvent;

        list<KBaseNodeInfo*> currentNeighbourNodeInfoList;
        list<KBaseNodeInfo*> previousNeighbourNodeInfoList;
        list<KBaseNodeInfo*> atTxNeighbourNodeInfoList;
        cMessage *currentPendingMsg;

        void setupSendingMsg(cMessage *msg);
        void sendPendingMsg();
        string getDestinationAddress(cMessage *msg);
        void generateStats();

        // stats related variables
        simsignal_t neighSizeSignal;
        simsignal_t neighSizeCountSignal;
        simsignal_t contactDurationSignal;
        simsignal_t contactDurationCountSignal;
        simsignal_t contactCountSignal;

        simsignal_t simpleNeighSizeSignal;
};

#define KWIRELESSINTERFACEWITHANGLE_SIMMODULEINFO            " KWirelessInterfaceWithAngle>!<" << simTime() << ">!<" << getParentModule()->getFullName()
#define KWIRELESSINTERFACEWITHANGLE_BIT_RATE_10Mbps          10000000
#define KWIRELESSINTERFACEWITHANGLE_NEIGH_EVENT_CODE         112
#define KWIRELESSINTERFACEWITHANGLE_PKTSEND_EVENT_CODE       114
#define KWIRELESSINTERFACEWITHANGLE_SPEEDCHECK_EVENT_CODE    975
#define KWIRELESSINTERFACEWITHANGLE_EUCLIDEAN_DISTANCE               // if defined, then uses Euclidean only,
                                                                     // else, uses Chebyshev first and then Euclidean
#define KWIRELESSINTERFACEWITHANGLE_COMPUTE_STATS            1       // if defined, computes stats for average contact durations
                                                                     // and average neighbour sizes

#endif /* KWIRELESSINTERFACEWITHANGLE_H_ */

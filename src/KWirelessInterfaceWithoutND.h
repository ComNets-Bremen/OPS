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

#ifndef KWIRELESSINTERFACEWITHOUTND_H_
#define KWIRELESSINTERFACEWITHOUTND_H_

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

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

using namespace std;

class KBaseNodeInfo;
class IKNeighborDiscovery;

class KWirelessInterfaceWithoutND: public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();

    private:
        string ownMACAddress;
        double bandwidthBitRate;
        int wirelessHeaderSize;
        int logging;

        string broadcastMACAddress;
        queue<cMessage*> packetQueue;
        cMessage *sendPacketTimeoutEvent;

        list<KBaseNodeInfo*> currentNeighbourNodeInfoList;
        list<KBaseNodeInfo*> atTxNeighbourNodeInfoList;
        cMessage *currentPendingMsg;
        IKNeighborDiscovery *neighborDiscovery;

        long numSent = 0;
        long numReceived = 0;

        void setupSendingMsg(cMessage *msg);
        void sendPendingMsg();
        string getDestinationAddress(cMessage *msg);

};

#endif /* KWIRELESSINTERFACE_H_ */

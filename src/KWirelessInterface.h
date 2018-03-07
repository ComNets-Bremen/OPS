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

#ifndef KWIRELESSINTERFACE_H_
#define KWIRELESSINTERFACE_H_

#define TRUE                            1
#define FALSE                           0

#include "inet/mobility/contract/IMobility.h"

#include <omnetpp.h>
#include <cstdlib>
#include <string>
#include <queue>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

using namespace std;

class KBaseNodeInfo;

class KWirelessInterface: public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();

    private:
        string ownMACAddress;
        double wirelessRange;
        string expectedNodeTypes;
        double neighbourScanInterval;
        double bandwidthBitRate;
        int wirelessHeaderSize;
        int logging;

        string broadcastMACAddress;
        KBaseNodeInfo *ownNodeInfo;
        list<KBaseNodeInfo*> allNodeInfoList;
        queue<cMessage*> packetQueue;
        cMessage *sendPacketTimeoutEvent;

        list<KBaseNodeInfo*> currentNeighbourNodeInfoList;
        list<KBaseNodeInfo*> atTxNeighbourNodeInfoList;
        cMessage *currentPendingMsg;

        void setupSendingMsg(cMessage *msg);
        void sendPendingMsg();
        string getDestinationAddress(cMessage *msg);

};

#define KWIRELESSINTERFACE_SIMMODULEINFO       " KWirelessInterface>!<" << simTime() << ">!<" << getParentModule()->getFullName()
#define KWIRELESSINTERFACE_BIT_RATE_10Mbps     10000000
#define KWIRELESSINTERFACE_NEIGH_EVENT_CODE    112
#define KWIRELESSINTERFACE_PKTSEND_EVENT_CODE  114


#endif /* KWIRELESSINTERFACE_H_ */

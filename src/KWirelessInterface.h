//
// Wireless interface used in Keetchi with functionality
// to handle sending of packets considering wireless range
// and broadcast of packets. It should have access to the
// mobility management components.
//
// @date: 08-11-2015
// @author: Asanga Udugama (adu@comnets.uni-bremen.de)

#ifndef KWIRELESSINTERFACE_H_
#define KWIRELESSINTERFACE_H_

#include "inet/mobility/contract/IMobility.h"

#include <omnetpp.h>
#include <cstdlib>
#include <string>


#include "KKeetchiMsg_m.h"
#include "KInternalMsg_m.h"

using namespace std;

class KBaseNodeInfo;

class KWirelessInterface: public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;

    private:
        string ownMACAddress;
        double wirelessRange;
		string expectedNodeTypes;
		double neighbourScanInterval;
        double bandwidthBitRate;
        int wirelessHeaderSize;

        string broadcastMACAddress;
        KBaseNodeInfo *ownNodeInfo;
		list<KBaseNodeInfo*> allNodeInfoList;

};

#define KWIRELESSINTERFACE_SIMMODULEINFO       " :: " << simTime() << " :: " << getParentModule()->getFullName() << " :: KWirelessInterface"
#define KWIRELESSINTERFACE_BIT_RATE_10Mbps     10000000
#define KWIRELESSINTERFACE_NEIGH_EVENT_CODE    112


#endif /* KWIRELESSINTERFACE_H_ */

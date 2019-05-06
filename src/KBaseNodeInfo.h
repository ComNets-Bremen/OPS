
#ifndef KBASENODEINFO_H_
#define KBASENODEINFO_H_
#include "inet/mobility/contract/IMobility.h"

#include <omnetpp.h>
#include <cstdlib>
#include <string>


#include "KWirelessInterface.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;

class KWirelessInterface;

class KBaseNodeInfo
{
    public:
        string nodeAddress;

        // mobility related info
        cModule *nodeModule;
		inet::IMobility *nodeMobilityModule;
		KWirelessInterface *nodeWirelessIfcModule;

        // variables to collect stats
        long likedDataBytesReceived = 0;
        long nonLikedDataBytesReceived = 0;
        long duplicateDataBytesReceived = 0;

        long likedDataCountReceived = 0;
        long nonLikedDataCountReceived = 0;
        long duplicateDataCountReceived = 0;

        long likedDataBytesMaxReceivable = 0;
        long nonLikedDataBytesMaxReceivable = 0;

        long likedDataCountMaxReceivable = 0;
        long nonLikedDataCountMaxReceivable = 0;

        simtime_t likedDataReceivedAvgDelay = 0.0;
        simtime_t nonLikedDataReceivedAvgDelay = 0.0;
        simtime_t totalDataReceivedAvgDelay = 0.0;

        double likedDataDeliveryRatio = 0.0;
        double nonLikedDataDeliveryRatio = 0.0;
        double totalDataDeliveryRatio = 0.0;
        
        simsignal_t likedDataReceivableSignalID;
};

#endif /* KBASENODEINFO_H_ */

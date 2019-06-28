
#ifndef KBASENODEINFO_H_
#define KBASENODEINFO_H_
#include "inet/mobility/contract/IMobility.h"

#include <omnetpp.h>
#include <cstdlib>
#include <string>


#include "KWirelessInterface.h"
#include "KMessengerApp.h"
#include "KHeraldApp.h"

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

		// messenger app related info
        KMessengerApp *nodeMessengerAppModule;

        // herald app related info
        KHeraldApp *nodeHeraldAppModule;

        // used for neighbour contact durations
        simtime_t neighbourStartTime;

};

#endif /* KBASENODEINFO_H_ */

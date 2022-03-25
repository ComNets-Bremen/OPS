
#ifndef KBASENODEINFO_H_
#define KBASENODEINFO_H_
#include "inet/mobility/contract/IMobility.h"

#include <omnetpp.h>
#include <cstdlib>
#include <string>


#include "KMessengerApp.h"
#include "KMessengerAppForDifferentiatedTraffic.h"
#include "KHeraldApp.h"
#include "KHeraldAppForDifferentiatedTraffic.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;

class KBaseNodeInfo
{
    public:
        string nodeAddress;
        double nodeSpeed;
        double nodeAngle;

        // mobility related info
        cModule *nodeModule;
		inet::IMobility *nodeMobilityModule;
        inet::Coord lastPosition;
        bool firstTimePosition;
		//KWirelessInterface *nodeWirelessIfcModule;

		// messenger app related info
        KMessengerApp *nodeMessengerAppModule;
	KMessengerAppForDifferentiatedTraffic *nodeMessengerAppForDifferentiatedTrafficModule;

        // herald app related info
        KHeraldApp *nodeHeraldAppModule;
        KHeraldAppForDifferentiatedTraffic *nodeHeraldAppForDifferentiatedTrafficModule;

        // used for neighbour contact durations
        simtime_t neighbourStartTime;

        // the unique node name assigned by OMNeT - e.g., host[23]
        string nodeName;
};

#endif /* KBASENODEINFO_H_ */

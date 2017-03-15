
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

        cModule *nodeModule;
		inet::IMobility *nodeMobilityModule;
		KWirelessInterface *nodeWirelessIfcModule;



};

#endif /* KBASENODEINFO_H_ */

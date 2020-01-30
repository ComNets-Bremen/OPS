//
// Classes that implement OPS specific statistics filters and recorders
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 21-jan-2020
//
//
#ifndef KOPSSTATS_H_
#define KOPSSTATS_H_

#include <omnetpp.h>
#include <cstdlib>
#include <ctime>
#include <list>
#include <string>

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;

class KOPSStats {
};

class KOPSStatsPeriodicAverageForAllNodesFilter : public cNumericResultFilter
{
    protected:
        double sum;
        long numNodes;
        bool numNodesFound;
        long receiptCount;

    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details);

    public:
        KOPSStatsPeriodicAverageForAllNodesFilter() {sum = 0; numNodesFound = false; receiptCount = 0; numNodes = 0;}
};



#endif /* KOPSSTATS_H_ */

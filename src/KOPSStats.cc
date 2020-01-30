//
// Classes that implement OPS specific statistics filters and recorders
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 21-jan-2020
//
//

#include "KOPSStats.h"

Register_ResultFilter("periodicAverageForAllNodes", KOPSStatsPeriodicAverageForAllNodesFilter);

bool KOPSStatsPeriodicAverageForAllNodesFilter::process(simtime_t& t, double& value, cObject *)
{
    if (!numNodesFound) {
        cSimulation *sim = cSimulation::getActiveSimulation();
        for (int id = 0; id <= sim->getLastComponentId(); id++) {
            cModule *unknownModule = sim->getModule(id);
            if (unknownModule == nullptr) {
                continue;
            }
            if (unknownModule->hasPar("numNodes")) {
                numNodes = unknownModule->par("numNodes");
                numNodesFound = true;
                break;
            }
        }
    }

    if (numNodesFound) {

        sum += value;
        receiptCount++;
        if (receiptCount == numNodes) {
            double avg = sum / numNodes;
            value = avg;
            receiptCount = 0;
            sum = 0;

            return true;
        }
    }

    value = 0.0;
    return false;
}

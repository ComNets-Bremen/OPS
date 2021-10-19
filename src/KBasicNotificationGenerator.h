//
// The model is the implementation of a basic Notification Generator
// for OPS Simulator
//
// @date   : 10-07-2016
// @author : Anas bin Muslim (anas1@uni-bremen.de)
//
// Change History:
// Asanga Udugama (adu@comnets.uni-bremen.de)
// - Locations file as a parameter 
// - use of interface
// - change of class name
//

#ifndef __KBASICNOTIFICATIONGENERATOR_H_
#define __KBASICNOTIFICATIONGENERATOR_H_

#include "KBasicUBMApp.h"
#include "INotificationGenerator.h"
#include "IUBM.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;

#define KBASICNOTIFICATIONGENERATOR_SIMMODULEINFO       " KBasicNotificationGenerator>!<" << simTime() << ">!<" << this->getFullName()

#define KBASICNOTIFICATIONGENERATOR_DISTR_NONE          1 // constant time intervals
#define KBASICNOTIFICATIONGENERATOR_DISTR_EXPONENTIAL   2 // exponential time intervals
#define KBASICNOTIFICATIONGENERATOR_DISTR_UNIFORM       3 // uniformly distributed time intervals


class KBasicNotificationGenerator : public cSimpleModule, public INotificationGenerator
{
    private:
        int interEventTimeDuration;                   // Maximum time difference between two events
        int nodes;                                    // Number of nodes present in simualation of the type KUBMNode
        int count;                                    // Number of the locations
        int radius;                                   // Radius of location to create cicle
        int usedRNG;                                  // RNG used for Random number generation
        int notificationGenDistribution;              // Random number distribution to use

//        int logging;

        bool eventsRead;                              // Boolean; whether events have been read from file
        
        string appPrefix;
        string appNameGenPrefix;

        string locationsFilePath;                     // path of locations file
        string eventsFilePath;                        // path of events file

        int dataSizeInBytes;                          // size of data payload

        std::vector<std::vector<int>> locations;      // Vector which stores all of the locations
        std::vector<int> nodesID;                     // Vector which stores all of the node IDs; used to send events to a node randomly
        std::vector <std::string> allEvents;          // Vector which stores all of the events
        
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
		virtual void finish();
		
		bool readEvents();                           // Function which read events from file and stores in a vector

        simsignal_t genTotalReceivableSignal;
		
};

#endif /* __KBASICNOTIFICATIONGENERATOR_H_ */

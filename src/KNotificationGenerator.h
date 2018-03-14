//
// The model is the implementation of the Notification Generator
// for Keetchi Simulator
//
// @date   : 10-07-2016
// @author : Anas bin Muslim (anas1@uni-bremen.de)
//
// Change History:
// Asanga Udugama (adu@comnets.uni-bremen.de)
// - Locations file as a parameter 
//

#ifndef __KNOTIFICATIONGENERATOR_H_
#define __KNOTIFICATIONGENERATOR_H_

#include "KBasicUBMApp.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;

#define KNOTIFICATIONGENERATOR_SIMMODULEINFO       " KNotificationGenerator>!<" << simTime() << ">!<" << this->getFullName()

#define KNOTIFICATIONGENERATOR_DISTR_NONE          1 // constant time intervals
#define KNOTIFICATIONGENERATOR_DISTR_EXPONENTIAL   2 // exponential time intervals
#define KNOTIFICATIONGENERATOR_DISTR_UNIFORM       3 // uniformly distributed time intervals


class KNotificationGenerator : public cSimpleModule
{
    private:
        int interEventTimeDuration;                   // Maximum time difference between two events
        int nodes;                                    // Number of nodes present in simualation of the type KUBMNode
        int count;                                    // Number of the locations
        int radius;                                   // Radius of location to create cicle
        int usedRNG;                                  // RNG used for Random number generation
        int notificationGenDistribution;              // Random number distribution to use
        
        int logging;
        
        bool eventsRead;                              // Boolean; whether events have been read from file
        
        string appPrefix;
        string appNameGenPrefix;

        string locationsFilePath;                     // path of locations file
        string eventsFilePath;                        // path of events file
        
        std::vector<std::vector<int>> locations;      // Vector which stores all of the locations
        std::vector<int> nodesID;                     // Vector which stores all of the node IDs; used to send events to a node randomly
        std::vector <std::string> allEvents;          // Vector which stores all of the events
        
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
		virtual void finish();
		
		bool readEvents();                           // Function which read events from file and stores in a vector

    public:
		
};

#endif /* __KٰNOTIFICATIONGENERATOR_H_ */

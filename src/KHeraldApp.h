//
// The model implementation for the Herald application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de), Anna Förster (afoerster@uni-bremen.de)
// @date   : 15-aug-2016, updated 6-febr-2018
//
//

#ifndef KHERALDAPP_H_
#define KHERALDAPP_H_

#include <omnetpp.h>
#include <cstdlib>
#include <ctime>
#include <list>
#include <string>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif


using namespace std;


#define KHERALDAPP_SIMMODULEINFO        " KHeraldApp>!<" << simTime() << ">!<" << getParentModule()->getFullName()
#define TRUE                            1
#define FALSE                           0
#define KHERALDAPP_MSGTYPE_NONE         0
#define KHERALDAPP_MSGTYPE_IMMEDIATE    1
#define KHERALDAPP_MSGTYPE_PREFERENCE   2
#define KHERALDAPP_START_ITEM_ID        22000
#define KHERALDAPP_REGISTRATION_EVENT	92
#define KHERALDAPP_DATASEND_EVENT		93



class KHeraldApp : public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();

    private:

        double totalSimulationTime;
        char prefixName[128] = "/herald";

		double ttl;
        int nodeIndex;
        int totalNumNodes;
        double dataGenerationInterval;
        int nextGenerationNotification;

        int notificationCount;
        vector<int> myLikenesses;
        vector<int> timesMessagesReceived;

        int usedRNG;

        cMessage *appRegistrationEvent;
        cMessage *dataTimeoutEvent;

        double popularityAssignmentPercentage;
        int dataSizeInBytes;
        int logging;

};


#endif /* KHERALDAPP_H_ */

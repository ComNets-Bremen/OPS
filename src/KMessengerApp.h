//
// The model implementation for the Messenger application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de), Anna Förster (afoerster@uni-bremen.de)
// @date   : 15-aug-2016, updated 6-febr-2018
//
//

#ifndef KMESSENGERAPP_H_
#define KMESSENGERAPP_H_

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


#define KMESSENGERAPP_SIMMODULEINFO        " KMessengerApp>!<" << simTime() << ">!<" << getParentModule()->getFullName()
#define TRUE                            1
#define FALSE                           0
#define KMESSENGERAPP_MSGTYPE_NONE         0
#define KMESSENGERAPP_MSGTYPE_IMMEDIATE    1
#define KMESSENGERAPP_MSGTYPE_PREFERENCE   2
#define KMESSENGERAPP_START_ITEM_ID        22000
#define KMESSENGERAPP_REGISTRATION_EVENT	92
#define KMESSENGERAPP_DATASEND_EVENT		93



class KMessengerApp : public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();

    private:
        string ownMACAddress;

        double totalSimulationTime;
        char prefixName[128] = "/messenger";

		double ttl;
        int nodeIndex;
        int totalNumNodes;
        double dataGenerationInterval;
        int nextGenerationNotification;

        int notificationCount;
        vector<int> timesMessagesReceived;

        int usedRNG;

        cMessage *appRegistrationEvent;
        cMessage *dataTimeoutEvent;

        int dataSizeInBytes;
        int logging;

};


#endif /* KMESSENGERAPP_H_ */

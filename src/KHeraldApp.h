//
// The model implementation for the Herald application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 15-aug-2016
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


#define KHERALDAPP_SIMMODULEINFO        ">!<" << simTime() << ">!<" << getParentModule()->getFullName()
#define TRUE                            1
#define FALSE                           0
#define KHERALDAPP_MSGTYPE_NONE         0
#define KHERALDAPP_MSGTYPE_IMMEDIATE    1
#define KHERALDAPP_MSGTYPE_PREFERENCE   2


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

        struct NotificationItem {
            int itemNumber;
            char dataName[128];
            char dummyPayloadContent[128];
            char dataMsgName[128];
            char feedbackMsgName[128];

            int popularity;
            int likeness;
            int goodnessValue;

            int realPayloadSize;
            int dataByteLength;
            int feedbackByteLength;

            double validUntilTime;

            int timesDataMsgReceived;
            int feedbackGenerated;


        };

        int nodeIndex;
        int dataGeneratingNodeIndex;
        double dataGenerationInterval;
        double feedbackGenerationInterval;
        double maxFeedbackGenerationInterval;

        int notificationCount;
        list<NotificationItem*> notificationList;

        int usedRNG;

        cMessage *appRegistrationEvent;
        cMessage *dataTimeoutEvent;
        cMessage *feedbackTimeoutEvent;

        double popularityAssignmentPercentage;
        int dataSizeInBytes;
        int logging;

};


#endif /* KHERALDAPP_H_ */

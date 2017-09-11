//
// The model implementation for the Uni Recycler application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 18-sep-2015
//
//

#ifndef KBRUITAPP_H_
#define KBRUITAPP_H_

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

class KBruitApp : public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();

    private:
        int nodeIndex;
        int operationMode;
        int dataGeneratingNodeIndex;
        int numberOfData;
        string appPrefix;
        string appNameGenPrefix;
        double dataGenerationInterval;
        double feedbackGenerationInterval;
        int logging;
		
		cMessage *appRegistrationEvent;
		cMessage *dataTimeoutEvent;
		cMessage *feedbackTimeoutEvent;
		
        int generatedDataCount;

        list<string*> knownDataNameList;

        simsignal_t dataSendSignal;
        simsignal_t dataReceiveSignal;
        simsignal_t feedbackSendSignal;

};

#define KBRUITAPP_SIMMODULEINFO       ">!<" << simTime() << ">!<" << getParentModule()->getFullName()

#endif /* KBRUITAPP_H_ */

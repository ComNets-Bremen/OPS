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
#include "KBaseNodeInfo.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif


using namespace std;


#define KMESSENGERAPP_SIMMODULEINFO         " KMessengerApp>!<" << simTime() << ">!<" << getParentModule()->getFullName()
#define TRUE                                1
#define FALSE                               0
#define KMESSENGERAPP_MSGTYPE_NONE          0
#define KMESSENGERAPP_MSGTYPE_IMMEDIATE     1
#define KMESSENGERAPP_MSGTYPE_PREFERENCE    2
#define KMESSENGERAPP_START_ITEM_ID         22000
#define KMESSENGERAPP_REGISTRATION_EVENT	92
#define KMESSENGERAPP_DATASEND_EVENT		93


class KMessengerApp : public cSimpleModule, public cListener
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *value, cObject *details);

    private:
        string ownMACAddress;

        double totalSimulationTime;
        char prefixName[128] = "/messenger";

		double ttl;
        int nodeIndex;
        int totalNumNodes;
        double dataGenerationInterval;
        int notificationCount;
        
        int nextGenerationIndex;

        int usedRNG;

        cMessage *appRegistrationEvent;
        cMessage *dataTimeoutEvent;

        int dataSizeInBytes;
        int logging;

        KBaseNodeInfo* ownNodeInfo;
        
        // variables to generate signals
        simsignal_t likedDataBytesReceivedSignal;
        simsignal_t nonLikedDataBytesReceivedSignal;
        simsignal_t duplicateDataBytesReceivedSignal;
        simsignal_t totalDataBytesReceivedSignal;

        simsignal_t likedDataCountReceivedSignal;
        simsignal_t nonLikedDataCountReceivedSignal;
        simsignal_t duplicateDataCountReceivedSignal;
        simsignal_t totalDataCountReceivedSignal;

        simsignal_t likedDataBytesMaxReceivableSignal;
        simsignal_t nonLikedDataBytesMaxReceivableSignal;
        simsignal_t totalDataBytesMaxReceivableSignal;

        simsignal_t likedDataCountMaxReceivableSignal;
        simsignal_t nonLikedDataCountMaxReceivableSignal;
        simsignal_t totalDataCountMaxReceivableSignal;

        simsignal_t likedDataReceivedAvgDelaySignal;
        simsignal_t nonLikedDataReceivedAvgDelaySignal;
        simsignal_t totalDataReceivedAvgDelaySignal;

        simsignal_t likedDataDeliveryRatioSignal;
        simsignal_t nonLikedDataDeliveryRatioSignal;
        simsignal_t totalDataDeliveryRatioSignal;

};


#endif /* KMESSENGERAPP_H_ */

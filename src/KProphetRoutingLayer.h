//
//The model for the Prophet Routing Layer
//
// @author : Kirishanth Chethuraja
// @date   : 25-feb-2019
//
//

#ifndef KPROPHETROUTINGLAYER_H_
#define KPROPHETROUTINGLAYER_H_

#define TRUE                            1
#define FALSE                           0

#include <omnetpp.h>
#include <cstdlib>
#include <sstream>
#include <string>
#include <algorithm>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;

class KProphetRoutingLayer: public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();


    private:
        string ownMACAddress;
        int nextAppID;
        int maximumCacheSize;
        double antiEntropyInterval;
        int maximumHopCount;
        double maximumRandomBackoffDuration;
        bool useTTL;
        int numEventsHandled;
        int currentCacheSize;
        double pEncounterFirst;
        double pFirstThreshold;
        double pEncounterMax;
        double beta;
        double gamma;
        double delta;
        double alpha;
//        double agingTimeUnit;
        double lastTimeAged;
        double standardTimeInterval;
        int usedRNG;

        struct AppInfo {
            int appID;
            string appName;
            string prefixName;
        };

        struct CacheEntry {
            string messageID;
            int hopCount;
            string dataName;
            int realPayloadSize;
            string dummyPayloadContent;
            simtime_t validUntilTime;
            int realPacketSize;
            bool destinationOriented;
            //string originatorNodeName;

//            string initialOriginatorAddress;
//            string finalDestinationNodeName;
//            string finalDestinationNodeAddress;

            string initialOriginatorAddress;
            string finalDestinationAddress;

            int goodnessValue;
            int hopsTravelled;

            int msgUniqueID;
            simtime_t initialInjectionTime;

            double createdTime;
            double updatedTime;
            double lastAccessedTime;

        };

        struct SyncedNeighbour {
            string nodeMACAddress;
            double syncCoolOffEndTime;

            bool randomBackoffStarted;
            double randomBackoffEndTime;

            bool neighbourSyncing;
            double neighbourSyncEndTime;

            bool nodeConsidered;

            int numPreviousEncounter;
            double lastSyncTime;
            double totalAEI;
            double aEIValue;

        };

        struct DeliveryPredictability {
            string nodeMACAddress;
            double nodeDP;
        };



        list<AppInfo*> registeredAppList;
        list<CacheEntry*> cacheList;
        list<SyncedNeighbour*> syncedNeighbourList;
        list<DeliveryPredictability*> dpList;
        list<DeliveryPredictability*> dpListReceived;
        bool syncedNeighbourListIHasChanged;


//        cMessage *ageDataTimeoutEvent;

        void ageDataInCache();
        void handleAppRegistrationMsg(cMessage *msg);
        void handleDataMsgFromUpperLayer(cMessage *msg);
        void handleNeighbourListMsgFromLowerLayer(cMessage *msg);
        void handleDataMsgFromLowerLayer(cMessage *msg);
        void sendDataMsg(vector<string> destinationNodes, string nodeMACAddress);
        //Receives the DPT table request and send its DP table
        void handleDPTableRequestFromLowerLayer(cMessage *msg);
        // Receives the Encountered Node's DPT
        void handleDPTableDataFromLowerLayer(cMessage *msg);

        //TO set Last synch time
        void disconnectEncounterInterval(string nodeMACAddress);
        //To update DP value based on Aging
        int agingDP();
        //Updates its own Table with the Received Table
        void updateDPT(string nodeMACAddress);

        SyncedNeighbour* getSyncingNeighbourInfo(string nodeMACAddress);
        void setSyncingNeighbourInfoForNextRound();
        void setSyncingNeighbourInfoForNoNeighboursOrEmptyCache();
        void updateNeighbourSyncStarted(string nodeMACAddress , string ownMACAddress );

        // stats related variables
        simsignal_t dataBytesReceivedSignal;
        simsignal_t sumVecBytesReceivedSignal;
        simsignal_t dataReqBytesReceivedSignal;
        simsignal_t dpTableRequestBytesReceived;
        simsignal_t dpTableDataBytesReceived;
        simsignal_t totalBytesReceivedSignal;
        simsignal_t hopsTravelledSignal;
        simsignal_t hopsTravelledCountSignal;
        simsignal_t cacheBytesRemovedSignal;
        simsignal_t cacheBytesAddedSignal;
        simsignal_t cacheBytesUpdatedSignal;
        simsignal_t currentCacheSizeBytesSignal;
        simsignal_t currentCacheSizeReportedCountSignal;

};
#define KPROPHETROUTINGLAYER_SIMMODULEINFO         ">!<" << simTime() << ">!<" << getParentModule()->getFullName()
#define KPROPHETROUTINGLAYER_DEBUG                 ">!<DEBUG>!<" << ownMACAddress

#define KPROPHETROUTINGLAYER_MSG_ID_HASH_SIZE      4 // in bytes

#endif /* KPROPHETROUTINGLAYER_H_ */

//
// The model implementation for the BubbleRAP Forwarding with Epidemic Routing layer foundation
//
// @author : Akhil Simha Neela (neela@uni-bremen.de)
// @date   : 17-10-2023
//


#ifndef KBUBBLERAPFORWARDING_H_
#define KBUBBLERAPFORWARDING_H_

#define TRUE                            1
#define FALSE                           0

#include <omnetpp.h>
#include <cstdlib>
#include <sstream>
#include <string>
#include <algorithm>
#include <fstream>


#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

using namespace omnetpp;

using namespace std;

class KBubbleRAPForwarding : public cSimpleModule
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
        int usedRNG;
        double cacheSizeReportingFrequency;

        int numEventsHandled;
        int currentCacheSize;

        cMessage *cacheSizeReportingTimeoutEvent;

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

        };

        list<AppInfo*> registeredAppList;
        vector<CacheEntry> cacheList;
        list<SyncedNeighbour*> syncedNeighbourList;
        bool syncedNeighbourListIHasChanged;

        void ageDataInCache();
        void handleAppRegistrationMsg(cMessage *msg);
        void handleDataMsgFromUpperLayer(cMessage *msg);
        void handleNeighbourListMsgFromLowerLayer(cMessage *msg);
        void handleDataMsgFromLowerLayer(cMessage *msg);
        void handleSummaryVectorMsgFromLowerLayer(cMessage *msg);
        void handleDataRequestMsgFromLowerLayer(cMessage *msg);

        SyncedNeighbour* getSyncingNeighbourInfo(string nodeMACAddress);
        void setSyncingNeighbourInfoForNextRound();
        void setSyncingNeighbourInfoForNoNeighboursOrEmptyCache();
        KSummaryVectorMsg* makeSummaryVectorMessage(string);
        bool isMsgSent(string, string);

        //bool compareMessageID(CacheEntry a, CacheEntry b);

        // stats related variables
        simsignal_t dataBytesReceivedSignal;
        simsignal_t sumVecBytesReceivedSignal;
        simsignal_t dataReqBytesReceivedSignal;
        simsignal_t totalBytesReceivedSignal;

        simsignal_t hopsTravelledSignal;
        simsignal_t hopsTravelledCountSignal;

        simsignal_t cacheBytesRemovedSignal;
        simsignal_t cacheBytesAddedSignal;
        simsignal_t cacheBytesUpdatedSignal;
        simsignal_t currentCacheSizeBytesSignal;
        simsignal_t currentCacheSizeReportedCountSignal;
        simsignal_t currentCacheSizeBytesPeriodicSignal;

        simsignal_t currentCacheSizeBytesSignal2;

        simsignal_t dataBytesSentSignal;
        simsignal_t sumVecBytesSentSignal;
        simsignal_t dataReqBytesSentSignal;
        simsignal_t totalBytesSentSignal;

};

#define KBUBBLERAPFORWARDING_SIMMODULEINFO         " KBubbleRAPForwarding>!<" << simTime() << ">!<" << getParentModule()->getFullName()

#define KBUBBLERAPFORWARDING_MSG_ID_HASH_SIZE      4 // in bytes
#define KBUBBLERAPFORWARDING_CACHESIZE_REP_EVENT   175

#endif /* KBUBBLERAPFORWARDING_H_ */

//
// The model implementation for the Epidemic Routing layer
//
// @author : Jibin John (adu@comnets.uni-bremen.de)
// @date   : 02-may-2017
//
//

#ifndef KSPRAYWAITROUTINGLAYER_H_
#define KSPRAYWAITROUTINGLAYER_H_

#define TRUE                            1
#define FALSE                           0

#include <omnetpp.h>
#include <cstdlib>
#include <sstream>
#include <string>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;

class KSpraywaitRoutingLayer: public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();

    private:
        string ownMACAddress;
        string sprayFlavour;
        int nextAppID;
        int maximumCacheSize;
        double antiEntropyInterval;
        int maximumHopCount;
        double maximumRandomBackoffDuration;
        int usedRNG;
        double cacheSizeReportingFrequency;

        int newcopies;
        int L;

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
            int copies;
            string dataName;
            int realPayloadSize;
            string dummyPayloadContent;

            simtime_t validUntilTime;

            int realPacketSize;

            bool destinationOriented;
            // string originatorNodeName;
            // string finalDestinationNodeName;
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
        list<CacheEntry*> cacheList;
        list<SyncedNeighbour*> syncedNeighbourList;
        bool syncedNeighbourListIHasChanged;

        cMessage *ageDataTimeoutEvent;

        void handleDataAgingTrigger(cMessage *msg);
        void handleAppRegistrationMsg(cMessage *msg);
        void handleDataMsgFromUpperLayer(cMessage *msg);
        void handleNeighbourListMsgFromLowerLayer(cMessage *msg);
        void handleDataMsgFromLowerLayer(cMessage *msg);
        void handleSummaryVectorMsgFromLowerLayer(cMessage *msg);
        void handleDataRequestMsgFromLowerLayer(cMessage *msg);

        SyncedNeighbour* getSyncingNeighbourInfo(string nodeMACAddress);
        void setSyncingNeighbourInfoForNextRound();
        void setSyncingNeighbourInfoForNoNeighboursOrEmptyCache();
        KSummaryVectorMsg* makeSummaryVectorMessage();

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
#define KSPRAYWAITROUTINGLAYER_SIMMODULEINFO          ">!<" << simTime() << ">!<" << getParentModule()->getFullName()
#define KSPRAYWAITROUTINGLAYER_DEBUG                  ">!<DEBUG>!<" << ownMACAddress

#define KSPRAYWAITROUTINGLAYER_MSG_ID_HASH_SIZE      4 // in bytes
#define KSPRAYWAITROUTINGLAYER_EVENTTYPE_AGEDATA     108
#define KSPRAYWAITROUTINGLAYER_CACHESIZE_REP_EVENT   175

#endif /* KEPIDEMICROUTINGLAYER_H_ */

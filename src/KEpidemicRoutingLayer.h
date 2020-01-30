//
// The model implementation for the Epidemic Routing layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 02-may-2017
//
//

#ifndef KEPIDEMICROUTINGLAYER_H_
#define KEPIDEMICROUTINGLAYER_H_

#define TRUE                            1
#define FALSE                           0

#include <omnetpp.h>
#include <cstdlib>
#include <sstream>
#include <string>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

using namespace omnetpp;

using namespace std;

class KEpidemicRoutingLayer : public cSimpleModule
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
//        string originatorNodeName;
//        string finalDestinationNodeName;
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

    simsignal_t dataBytesSentSignal;
    simsignal_t sumVecBytesSentSignal;
    simsignal_t dataReqBytesSentSignal;
    simsignal_t totalBytesSentSignal;

};

#define KEPIDEMICROUTINGLAYER_SIMMODULEINFO         " KEpidemicRoutingLayer>!<" << simTime() << ">!<" << getParentModule()->getFullName()

#define KEPIDEMICROUTINGLAYER_MSG_ID_HASH_SIZE      4 // in bytes
#define KEPIDEMICROUTINGLAYER_CACHESIZE_REP_EVENT   175

#endif /* KEPIDEMICROUTINGLAYER_H_ */

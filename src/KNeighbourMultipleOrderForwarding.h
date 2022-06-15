//
// The model for the Epidemic Routing where neighbours
// are ordered based on a selected set of criteria.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 02-may-2017
//
//

#ifndef KNEIGHBOURMULTIPLEORDERFORWARDING_H_
#define KNEIGHBOURMULTIPLEORDERFORWARDING_H_

#define TRUE                            1
#define FALSE                           0

#include <omnetpp.h>
#include <cstdlib>
#include <sstream>
#include <string>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"
#include "KBaseNodeInfo.h"
#include "inet/common/geometry/common/Coord.h"

using namespace omnetpp;

using namespace std;

using namespace inet;

class KNeighbourMultipleOrderForwarding: public cSimpleModule
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
        string orderType;

        int numEventsHandled;
        int currentCacheSize;

        cMessage *cacheSizeReportingTimeoutEvent;

        int higherThreshold = 4;
        int lowerThreshold = 2;
        int velocity = 2; // 1 = high speed; 2 = low speed

        inet::IMobility *nodeMobility;
        inet::Coord lastPosition;
        cMessage *speedCheckTimer;

        int currentForwarder = 2; // 1 = RRS, 2 = Epidemic
        bool firstTime = true;

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

        struct NeighPrioEntry {
            string nodeMACAddress;
            int neighIndex;
            int priority1;
            int priority2;
            int priority3;
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
        bool isAngleBetween(double angle, double lowerBoundary, double upperBoundary);

        vector<NeighPrioEntry *> prioritizeAngleRSSISpeed(KNeighbourListMsg *neighListMsg);
        vector<NeighPrioEntry *> prioritizeSpeedRSSIAngle(neighListMsg);
        vector<NeighPrioEntry *> prioritizeAngleSpeedRSSI(neighListMsg);
        vector<NeighPrioEntry *> prioritizeAngleSpeed(neighListMsg);
        vector<NeighPrioEntry *> prioritizeSpeedAngle(neighListMsg);
        vector<NeighPrioEntry *> prioritizeSpeedRSSI(neighListMsg);

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

#define KNEIGHBOURMULTIPLEORDERFORWARDING_SIMMODULEINFO         " KNeighbourMultipleOrderForwarding>!<" << simTime() << ">!<" << getParentModule()->getFullName()

#define KNEIGHBOURMULTIPLEORDERFORWARDING_MSG_ID_HASH_SIZE      4 // in bytes
#define KNEIGHBOURMULTIPLEORDERFORWARDING_CACHESIZE_REP_EVENT   175

#define PRIORITY_SAME_DIRECTION             1
#define PRIORITY_OPPOSITE_DIRECTION         2
#define PRIORITY_ADJACENT_LEFT_TOP          3
#define PRIORITY_ADJACENT_LEFT_BOTTOM       4
#define PRIORITY_ADJACENT_RIGHT_TOP         5
#define PRIORITY_ADJACENT_RIGHT_BOTTOM      6
#define PRIORITY_OTHER                      99
#define PRIORITY_STATIONARY                 100

#endif /* KNEIGHBOURMULTIPLEORDERFORWARDING_H_ */

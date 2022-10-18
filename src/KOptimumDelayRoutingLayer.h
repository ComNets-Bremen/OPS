// The model implementation of an Optimal Routing layer (delivery delay version)
//
// @author : Kirishanth Chethuraja
// @date   : 19-oct-2019
//
// Update for direct delivery of packets and stats.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 25-apr-2020
//
// Cache Modification - C++ map
// @author : Hai Thien Long Thai (hthai@uni-bremen.de, thaihaithienlong@yahoo.com)
// @date   : sept-2022

#ifndef KOPTIMUMDELAYROUTINGLAYER_H_
#define KOPTIMUMDELAYROUTINGLAYER_H_

#define TRUE                            1
#define FALSE                           0

#include <omnetpp.h>
#include <cstdlib>
#include <sstream>
#include <string>
#include <map>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

using namespace omnetpp;

using namespace std;

class KBaseNodeInfo;

class KOptimumDelayRoutingLayer : public cSimpleModule
{
    public:
        struct CacheEntry;
        string ownMACAddress;
        map<string, CacheEntry*> cacheList;

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

    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();

    private:
        int nextAppID;
        int usedRNG;
        double cacheSizeReportingFrequency;

        int numEventsHandled;

        int currentCacheSize;

        cMessage *cacheSizeReportingTimeoutEvent;

        list<KOptimumDelayRoutingLayer*> allOptimumDelayRoutingLayerModuleList;

        vector<string> lastNeighbourList;
        bool cacheUpdatedAfterLastCheck;

        struct AppInfo {
            int appID;
            string appName;
            string prefixName;
        };

        list<AppInfo*> registeredAppList;

        void handleAppRegistrationMsg(cMessage *msg);
        void handleDataMsgFromUpperLayer(cMessage *msg);
        void handleNeighbourListMsgFromLowerLayer(cMessage *msg);
        void handleDataMsgFromLowerLayerDirect(cMessage *msg);
        void sendDataMessageDirect(string nodeMACAddress);

        // stats related variables
        simsignal_t dataBytesReceivedSignal;
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
        simsignal_t totalBytesSentSignal;

};

#define KOPTIMUMDELAYROUTINGLAYER_SIMMODULEINFO         " KOptimumDelayRoutingLayer>!<" << simTime() << ">!<" << getParentModule()->getFullName()

#define KOPTIMUMDELAYROUTINGLAYER_MSG_ID_HASH_SIZE      4 // in bytes
#define KOPTIMUMDELAYROUTINGLAYER_CACHESIZE_REP_EVENT   185

#endif /* KOPTIMUMDELAYROUTINGLAYER_H_ */

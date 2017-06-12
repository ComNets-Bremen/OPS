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

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;

class KEpidemicRoutingLayer: public cSimpleModule
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

        int currentCacheSize;

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

            double validUntilTime;

            int realPacketSize;

            bool destinationOriented;
            string originatorNodeName;
            string finalDestinationNodeName;

            double createdTime;
            double updatedTime;
            double lastAccessedTime;

        };

        struct SyncedNeighbour {
            string nodeMACAddress;
            double lastSyncTime;
        };

        list<AppInfo*> registeredAppList;
        list<CacheEntry*> cacheList;
        list<SyncedNeighbour*> syncedNeighbourList;

        cMessage *ageDataTimeoutEvent;
        cMessage *syncedNeighboursInitTimeoutEvent;

        void handleDataAgingTrigger(cMessage *msg);
        void handleSyncedNeighbourRemovalTrigger(cMessage *msg);
        void handleAppRegistrationMsg(cMessage *msg);
        void handleDataMsgFromUpperLayer(cMessage *msg);
        void handleNeighbourListMsgFromLowerLayer(cMessage *msg);
        void handleDataMsgFromLowerLayer(cMessage *msg);
        void handleSummaryVectorMsgFromLowerLayer(cMessage *msg);
        void handleDataRequestMsgFromLowerLayer(cMessage *msg);
        bool syncDoneWithNeighbour(string nodeMACAddress);
        void updateNeighbourSyncStarted(string nodeMACAddress);

};
#define KEPIDEMICROUTINGLAYER_SIMMODULEINFO         " :: " << simTime() << " :: " << getParentModule()->getFullName() << " :: KEpidemicRoutingLayer "
#define KEPIDEMICROUTINGLAYER_DEBUG                 " :: DEBUG :: " << ownMACAddress

#define KEPIDEMICROUTINGLAYER_MSG_ID_HASH_SIZE      4 // in bytes

#endif /* KEPIDEMICROUTINGLAYER_H_ */
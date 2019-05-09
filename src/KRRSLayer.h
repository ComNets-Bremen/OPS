//
// The model implementation for the Randomized Rumor Spreading (RRS) layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 25-aug-2015
//
//

#ifndef KRRSLAYER_H_
#define KRRSLAYER_H_

#define TRUE                            1
#define FALSE                           0

#include <omnetpp.h>
#include <cstdlib>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;

class KRRSLayer: public cSimpleModule
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
        int logging;

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

            int msgType;
            simtime_t validUntilTime;

            int realPacketSize;

            bool destinationOriented;
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

        string broadcastMACAddress;

        list<AppInfo*> registeredAppList;
        list<CacheEntry*> cacheList;
        cMessage *ageDataTimeoutEvent;

};
#define KRRSLAYER_SIMMODULEINFO       " KRRSLayer>!<" << simTime() << ">!<" << getParentModule()->getFullName()

#endif /* KRRSLAYER_H_ */

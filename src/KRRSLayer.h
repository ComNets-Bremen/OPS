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
            string dataName;
            int goodnessValue;
            int realPayloadSize;
            string dummyPayloadContent;
            int msgType;
            double validUntilTime;

            int realPacketSize;

            string finalDestinationNodeName;

            double createdTime;
            double updatedTime;
            double lastAccessedTime;

        };

        string broadcastMACAddress;

        list<AppInfo*> registeredAppList;
        list<CacheEntry*> cacheList;
        cMessage *ageDataTimeoutEvent;

};
#define KRRSLAYER_SIMMODULEINFO       ">!<" << simTime() << ">!<" << getParentModule()->getFullName()

#endif /* KRRSLAYER_H_ */

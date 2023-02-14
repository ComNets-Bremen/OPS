//
// The model implementation for the BubbleRAP Forwarding .
//
// @author : Akhil Simha Neela (neela@uni-bremen.de)
// @date   : 24-jan-2023
//


#ifndef KBUBBLERAPFORWARDING_H_
#define KBUBBLERAPFORWARDING_H_

#define TRUE                            1
#define FALSE                           0

#include <omnetpp.h>
#include <cstdlib>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

////// BubbleRAP Inclusion //////
#include <iostream>
#include <fstream>
#include <string>


#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;

class KBubbleRAPForwarding: public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();

    private:
        int l_rank, g_rank, comm;
        vector<vector<string>> content;
        string ownMACAddress;
        int nodeIndex;
        int nextAppID;
        int maximumCacheSize;
        bool broadcastRRS;
        int usedRNG;
        bool ageCache;
        double cacheSizeReportingFrequency;
        bool  sendOnNeighReportingFrequency;
        double sendFrequencyWhenNotOnNeighFrequency;

        int currentCacheSize;
        simtime_t nextDataSendTime;

        int myGlobalRank;
        int myLocalRank;
        int myCommunityID;
        int destinationCommunity;

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
        cMessage *cacheSizeReportingTimeoutEvent;

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

        bool isDataMsgAccepted(int msgGlobalRank,int msgLocalRank, int msgCommunityID, string destinationMAC);

};
#define KBUBBLERAPFORWARDING_SIMMODULEINFO         " KBubbleRAPForwarding>!<" << simTime() << ">!<" << getParentModule()->getFullName()
#define KBUBBLERAPFORWARDING_EVENTTYPE_AGEDATA     108
#define KBUBBLERAPFORWARDING_CACHESIZE_REP_EVENT   175

#endif /* KBUBBLERAPFORWARDING_H_ */

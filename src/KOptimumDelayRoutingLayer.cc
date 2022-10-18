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
// Cache Modification - C++ vector
// @author : Hai Thien Long Thai (hthai@uni-bremen.de, thaihaithienlong@yahoo.com)
// @date   : sept-2022

#include "KOptimumDelayRoutingLayer.h"

Define_Module(KOptimumDelayRoutingLayer);


void KOptimumDelayRoutingLayer::initialize(int stage)
{
    if (stage == 0) {
        // get parameters
        ownMACAddress = par("ownMACAddress").stringValue();
        nextAppID = 1;
        currentCacheSize = 0;
        usedRNG = par("usedRNG");
        cacheSizeReportingFrequency = par("cacheSizeReportingFrequency");
        numEventsHandled = 0;
        cacheUpdatedAfterLastCheck = FALSE;


    } else if (stage == 1) {


    } else if (stage == 2) {

        // get module info of all other KOptimumDelayRoutingLayer modules in network
        for (int id = 0; id <= getSimulation()->getLastComponentId(); id++) {
            cModule *unknownModule = getSimulation()->getModule(id);
            if (unknownModule == NULL) {
                continue;
            }

            // if module is yourself, don't consider
            if (unknownModule == this) {
                continue;
            }

            // find the KOptimumDelayRoutingLayer module
            cModule *unknownSubModule = NULL;
            for (cModule::SubmoduleIterator it(unknownModule); !it.end(); ++it) {

                if (dynamic_cast<KOptimumDelayRoutingLayer*>(*it) != NULL) {
                    unknownSubModule = *it;
                }
            }

            if (unknownSubModule != NULL) {
                KOptimumDelayRoutingLayer *optimumDelayRoutingLayerModule = dynamic_cast<KOptimumDelayRoutingLayer*>(unknownSubModule);
                allOptimumDelayRoutingLayerModuleList.push_back(optimumDelayRoutingLayerModule);
            }
        }

        // create and setup cache size reporting trigger
        cacheSizeReportingTimeoutEvent = new cMessage("Cache Size Reporting Event");
        cacheSizeReportingTimeoutEvent->setKind(KOPTIMUMDELAYROUTINGLAYER_CACHESIZE_REP_EVENT);
        scheduleAt(simTime() + cacheSizeReportingFrequency, cacheSizeReportingTimeoutEvent);

        // setup statistics signals
        dataBytesReceivedSignal = registerSignal("fwdDataBytesReceived");
        totalBytesReceivedSignal = registerSignal("fwdTotalBytesReceived");
        hopsTravelledSignal = registerSignal("fwdHopsTravelled");
        hopsTravelledCountSignal = registerSignal("fwdHopsTravelledCount");

        cacheBytesRemovedSignal = registerSignal("fwdCacheBytesRemoved");
        cacheBytesAddedSignal = registerSignal("fwdCacheBytesAdded");
        cacheBytesUpdatedSignal = registerSignal("fwdCacheBytesUpdated");
        currentCacheSizeBytesSignal = registerSignal("fwdCurrentCacheSizeBytes");
        currentCacheSizeReportedCountSignal = registerSignal("fwdCurrentCacheSizeReportedCount");

        currentCacheSizeBytesPeriodicSignal = registerSignal("fwdCurrentCacheSizeBytesPeriodic");

        currentCacheSizeBytesSignal2 = registerSignal("fwdCurrentCacheSizeBytes2");

        dataBytesSentSignal = registerSignal("fwdDataBytesSent");
        totalBytesSentSignal = registerSignal("fwdTotalBytesSent");

    } else {
        EV_FATAL << KOPTIMUMDELAYROUTINGLAYER_SIMMODULEINFO << "Something is radically wrong in initialization \n";
    }
}

int KOptimumDelayRoutingLayer::numInitStages() const
{
    return 3;
}

void KOptimumDelayRoutingLayer::handleMessage(cMessage *msg)
{
    cGate *gate;
    char gateName[64];

    numEventsHandled++;

    // self messages
    if (msg->isSelfMessage()) {
        if (msg->getKind() == KOPTIMUMDELAYROUTINGLAYER_CACHESIZE_REP_EVENT) {

            // report cache size
            emit(currentCacheSizeBytesPeriodicSignal, currentCacheSize);

            // setup next cache size reporting trigger
            scheduleAt(simTime() + cacheSizeReportingFrequency, cacheSizeReportingTimeoutEvent);

        } else {
            EV_INFO << KOPTIMUMDELAYROUTINGLAYER_SIMMODULEINFO << "Received unexpected self message" << "\n";
            delete msg;
        }

    // messages from other layers
    } else {

       // get message arrival gate name
        gate = msg->getArrivalGate();
        strcpy(gateName, gate->getName());

        // app registration message arrived from the upper layer (app layer)
        if (strstr(gateName, "upperLayerIn") != NULL && dynamic_cast<KRegisterAppMsg*>(msg) != NULL) {

            handleAppRegistrationMsg(msg);

        // data message arrived from the upper layer (app layer)
        } else if (strstr(gateName, "upperLayerIn") != NULL && dynamic_cast<KDataMsg*>(msg) != NULL) {

            handleDataMsgFromUpperLayer(msg);

        // neighbor list message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && dynamic_cast<KNeighbourListMsg*>(msg) != NULL) {

            handleNeighbourListMsgFromLowerLayer(msg);

        // data message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerDirectIn") != NULL && dynamic_cast<KDataMsg*>(msg) != NULL) {

            handleDataMsgFromLowerLayerDirect(msg);

        // received some unexpected packet
        } else {

            EV_INFO << KOPTIMUMDELAYROUTINGLAYER_SIMMODULEINFO << "Received unexpected packet" << "\n";
            delete msg;
        }
    }
}

void KOptimumDelayRoutingLayer::handleAppRegistrationMsg(cMessage *msg)
{
    KRegisterAppMsg *regAppMsg = dynamic_cast<KRegisterAppMsg*>(msg);
    AppInfo *appInfo = NULL;
    int found = FALSE;
    list<AppInfo*>::iterator iteratorRegisteredApps = registeredAppList.begin();
    while (iteratorRegisteredApps != registeredAppList.end()) {
        appInfo = *iteratorRegisteredApps;
        if (appInfo->appName == regAppMsg->getAppName()) {
            found = TRUE;
            break;
        }
        iteratorRegisteredApps++;
    }

    if (!found) {
        appInfo = new AppInfo;
        appInfo->appID = nextAppID++;
        appInfo->appName = regAppMsg->getAppName();
        registeredAppList.push_back(appInfo);

    }
    appInfo->prefixName = regAppMsg->getPrefixName();
    delete msg;

}

void KOptimumDelayRoutingLayer::handleDataMsgFromUpperLayer(cMessage *msg)
{
    KDataMsg *omnetDataMsg = dynamic_cast<KDataMsg*>(msg);

    //CacheEntry *cacheEntry;
    vector<CacheEntry>::iterator iteratorCache;
    bool found = FALSE;
    /*
    iteratorCache = cacheList.begin();
    while (iteratorCache != cacheList.end()) {
        cacheEntry = *iteratorCache;
        if (cacheEntry->dataName == omnetDataMsg->getDataName()) {
            found = TRUE;
            break;
        }

        iteratorCache++;
    }
    */
    iteratorCache = lower_bound(cacheList.begin(), cacheList.end(), omnetDataMsg->getDataName(), [](const CacheEntry& a, string b) {return (a.dataName < b);});
    if (iteratorCache != cacheList.end() && (*iteratorCache).dataName == omnetDataMsg->getDataName()) {
        found = TRUE;
        //////std::cout << "5, found";
        //cacheEntry = *iteratorCache;
    }

    if (!found) {

        CacheEntry cacheEntry;

        cacheEntry.messageID = omnetDataMsg->getDataName();
        cacheEntry.hopCount = 0;
        cacheEntry.dataName = omnetDataMsg->getDataName();
        cacheEntry.realPayloadSize = omnetDataMsg->getRealPayloadSize();
        cacheEntry.dummyPayloadContent = omnetDataMsg->getDummyPayloadContent();
        cacheEntry.validUntilTime = omnetDataMsg->getValidUntilTime();
        cacheEntry.realPacketSize = omnetDataMsg->getRealPacketSize();
        cacheEntry.initialOriginatorAddress = omnetDataMsg->getInitialOriginatorAddress();
        cacheEntry.destinationOriented = omnetDataMsg->getDestinationOriented();
        if (omnetDataMsg->getDestinationOriented()) {
            cacheEntry.finalDestinationAddress = omnetDataMsg->getFinalDestinationAddress();
        }
        cacheEntry.goodnessValue = omnetDataMsg->getGoodnessValue();
        cacheEntry.hopsTravelled = 0;

        cacheEntry.msgUniqueID = omnetDataMsg->getMsgUniqueID();
        cacheEntry.initialInjectionTime = omnetDataMsg->getInitialInjectionTime();

        cacheEntry.createdTime = simTime().dbl();
        cacheEntry.updatedTime = simTime().dbl();

        //cacheList.push_back(cacheEntry);
        //sort(cacheList.begin(), cacheList.end(), [](const CacheEntry& a, const CacheEntry& b) {return (a.messageID < b.messageID);});
        iteratorCache = lower_bound(cacheList.begin(), cacheList.end(), cacheEntry.messageID, [](const CacheEntry& a, string b) {return (a.dataName < b);});
        iteratorCache = cacheList.insert(iteratorCache, cacheEntry);

        currentCacheSize += cacheEntry.realPayloadSize;

    }

    (*iteratorCache).lastAccessedTime = simTime().dbl();

    // log cache update or add
    if (found) {
        emit(cacheBytesUpdatedSignal, (*iteratorCache).realPayloadSize);
    } else {
        emit(cacheBytesAddedSignal, (*iteratorCache).realPayloadSize);
    }
    emit(currentCacheSizeBytesSignal, currentCacheSize);
    emit(currentCacheSizeReportedCountSignal, (int) 1);

    emit(currentCacheSizeBytesSignal2, currentCacheSize);

    cacheUpdatedAfterLastCheck = TRUE;

    delete msg;
}

void KOptimumDelayRoutingLayer::handleNeighbourListMsgFromLowerLayer(cMessage *msg)
{
    KNeighbourListMsg *neighListMsg = dynamic_cast<KNeighbourListMsg*>(msg);

    // if no neighbours or cache is empty, just return
    if (neighListMsg->getNeighbourNameListArraySize() == 0 || cacheList.size() == 0) {

        delete msg;
        return;
    }

    // send  messages (if appropriate) to all nodes to sync in a loop
    int i = 0;
    while (i < neighListMsg->getNeighbourNameListArraySize()) {
        string nodeMACAddress = neighListMsg->getNeighbourNameList(i);

        // check if neighbour was there before
        bool found = FALSE;
        int j = 0;
        while (j < lastNeighbourList.size()) {
            if (nodeMACAddress == lastNeighbourList[j]) {
                found = TRUE;
                break;
            }

            j++;
        }

        // if neighbour was not there before or if cache updated in between,
        // then check and send data messages
        if (!found || cacheUpdatedAfterLastCheck) {
            sendDataMessageDirect(nodeMACAddress);
        }

        i++;
    }

    // clear the last neighbour list
    lastNeighbourList.clear();

    // save last neighbour list
    i = 0;
    while (i < neighListMsg->getNeighbourNameListArraySize()) {
        string nodeMACAddress = neighListMsg->getNeighbourNameList(i);
        lastNeighbourList.push_back(nodeMACAddress);

        i++;
    }

    // reset cache updates flag
    cacheUpdatedAfterLastCheck = FALSE;

    delete msg;
}

void KOptimumDelayRoutingLayer::handleDataMsgFromLowerLayerDirect(cMessage *msg)
{
    KDataMsg *omnetDataMsg = dynamic_cast<KDataMsg*>(msg);
    bool found;

    // increment the traveled hop count
    omnetDataMsg->setHopsTravelled(omnetDataMsg->getHopsTravelled() + 1);
    omnetDataMsg->setHopCount(omnetDataMsg->getHopCount() + 1);

    emit(dataBytesReceivedSignal, (long) omnetDataMsg->getByteLength());
    emit(totalBytesReceivedSignal, (long) omnetDataMsg->getByteLength());
    emit(hopsTravelledSignal, (long) omnetDataMsg->getHopsTravelled());
    emit(hopsTravelledCountSignal, 1);

    // insert/update cache
    ///CacheEntry *cacheEntry;
    vector<CacheEntry>::iterator iteratorCache;
    found = FALSE;
    /*
    iteratorCache = cacheList.begin();
    while (iteratorCache != cacheList.end()) {
        cacheEntry = *iteratorCache;
        if (cacheEntry->dataName == omnetDataMsg->getDataName()) {
            found = TRUE;
            break;
        }

        iteratorCache++;
    }
    */
    iteratorCache = lower_bound(cacheList.begin(), cacheList.end(), omnetDataMsg->getDataName(), [](const CacheEntry& a, string b) {return (a.dataName < b);});
    if (iteratorCache != cacheList.end() && (*iteratorCache).dataName == omnetDataMsg->getDataName()) {
        found = TRUE;
        //////std::cout << "5, found";
        //cacheEntry = *iteratorCache;
    }

    if (!found) {
        CacheEntry cacheEntry;

        cacheEntry.messageID = omnetDataMsg->getMessageID();
        cacheEntry.dataName = omnetDataMsg->getDataName();
        cacheEntry.realPayloadSize = omnetDataMsg->getRealPayloadSize();
        cacheEntry.dummyPayloadContent = omnetDataMsg->getDummyPayloadContent();
        cacheEntry.validUntilTime = omnetDataMsg->getValidUntilTime();
        cacheEntry.realPacketSize = omnetDataMsg->getRealPacketSize();
        cacheEntry.initialOriginatorAddress = omnetDataMsg->getInitialOriginatorAddress();
        cacheEntry.destinationOriented = omnetDataMsg->getDestinationOriented();
        if (omnetDataMsg->getDestinationOriented()) {
            cacheEntry.finalDestinationAddress = omnetDataMsg->getFinalDestinationAddress();
        }
        cacheEntry.goodnessValue = omnetDataMsg->getGoodnessValue();

        cacheEntry.msgUniqueID = omnetDataMsg->getMsgUniqueID();
        cacheEntry.initialInjectionTime = omnetDataMsg->getInitialInjectionTime();

        cacheEntry.createdTime = simTime().dbl();
        cacheEntry.updatedTime = simTime().dbl();

        //cacheList.push_back(cacheEntry);
        //sort(cacheList.begin(), cacheList.end(), [](const CacheEntry& a, const CacheEntry& b) {return (a.messageID < b.messageID);});
        iteratorCache = lower_bound(cacheList.begin(), cacheList.end(), cacheEntry.messageID, [](const CacheEntry& a, string b) {return (a.dataName < b);});
        iteratorCache = cacheList.insert(iteratorCache, cacheEntry);

        currentCacheSize += cacheEntry.realPayloadSize;

    }

    (*iteratorCache).hopsTravelled = omnetDataMsg->getHopsTravelled();
    (*iteratorCache).hopCount = omnetDataMsg->getHopCount();
    (*iteratorCache).lastAccessedTime = simTime().dbl();

    // log cache update or add
    if (found) {
        emit(cacheBytesUpdatedSignal, (*iteratorCache).realPayloadSize);
    } else {
        emit(cacheBytesAddedSignal, (*iteratorCache).realPayloadSize);
    }
    emit(currentCacheSizeBytesSignal, currentCacheSize);
    emit(currentCacheSizeReportedCountSignal, (int) 1);

    emit(currentCacheSizeBytesSignal2, currentCacheSize);

    cacheUpdatedAfterLastCheck = TRUE;

    // if registered app exist, send data msg to app
    AppInfo *appInfo = NULL;
    found = FALSE;
    list<AppInfo*>::iterator iteratorRegisteredApps = registeredAppList.begin();
    while (iteratorRegisteredApps != registeredAppList.end()) {
        appInfo = *iteratorRegisteredApps;
        if (strstr(omnetDataMsg->getDataName(), appInfo->prefixName.c_str()) != NULL
                && ((omnetDataMsg->getDestinationOriented()
                      && strstr(omnetDataMsg->getFinalDestinationAddress(), ownMACAddress.c_str()) != NULL)
                    || (!omnetDataMsg->getDestinationOriented()))) {
            found = TRUE;
            break;
        }
        iteratorRegisteredApps++;
    }
    if (found) {
        send(msg, "upperLayerOut");

    } else {
        delete msg;
    }

}

void KOptimumDelayRoutingLayer::sendDataMessageDirect(string nodeMACAddress)
{

    // find the KOptimumDelayRoutingLayer module of given address
    KOptimumDelayRoutingLayer *optimumDelayRoutingLayerModule = NULL;
    bool found = FALSE;
    list<KOptimumDelayRoutingLayer*>::iterator iteratorModuleInfo = allOptimumDelayRoutingLayerModuleList.begin();
    while (iteratorModuleInfo != allOptimumDelayRoutingLayerModuleList.end()) {
        optimumDelayRoutingLayerModule = *iteratorModuleInfo;
        if (optimumDelayRoutingLayerModule->ownMACAddress == nodeMACAddress) {
            found = TRUE;
            break;
        }

        iteratorModuleInfo++;
    }

    // check own cache entries directly with the other node's cache
    // entries and send only ones not available
    if (found) {

        vector<CacheEntry>::iterator iteratorOwnCache = cacheList.begin();
        while (iteratorOwnCache != cacheList.end()) {
            //CacheEntry ownCacheEntry = *iteratorOwnCache;
            found = FALSE;
            //CacheEntry *othersCacheEntry;
            vector<CacheEntry>::iterator iteratorOtherCache;
            iteratorOtherCache = lower_bound(optimumDelayRoutingLayerModule->cacheList.begin(), optimumDelayRoutingLayerModule->cacheList.end(), (*iteratorOwnCache).dataName, [](const CacheEntry& a, string b) {return (a.dataName < b);});
            if ((iteratorOtherCache != optimumDelayRoutingLayerModule->cacheList.end()) && (*iteratorOwnCache).dataName == (*iteratorOtherCache).dataName ){
                found = TRUE;
            }
            /*
            while (iteratorOtherCache != optimumDelayRoutingLayerModule->cacheList.end()) {
                othersCacheEntry = (*iteratorOtherCache).second;

                if (ownCacheEntry->dataName == othersCacheEntry->dataName) {
                    found = TRUE;
                    break;
                }
                iteratorOtherCache++;
            }*/


            if (!found) {

                // create data packet
                KDataMsg *dataMsg = new KDataMsg();
                dataMsg->setSourceAddress(ownMACAddress.c_str());
                dataMsg->setDestinationAddress(nodeMACAddress.c_str());
                dataMsg->setDataName((*iteratorOwnCache).dataName.c_str());
                dataMsg->setDummyPayloadContent((*iteratorOwnCache).dummyPayloadContent.c_str());
                dataMsg->setValidUntilTime((*iteratorOwnCache).validUntilTime);
                dataMsg->setRealPayloadSize((*iteratorOwnCache).realPayloadSize);
                // check KOPSMsg.msg on sizing mssages
                int realPacketSize = 6 + 6 + 2 + (*iteratorOwnCache).realPayloadSize + 4 + 6 + 1;
                dataMsg->setRealPacketSize(realPacketSize);
                dataMsg->setByteLength(realPacketSize);
                dataMsg->setInitialOriginatorAddress((*iteratorOwnCache).initialOriginatorAddress.c_str());
                dataMsg->setDestinationOriented((*iteratorOwnCache).destinationOriented);
                if ((*iteratorOwnCache).destinationOriented) {
                    dataMsg->setFinalDestinationAddress((*iteratorOwnCache).finalDestinationAddress.c_str());
                }
                dataMsg->setMessageID((*iteratorOwnCache).messageID.c_str());
                dataMsg->setHopCount((*iteratorOwnCache).hopCount);
                dataMsg->setGoodnessValue((*iteratorOwnCache).goodnessValue);
                dataMsg->setHopsTravelled((*iteratorOwnCache).hopsTravelled);
                dataMsg->setMsgUniqueID((*iteratorOwnCache).msgUniqueID);
                dataMsg->setInitialInjectionTime((*iteratorOwnCache).initialInjectionTime);

                // send message directly
                sendDirect(dataMsg, optimumDelayRoutingLayerModule, "lowerLayerDirectIn");

                emit(dataBytesSentSignal, (long) dataMsg->getByteLength());
                emit(totalBytesSentSignal, (long) dataMsg->getByteLength());

            }

            iteratorOwnCache++;
        }
    }
}

void KOptimumDelayRoutingLayer::finish()
{

    recordScalar("numEventsHandled", numEventsHandled);


    // clear registered app list
    while (registeredAppList.size() > 0) {
        list<AppInfo*>::iterator iteratorRegisteredApp = registeredAppList.begin();
        AppInfo *appInfo= *iteratorRegisteredApp;
        registeredAppList.remove(appInfo);
        delete appInfo;
    }

    // clear  cache list
    cacheList.clear();

    // remove triggers
    cancelEvent(cacheSizeReportingTimeoutEvent);
    delete cacheSizeReportingTimeoutEvent;

    // clear last neighbour list
    lastNeighbourList.clear();

}

//
// The model implementation for the Epidemic Routing layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 02-may-2017
//
//
// Cache Modification - C++ vector
// @author : Hai Thien Long Thai (hthai@uni-bremen.de, thaihaithienlong@yahoo.com)
// @date   : jan-2022
//
//
// Fixing delivery of destination-oriented data (fix 1, 2)
// @author : Hai Thien Long Thai (hthai@uni-bremen.de, thaihaithienlong@yahoo.com)
// @date   : june-2022
//

#include "KEpidemicRoutingLayer.h"

Define_Module(KEpidemicRoutingLayer);

void KEpidemicRoutingLayer::initialize(int stage)
{
    if (stage == 0) {
        // get parameters
        ownMACAddress = par("ownMACAddress").stringValue();
        nextAppID = 1;
        maximumCacheSize = par("maximumCacheSize");
        currentCacheSize = 0;
        antiEntropyInterval = par("antiEntropyInterval");
        maximumHopCount = par("maximumHopCount");
        maximumRandomBackoffDuration = par("maximumRandomBackoffDuration");
        useTTL = par("useTTL");
        usedRNG = par("usedRNG");
        cacheSizeReportingFrequency = par("cacheSizeReportingFrequency");
        numEventsHandled = 0;

        syncedNeighbourListIHasChanged = TRUE;

    } else if (stage == 1) {


    } else if (stage == 2) {

        // create and setup cache size reporting trigger
        cacheSizeReportingTimeoutEvent = new cMessage("Cache Size Reporting Event");
        cacheSizeReportingTimeoutEvent->setKind(KEPIDEMICROUTINGLAYER_CACHESIZE_REP_EVENT);
        scheduleAt(simTime() + cacheSizeReportingFrequency, cacheSizeReportingTimeoutEvent);

        // setup statistics signals
        dataBytesReceivedSignal = registerSignal("fwdDataBytesReceived");
        sumVecBytesReceivedSignal = registerSignal("fwdSumVecBytesReceived");
        dataReqBytesReceivedSignal = registerSignal("fwdDataReqBytesReceived");
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
        sumVecBytesSentSignal = registerSignal("fwdSumVecBytesSent");
        dataReqBytesSentSignal = registerSignal("fwdDataReqBytesSent");
        totalBytesSentSignal = registerSignal("fwdTotalBytesSent");

    } else {
        EV_FATAL << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << "Something is radically wrong in initialization \n";
    }
}

int KEpidemicRoutingLayer::numInitStages() const
{
    return 3;
}

void KEpidemicRoutingLayer::handleMessage(cMessage *msg)
{
    cGate *gate;
    char gateName[64];

    numEventsHandled++;

    // age the data in the cache only if needed (e.g. a message arrived)
    if (useTTL)
        ageDataInCache();

    // self messages
    if (msg->isSelfMessage()) {
        if (msg->getKind() == KEPIDEMICROUTINGLAYER_CACHESIZE_REP_EVENT) {

            // report cache size
            emit(currentCacheSizeBytesPeriodicSignal, currentCacheSize);

            // setup next cache size reporting trigger
            scheduleAt(simTime() + cacheSizeReportingFrequency, cacheSizeReportingTimeoutEvent);

        } else {
            EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << "Received unexpected self message" << "\n";
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

        // neighbour list message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && dynamic_cast<KNeighbourListMsg*>(msg) != NULL) {

            handleNeighbourListMsgFromLowerLayer(msg);

        // data message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && dynamic_cast<KDataMsg*>(msg) != NULL) {

            handleDataMsgFromLowerLayer(msg);

        // summary vector message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && dynamic_cast<KSummaryVectorMsg*>(msg) != NULL) {

            handleSummaryVectorMsgFromLowerLayer(msg);

        // data request message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && dynamic_cast<KDataRequestMsg*>(msg) != NULL) {

            handleDataRequestMsgFromLowerLayer(msg);

        // received some unexpected packet
        } else {

            EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << "Received unexpected packet" << "\n";
            delete msg;
        }
    }
}

void KEpidemicRoutingLayer::ageDataInCache()
{

    // remove expired data items
    int expiredFound = TRUE;
    while (expiredFound) {
        expiredFound = FALSE;

        // MODIFIED
        //CacheEntry cacheEntry;
        vector<CacheEntry>::iterator iteratorCache;
        iteratorCache = cacheList.begin();
        while (iteratorCache != cacheList.end()) {
            //cacheEntry = *iteratorCache;
            if ((*iteratorCache).validUntilTime < simTime().dbl()) {
                expiredFound = TRUE;
                break;
            }
            iteratorCache++;
        }
        if (expiredFound) {
            currentCacheSize -= (*iteratorCache).realPacketSize;

            emit(cacheBytesRemovedSignal, (*iteratorCache).realPayloadSize);
            emit(currentCacheSizeBytesSignal, currentCacheSize);
            emit(currentCacheSizeReportedCountSignal, (int) 1);

            emit(currentCacheSizeBytesSignal2, currentCacheSize);

            //int range = iteratorCache - cacheList.begin();
            //swap(*iteratorCache, cacheList.back());
            //cacheList.pop_back();
            //sort(cacheList.begin() + range, cacheList.end(), [](const CacheEntry& a, const CacheEntry& b) {return (a.messageID < b.messageID);});
            cacheList.erase(iteratorCache);

            //delete cacheEntry;

        }
        ///////////////////////////////////////
    }

}


void KEpidemicRoutingLayer::handleAppRegistrationMsg(cMessage *msg)
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

void KEpidemicRoutingLayer::handleDataMsgFromUpperLayer(cMessage *msg)
{
    KDataMsg *omnetDataMsg = dynamic_cast<KDataMsg*>(msg);

    //CacheEntry cacheEntry;
    vector<CacheEntry>::iterator iteratorCache;
    int found = FALSE;
    /*
    iteratorCache = cacheList.begin();
    while (iteratorCache != cacheList.end()) {
        cacheEntry = *iteratorCache;
        if (cacheEntry.dataName == omnetDataMsg->getDataName()) {
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

        // MODIFIED
        // apply caching policy if limited cache and cache is full

        if (maximumCacheSize != 0
                && (currentCacheSize + omnetDataMsg->getRealPayloadSize()) > maximumCacheSize
                && cacheList.size() > 0) {
            iteratorCache = cacheList.begin();
            //CacheEntry removingCacheEntry;
            //removingCacheEntry = *iteratorCache;
            vector<CacheEntry>::iterator iteratorRemovingCacheEntry;
            iteratorRemovingCacheEntry = iteratorCache;
            while (iteratorCache != cacheList.end()) {
                //cacheEntry = *iteratorCache;
                if ((*iteratorCache).validUntilTime < (*iteratorRemovingCacheEntry).validUntilTime) {
                    //removingCacheEntry = cacheEntry;
                    iteratorRemovingCacheEntry = iteratorCache;
                }
                iteratorCache++;
            }
            currentCacheSize -= (*iteratorRemovingCacheEntry).realPayloadSize;

            emit(cacheBytesRemovedSignal, (*iteratorRemovingCacheEntry).realPayloadSize);
            emit(currentCacheSizeBytesSignal, currentCacheSize);
            emit(currentCacheSizeReportedCountSignal, (int) 1);

            emit(currentCacheSizeBytesSignal2, currentCacheSize);

            //int range = iteratorRemovingCacheEntry - cacheList.begin();
            //swap(*iteratorRemovingCacheEntry, cacheList.back());
            //cacheList.pop_back();
            //sort(cacheList.begin() + range, cacheList.end(), [](const CacheEntry& a, const CacheEntry& b) {return (a.messageID < b.messageID);});
            cacheList.erase(iteratorRemovingCacheEntry);

            //delete removingCacheEntry;

        }
        ///////////////////////////////////


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

    delete msg;
}

void KEpidemicRoutingLayer::handleNeighbourListMsgFromLowerLayer(cMessage *msg)
{
    KNeighbourListMsg *neighListMsg = dynamic_cast<KNeighbourListMsg*>(msg);

    // if no neighbours or cache is empty, just return
    if (neighListMsg->getNeighbourNameListArraySize() == 0 || cacheList.size() == 0) {

        // setup sync neighbour list for the next time - only if there were some changes
        if (syncedNeighbourListIHasChanged) {
            setSyncingNeighbourInfoForNoNeighboursOrEmptyCache();
            syncedNeighbourListIHasChanged = FALSE;
        }

        delete msg;
        return;
    }

    // send summary vector messages (if appropriate) to all nodes to sync in a loop
    int i = 0;
    while (i < neighListMsg->getNeighbourNameListArraySize()) {
        string nodeMACAddress = neighListMsg->getNeighbourNameList(i);

        // get syncing info of neighbor
        SyncedNeighbour *syncedNeighbour = getSyncingNeighbourInfo(nodeMACAddress);

        // indicate that this node was considered this time
        syncedNeighbour->nodeConsidered = TRUE;

        bool syncWithNeighbour = FALSE;

        if (syncedNeighbour->syncCoolOffEndTime >= simTime().dbl()) {
            // if the sync was done recently, don't sync again until the anti-entropy interval
            // has elapsed
            syncWithNeighbour = FALSE;

        } else if (syncedNeighbour->randomBackoffStarted && syncedNeighbour->randomBackoffEndTime >= simTime().dbl()) {
            // if random backoff to sync is still active, then wait until time expires
            syncWithNeighbour = FALSE;

        } else if (syncedNeighbour->neighbourSyncing && syncedNeighbour->neighbourSyncEndTime >= simTime().dbl()) {
            // if this neighbour has started syncing with me, then wait until this neighbour finishes
            syncWithNeighbour = FALSE;

        } else if (syncedNeighbour->randomBackoffStarted && syncedNeighbour->randomBackoffEndTime < simTime().dbl()) {
            // has the random backoff just finished - if so, then my turn to start the syncing process
            syncWithNeighbour = TRUE;

        } else if (syncedNeighbour->neighbourSyncing && syncedNeighbour->neighbourSyncEndTime < simTime().dbl()) {
            // has the neighbours syncing period elapsed - if so, my turn to sync
            syncWithNeighbour = TRUE;

        } else {
            // neighbour seen for the first time (could also be after the cool off period)
            // then start the random backoff
            syncedNeighbour->randomBackoffStarted = TRUE;
            double randomBackoffDuration = uniform(1.0, maximumRandomBackoffDuration, usedRNG);
            syncedNeighbour->randomBackoffEndTime = simTime().dbl() + randomBackoffDuration;

            syncWithNeighbour = FALSE;

        }

        // from previous questions - if syncing required
        if (syncWithNeighbour) {

            // set the cooloff period
            syncedNeighbour->syncCoolOffEndTime = simTime().dbl() + antiEntropyInterval;

            // initialize all other checks
            syncedNeighbour->randomBackoffStarted = FALSE;
            syncedNeighbour->randomBackoffEndTime = 0.0;
            syncedNeighbour->neighbourSyncing = FALSE;
            syncedNeighbour->neighbourSyncEndTime = 0.0;

            // send summary vector (to start syncing)
            KSummaryVectorMsg *summaryVectorMsg = makeSummaryVectorMessage();
            summaryVectorMsg->setDestinationAddress(nodeMACAddress.c_str());
            send(summaryVectorMsg, "lowerLayerOut");

            emit(sumVecBytesSentSignal, (long) summaryVectorMsg->getByteLength());
            emit(totalBytesSentSignal, (long) summaryVectorMsg->getByteLength());
        }

        i++;
    }

    // setup sync neighbour list for the next time
    setSyncingNeighbourInfoForNextRound();

    // synched neighbour list must be updated in next round
    // as there were changes
    syncedNeighbourListIHasChanged = TRUE;

    // delete the received neighbor list msg
    delete msg;
}

void KEpidemicRoutingLayer::handleDataMsgFromLowerLayer(cMessage *msg)
{
    KDataMsg *omnetDataMsg = dynamic_cast<KDataMsg*>(msg);
    bool found;

    // increment the travelled hop count
    omnetDataMsg->setHopsTravelled(omnetDataMsg->getHopsTravelled() + 1);
    omnetDataMsg->setHopCount(omnetDataMsg->getHopCount() + 1);

    emit(dataBytesReceivedSignal, (long) omnetDataMsg->getByteLength());
    emit(totalBytesReceivedSignal, (long) omnetDataMsg->getByteLength());
    emit(hopsTravelledSignal, (long) omnetDataMsg->getHopsTravelled());
    emit(hopsTravelledCountSignal, 1);

    // if destination oriented data sent around and this node is the destination
    // or if maximum hop count is reached
    // then cache or else don't cache
    bool cacheData = TRUE;
	
    ///Fix 1: if this node is the destination, no caching, data passed directly to app layer
    if ((omnetDataMsg->getDestinationOriented() && strstr(ownMACAddress.c_str(), omnetDataMsg->getFinalDestinationAddress()) != NULL) || omnetDataMsg->getHopCount() >= maximumHopCount) {
    //if (omnetDataMsg->getHopCount() >= maximumHopCount) {

        cacheData = FALSE;
    }

    if(cacheData) {

        // insert/update cache
        //CacheEntry cacheEntry;
        vector<CacheEntry>::iterator iteratorCache;
        found = FALSE;
        /*
        iteratorCache = cacheList.begin();
        while (iteratorCache != cacheList.end()) {
            cacheEntry = *iteratorCache;
            if (cacheEntry.dataName == omnetDataMsg->getDataName()) {
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

            // MODIFIED
            // apply caching policy if limited cache and cache is full

            if (maximumCacheSize != 0
                && (currentCacheSize + omnetDataMsg->getRealPayloadSize()) > maximumCacheSize
                && cacheList.size() > 0) {
                iteratorCache = cacheList.begin();
                //CacheEntry removingCacheEntry;
                //removingCacheEntry = *iteratorCache;
                vector<CacheEntry>::iterator iteratorRemovingCacheEntry;
                iteratorRemovingCacheEntry = iteratorCache;
                while (iteratorCache != cacheList.end()) {
                    //cacheEntry = *iteratorCache;
                    if ((*iteratorCache).validUntilTime < (*iteratorRemovingCacheEntry).validUntilTime) {
                        //removingCacheEntry = cacheEntry;
                        iteratorRemovingCacheEntry = iteratorCache;
                    }
                    iteratorCache++;
                }
                currentCacheSize -= (*iteratorRemovingCacheEntry).realPayloadSize;

                emit(cacheBytesRemovedSignal, (*iteratorRemovingCacheEntry).realPayloadSize);
                emit(currentCacheSizeBytesSignal, currentCacheSize);
                emit(currentCacheSizeReportedCountSignal, (int) 1);

                emit(currentCacheSizeBytesSignal2, currentCacheSize);

                //int range = iteratorRemovingCacheEntry - cacheList.begin();
                //swap(*iteratorRemovingCacheEntry, cacheList.back());
                //cacheList.pop_back();
                //sort(cacheList.begin() + range, cacheList.end(), [](const CacheEntry& a, const CacheEntry& b) {return (a.messageID < b.messageID);});
                cacheList.erase(iteratorRemovingCacheEntry);

                //delete removingCacheEntry;
            }
            ////////////////////////////////


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
    }

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

void KEpidemicRoutingLayer::handleSummaryVectorMsgFromLowerLayer(cMessage *msg)
{
    KSummaryVectorMsg *summaryVectorMsg = dynamic_cast<KSummaryVectorMsg*>(msg);

    emit(sumVecBytesReceivedSignal, (long) summaryVectorMsg->getByteLength());
    emit(totalBytesReceivedSignal, (long) summaryVectorMsg->getByteLength());

    // when a summary vector is received, it means that the neighbour started the syncing
    // so send the data request message with the required data items


    // check and build a list of missing data items
    string messageID;
    vector<string> selectedMessageIDList;
    int i = 0;
    while (i < summaryVectorMsg->getMessageIDHashVectorArraySize()) {
        messageID = summaryVectorMsg->getMessageIDHashVector(i);

        // see if data item exist in cache
        //CacheEntry cacheEntry;
        vector<CacheEntry>::iterator iteratorCache;
        bool found = FALSE;
        /*
        iteratorCache = cacheList.begin();
        while (iteratorCache != cacheList.end()) {
            cacheEntry = *iteratorCache;
            if (cacheEntry.messageID == messageID) {
                found = TRUE;
                break;
            }

            iteratorCache++;
        }
        */
        iteratorCache = lower_bound(cacheList.begin(), cacheList.end(), messageID, [](const CacheEntry& a, string b) {return (a.messageID < b);});
        if (iteratorCache != cacheList.end() && (*iteratorCache).messageID == messageID) {
            found = TRUE;
            //////std::cout << "5, found";
            //cacheEntry = *iteratorCache;
        }

        if (!found) {
            selectedMessageIDList.push_back(messageID);
        }
        i++;
    }

    // build a KDataRequestMsg with missing data items (i.e.,  message IDs)
    KDataRequestMsg *dataRequestMsg = new KDataRequestMsg();
    dataRequestMsg->setSourceAddress(ownMACAddress.c_str());
    dataRequestMsg->setDestinationAddress(summaryVectorMsg->getSourceAddress());
    int realPacketSize = 6 + 6 + (selectedMessageIDList.size() * KEPIDEMICROUTINGLAYER_MSG_ID_HASH_SIZE);
    dataRequestMsg->setRealPacketSize(realPacketSize);
    dataRequestMsg->setByteLength(realPacketSize);
    dataRequestMsg->setMessageIDHashVectorArraySize(selectedMessageIDList.size());
    i = 0;
    vector<string>::iterator iteratorMessageIDList;
    iteratorMessageIDList = selectedMessageIDList.begin();
    while (iteratorMessageIDList != selectedMessageIDList.end()) {
        messageID = *iteratorMessageIDList;

        dataRequestMsg->setMessageIDHashVector(i, messageID.c_str());

        i++;
        iteratorMessageIDList++;
    }

    send(dataRequestMsg, "lowerLayerOut");

    emit(dataReqBytesSentSignal, (long) dataRequestMsg->getByteLength());
    emit(totalBytesSentSignal, (long) dataRequestMsg->getByteLength());


    // cancel the random backoff timer (because neighbour started syncing)
    string nodeMACAddress = summaryVectorMsg->getSourceAddress();
    SyncedNeighbour *syncedNeighbour = getSyncingNeighbourInfo(nodeMACAddress);
    syncedNeighbour->randomBackoffStarted = FALSE;
    syncedNeighbour->randomBackoffEndTime = 0.0;

    // second - start wait timer until neighbour has finished syncing
    syncedNeighbour->neighbourSyncing = TRUE;
    double delayPerDataMessage = 0.5; // assume 500 milli seconds per data message
    syncedNeighbour->neighbourSyncEndTime = simTime().dbl() + (selectedMessageIDList.size() * delayPerDataMessage);

    // synched neighbour list must be updated in next round
    // as there were changes
    syncedNeighbourListIHasChanged = TRUE;


    delete msg;
}

void KEpidemicRoutingLayer::handleDataRequestMsgFromLowerLayer(cMessage *msg)
{
    KDataRequestMsg *dataRequestMsg = dynamic_cast<KDataRequestMsg*>(msg);

    emit(dataReqBytesReceivedSignal, (long) dataRequestMsg->getByteLength());
    emit(totalBytesReceivedSignal, (long) dataRequestMsg->getByteLength());

    int i = 0;
    while (i < dataRequestMsg->getMessageIDHashVectorArraySize()) {
        string messageID = dataRequestMsg->getMessageIDHashVector(i);

        //CacheEntry cacheEntry;
        vector<CacheEntry>::iterator iteratorCache;
        bool found = FALSE;
        /*
        iteratorCache = cacheList.begin();
        while (iteratorCache != cacheList.end()) {
            cacheEntry = *iteratorCache;
            if (cacheEntry.messageID == messageID) {
                found = TRUE;
                break;
            }

            iteratorCache++;
        }
        */
        iteratorCache = lower_bound(cacheList.begin(), cacheList.end(), messageID, [](const CacheEntry& a, string b) {return (a.messageID < b);});
        if (iteratorCache != cacheList.end() && (*iteratorCache).messageID == messageID) {
            found = TRUE;
            ////std::cout << "5, found"
            //cacheEntry = *iteratorCache;
        }

        if (found) {

            KDataMsg *dataMsg = new KDataMsg();

            dataMsg->setSourceAddress(ownMACAddress.c_str());
            dataMsg->setDestinationAddress(dataRequestMsg->getSourceAddress());
            dataMsg->setDataName((*iteratorCache).dataName.c_str());
            dataMsg->setDummyPayloadContent((*iteratorCache).dummyPayloadContent.c_str());
            dataMsg->setValidUntilTime((*iteratorCache).validUntilTime);
            dataMsg->setRealPayloadSize((*iteratorCache).realPayloadSize);
            // check KOPSMsg.msg on sizing mssages
            int realPacketSize = 6 + 6 + 2 + (*iteratorCache).realPayloadSize + 4 + 6 + 1;
            dataMsg->setRealPacketSize(realPacketSize);
            dataMsg->setByteLength(realPacketSize);
            dataMsg->setInitialOriginatorAddress((*iteratorCache).initialOriginatorAddress.c_str());
            dataMsg->setDestinationOriented((*iteratorCache).destinationOriented);
            if ((*iteratorCache).destinationOriented) {
                dataMsg->setFinalDestinationAddress((*iteratorCache).finalDestinationAddress.c_str());
            }
            dataMsg->setMessageID((*iteratorCache).messageID.c_str());
            dataMsg->setHopCount((*iteratorCache).hopCount);
            dataMsg->setGoodnessValue((*iteratorCache).goodnessValue);
            dataMsg->setHopsTravelled((*iteratorCache).hopsTravelled);
            dataMsg->setMsgUniqueID((*iteratorCache).msgUniqueID);
            dataMsg->setInitialInjectionTime((*iteratorCache).initialInjectionTime);

            send(dataMsg, "lowerLayerOut");

            emit(dataBytesSentSignal, (long) dataMsg->getByteLength());
            emit(totalBytesSentSignal, (long) dataMsg->getByteLength());
			
			///Fix 2: remove cache entry after sending to destination
            if (strstr((*iteratorCache).finalDestinationAddress.c_str(), dataRequestMsg->getSourceAddress()) != NULL
                    && (*iteratorCache).destinationOriented) {

                currentCacheSize -= (*iteratorCache).realPacketSize;

                emit(cacheBytesRemovedSignal, (*iteratorCache).realPayloadSize);
                emit(currentCacheSizeBytesSignal, currentCacheSize);
                emit(currentCacheSizeReportedCountSignal, (int) 1);

                emit(currentCacheSizeBytesSignal2, currentCacheSize);

                cacheList.erase(iteratorCache);
                //delete cacheEntry;
            }

        }

        i++;
    }

    delete msg;
}

KEpidemicRoutingLayer::SyncedNeighbour* KEpidemicRoutingLayer::getSyncingNeighbourInfo(string nodeMACAddress)
{
    // check if sync entry is there
    SyncedNeighbour *syncedNeighbour = NULL;
    list<SyncedNeighbour*>::iterator iteratorSyncedNeighbour;
    bool found = FALSE;
    iteratorSyncedNeighbour = syncedNeighbourList.begin();
    while (iteratorSyncedNeighbour != syncedNeighbourList.end()) {
        syncedNeighbour = *iteratorSyncedNeighbour;
        if (syncedNeighbour->nodeMACAddress == nodeMACAddress) {
            found = TRUE;
            break;
        }

        iteratorSyncedNeighbour++;
    }

    if (!found) {

        // if sync entry not there, create an entry with initial values
        syncedNeighbour = new SyncedNeighbour;

        syncedNeighbour->nodeMACAddress = nodeMACAddress.c_str();
        syncedNeighbour->syncCoolOffEndTime = 0.0;
        syncedNeighbour->randomBackoffStarted = FALSE;
        syncedNeighbour->randomBackoffEndTime = 0.0;
        syncedNeighbour->neighbourSyncing = FALSE;
        syncedNeighbour->neighbourSyncEndTime = 0.0;
        syncedNeighbour->nodeConsidered = FALSE;

        syncedNeighbourList.push_back(syncedNeighbour);
    }

    return syncedNeighbour;
}

void KEpidemicRoutingLayer::setSyncingNeighbourInfoForNextRound()
{
    // loop thru syncing neighbor list and set for next round
    list<SyncedNeighbour*>::iterator iteratorSyncedNeighbour;
    iteratorSyncedNeighbour = syncedNeighbourList.begin();
    while (iteratorSyncedNeighbour != syncedNeighbourList.end()) {
        SyncedNeighbour *syncedNeighbour = *iteratorSyncedNeighbour;

        if (!syncedNeighbour->nodeConsidered) {

            // if neighbour not considered this time, then it means the
            // neighbour was not in my neighbourhood - so init all flags and timers

            syncedNeighbour->randomBackoffStarted = FALSE;
            syncedNeighbour->randomBackoffEndTime = 0.0;
            syncedNeighbour->neighbourSyncing = FALSE;
            syncedNeighbour->neighbourSyncEndTime = 0.0;
        }

        // setup for next time
        syncedNeighbour->nodeConsidered = FALSE;

        iteratorSyncedNeighbour++;
    }
}

void KEpidemicRoutingLayer::setSyncingNeighbourInfoForNoNeighboursOrEmptyCache()
{
    // loop thru syncing neighbor list and set for next round
    list<SyncedNeighbour*>::iterator iteratorSyncedNeighbour;
    iteratorSyncedNeighbour = syncedNeighbourList.begin();
    while (iteratorSyncedNeighbour != syncedNeighbourList.end()) {
        SyncedNeighbour *syncedNeighbour = *iteratorSyncedNeighbour;

        syncedNeighbour->randomBackoffStarted = FALSE;
        syncedNeighbour->randomBackoffEndTime = 0.0;
        syncedNeighbour->neighbourSyncing = FALSE;
        syncedNeighbour->neighbourSyncEndTime = 0.0;
        syncedNeighbour->nodeConsidered = FALSE;

        iteratorSyncedNeighbour++;
    }
}

KSummaryVectorMsg* KEpidemicRoutingLayer::makeSummaryVectorMessage()
{

    // identify the entries of the summary vector
    vector<string> selectedMessageIDList;
    //CacheEntry cacheEntry;
    vector<CacheEntry>::iterator iteratorCache;
    iteratorCache = cacheList.begin();
    while (iteratorCache != cacheList.end()) {
        //cacheEntry = *iteratorCache;
        if (((*iteratorCache).hopCount + 1) < maximumHopCount) {
            selectedMessageIDList.push_back((*iteratorCache).messageID);
        }

        iteratorCache++;
    }

    // make a summary vector message
    KSummaryVectorMsg *summaryVectorMsg = new KSummaryVectorMsg();
    summaryVectorMsg->setSourceAddress(ownMACAddress.c_str());
    summaryVectorMsg->setMessageIDHashVectorArraySize(selectedMessageIDList.size());
    vector<string>::iterator iteratorMessageIDList;
    int i = 0;
    iteratorMessageIDList = selectedMessageIDList.begin();
    while (iteratorMessageIDList != selectedMessageIDList.end()) {
        string messageID = *iteratorMessageIDList;

        summaryVectorMsg->setMessageIDHashVector(i, messageID.c_str());

        i++;
        iteratorMessageIDList++;
    }
    int realPacketSize = 6 + 6 + (selectedMessageIDList.size() * KEPIDEMICROUTINGLAYER_MSG_ID_HASH_SIZE);
    summaryVectorMsg->setRealPacketSize(realPacketSize);
    summaryVectorMsg->setByteLength(realPacketSize);

    return summaryVectorMsg;
}

void KEpidemicRoutingLayer::finish()
{

    recordScalar("numEventsHandled", numEventsHandled);


    // clear resgistered app list
    while (registeredAppList.size() > 0) {
        list<AppInfo*>::iterator iteratorRegisteredApp = registeredAppList.begin();
        AppInfo *appInfo= *iteratorRegisteredApp;
        registeredAppList.remove(appInfo);
        delete appInfo;
    }

    // MODIFIED
    // clear data cache
    cacheList.clear();
    ///////////////////////

    // clear synced neighbour info list
    list<SyncedNeighbour*> syncedNeighbourList;
    while (syncedNeighbourList.size() > 0) {
        list<SyncedNeighbour*>::iterator iteratorSyncedNeighbour = syncedNeighbourList.begin();
        SyncedNeighbour *syncedNeighbour = *iteratorSyncedNeighbour;
        syncedNeighbourList.remove(syncedNeighbour);
        delete syncedNeighbour;
    }

    // remove triggers
    cancelEvent(cacheSizeReportingTimeoutEvent);
    delete cacheSizeReportingTimeoutEvent;

}

//
// The model for the Epidemic Routing where neighbours
// are ordered based on a selected set of criteria.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 02-may-2017
//
//

#include "KNeighbourMultipleOrderForwarding.h"
#include "inet/common/ModuleAccess.h"
#include <math.h>

Define_Module(KNeighbourMultipleOrderForwarding);

void KNeighbourMultipleOrderForwarding::initialize(int stage)
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
        orderType = par("orderType").stringValue();
        numEventsHandled = 0;

        syncedNeighbourListIHasChanged = TRUE;

    } else if (stage == 1) {
    //std::cout << "stage 1 " << "\n" ;

    } else if (stage == 2) {

        // create and setup cache size reporting trigger
        cacheSizeReportingTimeoutEvent = new cMessage("Cache Size Reporting Event");
        cacheSizeReportingTimeoutEvent->setKind(KNEIGHBOURMULTIPLEORDERFORWARDING_CACHESIZE_REP_EVENT);
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
        EV_FATAL << KNEIGHBOURMULTIPLEORDERFORWARDING_SIMMODULEINFO << "Something is radically wrong in initialisation \n";
    }
}

int KNeighbourMultipleOrderForwarding::numInitStages() const
{
    return 3;
}

void KNeighbourMultipleOrderForwarding::handleMessage(cMessage *msg)
{
    cGate *gate;
    char gateName[64];

    numEventsHandled++;

    // age the data in the cache only if needed (e.g. a message arrived)
    if (useTTL)
        ageDataInCache();

    // self messages
    if (msg->isSelfMessage()) {
        if (msg->getKind() == KNEIGHBOURMULTIPLEORDERFORWARDING_CACHESIZE_REP_EVENT) {

            // report cache size
            emit(currentCacheSizeBytesPeriodicSignal, currentCacheSize);

            // setup next cache size reporting trigger
            scheduleAt(simTime() + cacheSizeReportingFrequency, cacheSizeReportingTimeoutEvent);

        } else {
            EV_INFO << KNEIGHBOURMULTIPLEORDERFORWARDING_SIMMODULEINFO << "Received unexpected self message" << "\n";
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

            EV_INFO << KNEIGHBOURMULTIPLEORDERFORWARDING_SIMMODULEINFO << "Received unexpected packet" << "\n";
            delete msg;
        }
    }
}

void KNeighbourMultipleOrderForwarding::ageDataInCache()
{

    // remove expired data items
    int expiredFound = TRUE;
    while (expiredFound) {
        expiredFound = FALSE;

        CacheEntry *cacheEntry;
        list<CacheEntry*>::iterator iteratorCache;
        iteratorCache = cacheList.begin();
        while (iteratorCache != cacheList.end()) {
            cacheEntry = *iteratorCache;
            if (cacheEntry->validUntilTime < simTime().dbl()) {
                expiredFound = TRUE;
                break;
            }
            iteratorCache++;
        }
        if (expiredFound) {
            currentCacheSize -= cacheEntry->realPacketSize;

            emit(cacheBytesRemovedSignal, cacheEntry->realPayloadSize);
            emit(currentCacheSizeBytesSignal, currentCacheSize);
            emit(currentCacheSizeReportedCountSignal, (int) 1);

            emit(currentCacheSizeBytesSignal2, currentCacheSize);

            cacheList.remove(cacheEntry);
            delete cacheEntry;

        }
    }

}


void KNeighbourMultipleOrderForwarding::handleAppRegistrationMsg(cMessage *msg)
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

void KNeighbourMultipleOrderForwarding::handleDataMsgFromUpperLayer(cMessage *msg)
{
    KDataMsg *omnetDataMsg = dynamic_cast<KDataMsg*>(msg);

    CacheEntry *cacheEntry;
    list<CacheEntry*>::iterator iteratorCache;
    int found = FALSE;
    iteratorCache = cacheList.begin();
    while (iteratorCache != cacheList.end()) {
        cacheEntry = *iteratorCache;
        if (cacheEntry->dataName == omnetDataMsg->getDataName()) {
            found = TRUE;
            break;
        }

        iteratorCache++;
    }

    if (!found) {

        // apply caching policy if limited cache and cache is full
        if (maximumCacheSize != 0
                && (currentCacheSize + omnetDataMsg->getRealPayloadSize()) > maximumCacheSize
                && cacheList.size() > 0) {
            iteratorCache = cacheList.begin();
            CacheEntry *removingCacheEntry = *iteratorCache;
            iteratorCache = cacheList.begin();
            while (iteratorCache != cacheList.end()) {
                cacheEntry = *iteratorCache;
                if (cacheEntry->validUntilTime < removingCacheEntry->validUntilTime) {
                    removingCacheEntry = cacheEntry;
                }
                iteratorCache++;
            }
            currentCacheSize -= removingCacheEntry->realPayloadSize;

            emit(cacheBytesRemovedSignal, removingCacheEntry->realPayloadSize);
            emit(currentCacheSizeBytesSignal, currentCacheSize);
            emit(currentCacheSizeReportedCountSignal, (int) 1);

            emit(currentCacheSizeBytesSignal2, currentCacheSize);

            cacheList.remove(removingCacheEntry);
            delete removingCacheEntry;

        }

        cacheEntry = new CacheEntry;

        cacheEntry->messageID = omnetDataMsg->getDataName();
        cacheEntry->hopCount = 0;
        cacheEntry->dataName = omnetDataMsg->getDataName();
        cacheEntry->realPayloadSize = omnetDataMsg->getRealPayloadSize();
        cacheEntry->dummyPayloadContent = omnetDataMsg->getDummyPayloadContent();
        cacheEntry->validUntilTime = omnetDataMsg->getValidUntilTime();
        cacheEntry->realPacketSize = omnetDataMsg->getRealPacketSize();
        cacheEntry->initialOriginatorAddress = omnetDataMsg->getInitialOriginatorAddress();
        cacheEntry->destinationOriented = omnetDataMsg->getDestinationOriented();
        if (omnetDataMsg->getDestinationOriented()) {
            cacheEntry->finalDestinationAddress = omnetDataMsg->getFinalDestinationAddress();
        }
        cacheEntry->goodnessValue = omnetDataMsg->getGoodnessValue();
        cacheEntry->hopsTravelled = 0;

        cacheEntry->msgUniqueID = omnetDataMsg->getMsgUniqueID();
        cacheEntry->initialInjectionTime = omnetDataMsg->getInitialInjectionTime();

        cacheEntry->createdTime = simTime().dbl();
        cacheEntry->updatedTime = simTime().dbl();

        cacheList.push_back(cacheEntry);

        currentCacheSize += cacheEntry->realPayloadSize;

    }

    cacheEntry->lastAccessedTime = simTime().dbl();

    // log cache update or add
    if (found) {
        emit(cacheBytesUpdatedSignal, cacheEntry->realPayloadSize);
    } else {
        emit(cacheBytesAddedSignal, cacheEntry->realPayloadSize);
    }
    emit(currentCacheSizeBytesSignal, currentCacheSize);
    emit(currentCacheSizeReportedCountSignal, (int) 1);

    emit(currentCacheSizeBytesSignal2, currentCacheSize);

    delete msg;
}

void KNeighbourMultipleOrderForwarding::handleNeighbourListMsgFromLowerLayer(cMessage *msg)
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

    // order based on orderType set
    vector<NeighPrioEntry *> orderList;
    if (strstr(orderType.c_str(), "ARS") != NULL) {
        orderList = prioritizeAngleRSSISpeed(neighListMsg);
    } else if (strstr(orderType, "SRA") != NULL) {
        orderList = prioritizeSpeedRSSIAngle(neighListMsg);
    } else if (strstr(orderType, "ASR") != NULL) {
        orderList = prioritizeAngleSpeedRSSI(neighListMsg);
    } else if (strstr(orderType, "AS") != NULL) {
        orderList = prioritizeAngleSpeed(neighListMsg);
    } else if (strstr(orderType, "SA") != NULL) {
        orderList = prioritizeSpeedAngle(neighListMsg);
    } else if (strstr(orderType, "SR") != NULL) {
        orderList = prioritizeSpeedRSSI(neighListMsg);
    } else if (strstr(orderType, "RA") != NULL) {
        orderList = prioritizeRSSIAngle(neighListMsg);
    } else {
        EV_INFO << KNEIGHBOURMULTIPLEORDERFORWARDING_SIMMODULEINFO << "Undefined ordering" << "\n";
        delete msg;
        return;
    }    


    for (int i = 0; i < orderList.size(); i++) {
        string nodeMACAddress = neighListMsg->getNeighbourNameList(orderList[i]->neighIndex);
        
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
            double randomBackoffDuration = uniform(1.0, maximumRandomBackoffDuration);
            syncedNeighbour->randomBackoffEndTime = simTime().dbl() + randomBackoffDuration;

            syncWithNeighbour = FALSE;

        }

        // from previous questions - if syncing required
        if (syncWithNeighbour) {

            //cout << KNEIGHBOURMULTIPLEORDERFORWARDING_SIMMODULEINFO << "neigh Mac" << nodeMACAddress << "\n";
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
    }
    
    // clear lists
    for (int i = 0; i < orderList.size(); i++) {
        delete orderList[i];
    }

    // setup sync neighbour list for the next time
    setSyncingNeighbourInfoForNextRound();

    // synched neighbour list must be updated in next round
    // as there were changes
    syncedNeighbourListIHasChanged = TRUE;

    // delete the received neighbor list msg
    delete msg;
}


void KNeighbourMultipleOrderForwarding::handleDataMsgFromLowerLayer(cMessage *msg)
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
    if (omnetDataMsg->getHopCount() >= maximumHopCount) {
        cacheData = FALSE;
    }

    if(cacheData) {

        // insert/update cache
        CacheEntry *cacheEntry;
        list<CacheEntry*>::iterator iteratorCache;
        found = FALSE;
        iteratorCache = cacheList.begin();
        while (iteratorCache != cacheList.end()) {
            cacheEntry = *iteratorCache;
            if (cacheEntry->dataName == omnetDataMsg->getDataName()) {
                found = TRUE;
                break;
            }

            iteratorCache++;
        }

        if (!found) {

            // apply caching policy if limited cache and cache is full
            if (maximumCacheSize != 0
                && (currentCacheSize + omnetDataMsg->getRealPayloadSize()) > maximumCacheSize
                && cacheList.size() > 0) {
                iteratorCache = cacheList.begin();
                CacheEntry *removingCacheEntry = *iteratorCache;
                iteratorCache = cacheList.begin();
                while (iteratorCache != cacheList.end()) {
                    cacheEntry = *iteratorCache;
                    if (cacheEntry->validUntilTime < removingCacheEntry->validUntilTime) {
                        removingCacheEntry = cacheEntry;
                    }
                    iteratorCache++;
                }
                currentCacheSize -= removingCacheEntry->realPayloadSize;

                emit(cacheBytesRemovedSignal, removingCacheEntry->realPayloadSize);
                emit(currentCacheSizeBytesSignal, currentCacheSize);
                emit(currentCacheSizeReportedCountSignal, (int) 1);

                emit(currentCacheSizeBytesSignal2, currentCacheSize);

                cacheList.remove(removingCacheEntry);

                delete removingCacheEntry;
            }

            cacheEntry = new CacheEntry;

            cacheEntry->messageID = omnetDataMsg->getMessageID();
            cacheEntry->dataName = omnetDataMsg->getDataName();
            cacheEntry->realPayloadSize = omnetDataMsg->getRealPayloadSize();
            cacheEntry->dummyPayloadContent = omnetDataMsg->getDummyPayloadContent();
            cacheEntry->validUntilTime = omnetDataMsg->getValidUntilTime();
            cacheEntry->realPacketSize = omnetDataMsg->getRealPacketSize();
            cacheEntry->initialOriginatorAddress = omnetDataMsg->getInitialOriginatorAddress();
            cacheEntry->destinationOriented = omnetDataMsg->getDestinationOriented();
            if (omnetDataMsg->getDestinationOriented()) {
                cacheEntry->finalDestinationAddress = omnetDataMsg->getFinalDestinationAddress();
            }
            cacheEntry->goodnessValue = omnetDataMsg->getGoodnessValue();

            cacheEntry->msgUniqueID = omnetDataMsg->getMsgUniqueID();
            cacheEntry->initialInjectionTime = omnetDataMsg->getInitialInjectionTime();

            cacheEntry->createdTime = simTime().dbl();
            cacheEntry->updatedTime = simTime().dbl();

            cacheList.push_back(cacheEntry);

            currentCacheSize += cacheEntry->realPayloadSize;

        }

        cacheEntry->hopsTravelled = omnetDataMsg->getHopsTravelled();
        cacheEntry->hopCount = omnetDataMsg->getHopCount();
        cacheEntry->lastAccessedTime = simTime().dbl();

        // log cache update or add
        if (found) {
            emit(cacheBytesUpdatedSignal, cacheEntry->realPayloadSize);
        } else {
            emit(cacheBytesAddedSignal, cacheEntry->realPayloadSize);
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

void KNeighbourMultipleOrderForwarding::handleSummaryVectorMsgFromLowerLayer(cMessage *msg)
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
        CacheEntry *cacheEntry;
        list<CacheEntry*>::iterator iteratorCache;
        bool found = FALSE;
        iteratorCache = cacheList.begin();
        while (iteratorCache != cacheList.end()) {
            cacheEntry = *iteratorCache;
            if (cacheEntry->messageID == messageID) {
                found = TRUE;
                break;
            }

            iteratorCache++;
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
    int realPacketSize = 6 + 6 + (selectedMessageIDList.size() * KNEIGHBOURMULTIPLEORDERFORWARDING_MSG_ID_HASH_SIZE);
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

void KNeighbourMultipleOrderForwarding::handleDataRequestMsgFromLowerLayer(cMessage *msg)
{
    KDataRequestMsg *dataRequestMsg = dynamic_cast<KDataRequestMsg*>(msg);

    emit(dataReqBytesReceivedSignal, (long) dataRequestMsg->getByteLength());
    emit(totalBytesReceivedSignal, (long) dataRequestMsg->getByteLength());

    int i = 0;
    while (i < dataRequestMsg->getMessageIDHashVectorArraySize()) {
        string messageID = dataRequestMsg->getMessageIDHashVector(i);

        CacheEntry *cacheEntry;
        list<CacheEntry*>::iterator iteratorCache;
        bool found = FALSE;
        iteratorCache = cacheList.begin();
        while (iteratorCache != cacheList.end()) {
            cacheEntry = *iteratorCache;
            if (cacheEntry->messageID == messageID) {
                found = TRUE;
                break;
            }

            iteratorCache++;
        }

        if (found) {

            KDataMsg *dataMsg = new KDataMsg();

            dataMsg->setSourceAddress(ownMACAddress.c_str());
            dataMsg->setDestinationAddress(dataRequestMsg->getSourceAddress());
            dataMsg->setDataName(cacheEntry->dataName.c_str());
            dataMsg->setDummyPayloadContent(cacheEntry->dummyPayloadContent.c_str());
            dataMsg->setValidUntilTime(cacheEntry->validUntilTime);
            dataMsg->setRealPayloadSize(cacheEntry->realPayloadSize);
            // check KOPSMsg.msg on sizing mssages
            int realPacketSize = 6 + 6 + 2 + cacheEntry->realPayloadSize + 4 + 6 + 1;
            dataMsg->setRealPacketSize(realPacketSize);
            dataMsg->setByteLength(realPacketSize);
            dataMsg->setInitialOriginatorAddress(cacheEntry->initialOriginatorAddress.c_str());
            dataMsg->setDestinationOriented(cacheEntry->destinationOriented);
            if (cacheEntry->destinationOriented) {
                dataMsg->setFinalDestinationAddress(cacheEntry->finalDestinationAddress.c_str());
            }
            dataMsg->setMessageID(cacheEntry->messageID.c_str());
            dataMsg->setHopCount(cacheEntry->hopCount);
            dataMsg->setGoodnessValue(cacheEntry->goodnessValue);
            dataMsg->setHopsTravelled(cacheEntry->hopsTravelled);
            dataMsg->setMsgUniqueID(cacheEntry->msgUniqueID);
            dataMsg->setInitialInjectionTime(cacheEntry->initialInjectionTime);

            send(dataMsg, "lowerLayerOut");

            emit(dataBytesSentSignal, (long) dataMsg->getByteLength());
            emit(totalBytesSentSignal, (long) dataMsg->getByteLength());

        }

        i++;
    }

    delete msg;
}

KNeighbourMultipleOrderForwarding::SyncedNeighbour* KNeighbourMultipleOrderForwarding::getSyncingNeighbourInfo(string nodeMACAddress)
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

void KNeighbourMultipleOrderForwarding::setSyncingNeighbourInfoForNextRound()
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

void KNeighbourMultipleOrderForwarding::setSyncingNeighbourInfoForNoNeighboursOrEmptyCache()
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

KSummaryVectorMsg* KNeighbourMultipleOrderForwarding::makeSummaryVectorMessage()
{

    // identify the entries of the summary vector
    vector<string> selectedMessageIDList;
    CacheEntry *cacheEntry;
    list<CacheEntry*>::iterator iteratorCache;
    iteratorCache = cacheList.begin();
    while (iteratorCache != cacheList.end()) {
        cacheEntry = *iteratorCache;
        if ((cacheEntry->hopCount + 1) < maximumHopCount) {
            selectedMessageIDList.push_back(cacheEntry->messageID);
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
    int realPacketSize = 6 + 6 + (selectedMessageIDList.size() * KNEIGHBOURMULTIPLEORDERFORWARDING_MSG_ID_HASH_SIZE);
    summaryVectorMsg->setRealPacketSize(realPacketSize);
    summaryVectorMsg->setByteLength(realPacketSize);

    return summaryVectorMsg;
}


bool KNeighbourMultipleOrderForwarding::isAngleBetween(double angle, double lowerBoundary, double upperBoundary)
{
    // adjust for zero degrees within range
    float val = fmod((fmod((upperBoundary - lowerBoundary), 360) + 360), 360);
    if (val >= 180.0) {
        std::swap(lowerBoundary, upperBoundary);
    }

    // if it goes through zero
    if (lowerBoundary <= upperBoundary)
        return angle >= lowerBoundary && angle <= upperBoundary;

    else
        return angle >= lowerBoundary || angle <= upperBoundary;

}

void KNeighbourMultipleOrderForwarding::finish()
{

    recordScalar("numEventsHandled", numEventsHandled);


    // clear resgistered app list
    while (registeredAppList.size() > 0) {
        list<AppInfo*>::iterator iteratorRegisteredApp = registeredAppList.begin();
        AppInfo *appInfo= *iteratorRegisteredApp;
        registeredAppList.remove(appInfo);
        delete appInfo;
    }

    // clear registered app list
    while (cacheList.size() > 0) {
        list<CacheEntry*>::iterator iteratorCache = cacheList.begin();
        CacheEntry *cacheEntry= *iteratorCache;
        cacheList.remove(cacheEntry);
        delete cacheEntry;
    }

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

vector<NeighPrioEntry *> KNeighbourMultipleOrderForwarding::prioritizeAngleRSSISpeed(KNeighbourListMsg *neighListMsg)
{
    // compute angle checking boundaries
    double leftTop = (neighListMsg->getMyAngle() - 22.5 < 0.0 ? (neighListMsg->getMyAngle() - 22.5) + 360.0 : neighListMsg->getMyAngle() - 22.5);
    double rightTop = (neighListMsg->getMyAngle() + 22.5 > 360.0 ? (neighListMsg->getMyAngle() + 22.5) - 360.0 : neighListMsg->getMyAngle() + 22.5);
    double leftMiddle = (neighListMsg->getMyAngle() - 90.0 < 0.0 ? (neighListMsg->getMyAngle() - 90.0) + 360.0 : neighListMsg->getMyAngle() - 90.0);
    double rightMiddle = (neighListMsg->getMyAngle() + 90.0 > 360.0 ? (neighListMsg->getMyAngle() + 90.0) - 360.0 : neighListMsg->getMyAngle() + 90.0);
    double leftBottom = (neighListMsg->getMyAngle() - 157.5 < 0.0 ? (neighListMsg->getMyAngle() - 157.5) + 360.0 : neighListMsg->getMyAngle() - 157.5);
    double rightBottom = (neighListMsg->getMyAngle() + 157.5 > 360.0 ? (neighListMsg->getMyAngle() + 157.5) - 360.0 : neighListMsg->getMyAngle() + 157.5);
    
    // create working list
    vector<NeighPrioEntry *> baseOrderList;
    int i = 0;
    while (i < neighListMsg->getNeighbourNameListArraySize()) {
        NeighPrioEntry *neighPrioEntry = new NeighPrioEntry;
        
        neighPrioEntry->nodeMACAddress = neighListMsg->getNeighbourNameList(i);
        neighPrioEntry->neighIndex = i;
        neighPrioEntry->priority1 = 0;
        neighPrioEntry->priority2 = 0;
        neighPrioEntry->priority3 = 0;
        baseOrderList.push_back(neighPrioEntry);

        i++;
    }    
    
    // order priority1 by angle
    vector<NeighPrioEntry *> angleOrderList;
    for (int i = 1; i < 5; i++) {
        for (int j = 0; j < baseOrderList.size(); j++) {
            
            if (i == 1 && isAngleBetween(neighListMsg->getNeighbourAngleList(baseOrderList[j]->neighIndex), leftTop, rightTop)) {
                // same direction selection
                baseOrderList[j]->priority1 = i;
                angleOrderList.push_back(baseOrderList[j]);
                
            } else if (i == 2 && isAngleBetween(neighListMsg->getNeighbourAngleList(baseOrderList[j]->neighIndex), rightBottom, leftBottom)) {
                // opposite direction selection
                baseOrderList[j]->priority1 = i;
                angleOrderList.push_back(baseOrderList[j]);
             
            } else if (i == 3 && neighListMsg->getMyAngle() == 0.0 && neighListMsg->getNeighbourAngleList(j) == 0.0) {
                // stationary selection
                baseOrderList[j]->priority1 = i;
                angleOrderList.push_back(baseOrderList[j]);
                
            } else if (i == 4 && !(isAngleBetween(neighListMsg->getNeighbourAngleList(baseOrderList[j]->neighIndex), leftTop, rightTop)
                                   || isAngleBetween(neighListMsg->getNeighbourAngleList(baseOrderList[j]->neighIndex), rightBottom, leftBottom)
                                   || (neighListMsg->getMyAngle() == 0.0 && neighListMsg->getNeighbourAngleList(j) == 0.0))) {
                // other directions
                baseOrderList[j]->priority1 = i;
                angleOrderList.push_back(baseOrderList[j]);
            }
        }
    }

    // order priority2 by RSSI 
    vector<NeighPrioEntry *> angleAndRSSIOrderList;
    for (int i = 1; i < 5; i++) {
        for (int j = 1; j < 6; j++) {
            for (int k = 0; k < angleOrderList.size(); k++) {
                if (angleOrderList[k]->priority1 == i && neighListMsg->getNeighbourRangeCatList(angleOrderList[k]->neighIndex) == j) {
                    angleOrderList[k]->priority2 = j;
                    angleAndRSSIOrderList.push_back(angleOrderList[k]);
                }
            }
        }
    }

    // order priority3 by speed
    vector<NeighPrioEntry *> angleAndRSSIAndSpeedOrderList;
    for (int i = 1; i < 5; i++) {
        for (int j = 1; j < 6; j++) {
            vector<NeighPrioEntry *> tempList;
            for (int k = 0; k < angleAndRSSIOrderList.size(); k++) {
                if (angleAndRSSIOrderList[k]->priority1 == i && angleAndRSSIOrderList[k]->priority2 == j) {
                    tempList.push_back(angleAndRSSIOrderList[k]);
                }
            }
            
            // sort by speed
            for (int p = 0; p < tempList.size(); p++) {
                for (int q = (p + 1); q < tempList.size(); q++) {
                    if (neighListMsg->getNeighbourSpeedList(tempList[p]->neighIndex) > neighListMsg->getNeighbourSpeedList(tempList[q]->neighIndex)) {
                        NeighPrioEntry *temp = tempList[p];
                        tempList[p] = tempList[q];
                        tempList[q] = temp;
                    }
                }
            }
            
            for (int p = 0; p < tempList.size(); p++) {
                angleAndRSSIAndSpeedOrderList.push_back(tempList[p]);
            }
        }
    }

    // show list
    cout << "Own Angle: " << neighListMsg->getMyAngle() << ", Speed: " << neighListMsg->getMySpeed() << endl;
    for (int i = 0; i < angleAndRSSIAndSpeedOrderList.size(); i++) {
        cout << "Address: " << neighListMsg->getNeighbourNameList(angleAndRSSIAndSpeedOrderList[i]->neighIndex)
                << ", Angle:  " << neighListMsg->getNeighbourAngleList(angleAndRSSIAndSpeedOrderList[i]->neighIndex)
                << ", RangeCat: " << neighListMsg->getNeighbourRangeCatList(angleAndRSSIAndSpeedOrderList[i]->neighIndex)
                << ", Speed: " << neighListMsg->getNeighbourSpeedList(angleAndRSSIAndSpeedOrderList[i]->neighIndex)
                << endl;
    }

    return angleAndRSSIAndSpeedOrderList;
}

vector<NeighPrioEntry *> KNeighbourMultipleOrderForwarding::prioritizeSpeedRSSIAngle(neighListMsg)
{

}

vector<NeighPrioEntry *> KNeighbourMultipleOrderForwarding::prioritizeAngleSpeedRSSI(neighListMsg)
{

}

vector<NeighPrioEntry *> KNeighbourMultipleOrderForwarding::prioritizeAngleSpeed(neighListMsg)
{

    // compute angle checking boundaries
    double leftTop = (neighListMsg->getMyAngle() - 22.5 < 0.0 ? (neighListMsg->getMyAngle() - 22.5) + 360.0 : neighListMsg->getMyAngle() - 22.5);
    double rightTop = (neighListMsg->getMyAngle() + 22.5 > 360.0 ? (neighListMsg->getMyAngle() + 22.5) - 360.0 : neighListMsg->getMyAngle() + 22.5);
    double leftMiddle = (neighListMsg->getMyAngle() - 90.0 < 0.0 ? (neighListMsg->getMyAngle() - 90.0) + 360.0 : neighListMsg->getMyAngle() - 90.0);
    double rightMiddle = (neighListMsg->getMyAngle() + 90.0 > 360.0 ? (neighListMsg->getMyAngle() + 90.0) - 360.0 : neighListMsg->getMyAngle() + 90.0);
    double leftBottom = (neighListMsg->getMyAngle() - 157.5 < 0.0 ? (neighListMsg->getMyAngle() - 157.5) + 360.0 : neighListMsg->getMyAngle() - 157.5);
    double rightBottom = (neighListMsg->getMyAngle() + 157.5 > 360.0 ? (neighListMsg->getMyAngle() + 157.5) - 360.0 : neighListMsg->getMyAngle() + 157.5);
    
    // create working list
    vector<NeighPrioEntry *> baseOrderList;
    int i = 0;
    while (i < neighListMsg->getNeighbourNameListArraySize()) {
        NeighPrioEntry *neighPrioEntry = new NeighPrioEntry;
        
        neighPrioEntry->nodeMACAddress = neighListMsg->getNeighbourNameList(i);
        neighPrioEntry->neighIndex = i;
        neighPrioEntry->priority1 = 0;
        neighPrioEntry->priority2 = 0;
        neighPrioEntry->priority3 = 0;
        baseOrderList.push_back(neighPrioEntry);

        i++;
    }    
    
    // order priority1 by angle
    vector<NeighPrioEntry *> angleOrderList;
    for (int i = 1; i < 5; i++) {
        for (int j = 0; j < baseOrderList.size(); j++) {
            
            if (i == 1 && isAngleBetween(neighListMsg->getNeighbourAngleList(baseOrderList[j]->neighIndex), leftTop, rightTop)) {
                // same direction selection
                baseOrderList[j]->priority1 = i;
                angleOrderList.push_back(baseOrderList[j]);
                
            } else if (i == 2 && isAngleBetween(neighListMsg->getNeighbourAngleList(baseOrderList[j]->neighIndex), rightBottom, leftBottom)) {
                // opposite direction selection
                baseOrderList[j]->priority1 = i;
                angleOrderList.push_back(baseOrderList[j]);
             
            } else if (i == 3 && neighListMsg->getMyAngle() == 0.0 && neighListMsg->getNeighbourAngleList(j) == 0.0) {
                // stationary selection
                baseOrderList[j]->priority1 = i;
                angleOrderList.push_back(baseOrderList[j]);
                
            } else if (i == 4 && !(isAngleBetween(neighListMsg->getNeighbourAngleList(baseOrderList[j]->neighIndex), leftTop, rightTop)
                                   || isAngleBetween(neighListMsg->getNeighbourAngleList(baseOrderList[j]->neighIndex), rightBottom, leftBottom)
                                   || (neighListMsg->getMyAngle() == 0.0 && neighListMsg->getNeighbourAngleList(j) == 0.0))) {
                // other directions
                baseOrderList[j]->priority1 = i;
                angleOrderList.push_back(baseOrderList[j]);
            }
        }
    }

    // order priority2 by speed
    vector<NeighPrioEntry *> angleAndSpeedOrderList;
    for (int i = 1; i < 5; i++) {
        vector<NeighPrioEntry *> tempList;
        for (int k = 0; k < angleOrderList.size(); k++) {
            if (angleOrderList[k]->priority1 == i) {
                tempList.push_back(angleOrderList[k]);
            }
        }

        // sort by speed
        for (int p = 0; p < tempList.size(); p++) {
            for (int q = (p + 1); q < tempList.size(); q++) {
                if (neighListMsg->getNeighbourSpeedList(tempList[p]->neighIndex) > neighListMsg->getNeighbourSpeedList(tempList[q]->neighIndex)) {
                    NeighPrioEntry *temp = tempList[p];
                    tempList[p] = tempList[q];
                    tempList[q] = temp;
                }
            }
        }

        for (int p = 0; p < tempList.size(); p++) {
            angleAndSpeedOrderList.push_back(tempList[p]);
        }
    }

    // show list
    cout << "Own Angle: " << neighListMsg->getMyAngle() << ", Speed: " << neighListMsg->getMySpeed() << endl;
    for (int i = 0; i < angleAndSpeedOrderList.size(); i++) {
        cout << "Address: " << neighListMsg->getNeighbourNameList(angleAndSpeedOrderList[i]->neighIndex)
                << ", Angle:  " << neighListMsg->getNeighbourAngleList(angleAndSpeedOrderList[i]->neighIndex)
                << ", RangeCat: " << neighListMsg->getNeighbourRangeCatList(angleAndSpeedOrderList[i]->neighIndex)
                << ", Speed: " << neighListMsg->getNeighbourSpeedList(angleAndSpeedOrderList[i]->neighIndex)
                << endl;
    }

    return angleAndSpeedOrderList;
}

vector<NeighPrioEntry *> KNeighbourMultipleOrderForwarding::prioritizeSpeedAngle(neighListMsg)
{

	// this is not a pissible case because of the unlikeliness of the following 2 conditions
	// - have to travel at the same speed
	// - have to be in the same neighbourhood

}

vector<NeighPrioEntry *> KNeighbourMultipleOrderForwarding::prioritizeSpeedRSSI(neighListMsg)
{
    // compute angle checking boundaries
    double leftTop = (neighListMsg->getMyAngle() - 22.5 < 0.0 ? (neighListMsg->getMyAngle() - 22.5) + 360.0 : neighListMsg->getMyAngle() - 22.5);
    double rightTop = (neighListMsg->getMyAngle() + 22.5 > 360.0 ? (neighListMsg->getMyAngle() + 22.5) - 360.0 : neighListMsg->getMyAngle() + 22.5);
    double leftMiddle = (neighListMsg->getMyAngle() - 90.0 < 0.0 ? (neighListMsg->getMyAngle() - 90.0) + 360.0 : neighListMsg->getMyAngle() - 90.0);
    double rightMiddle = (neighListMsg->getMyAngle() + 90.0 > 360.0 ? (neighListMsg->getMyAngle() + 90.0) - 360.0 : neighListMsg->getMyAngle() + 90.0);
    double leftBottom = (neighListMsg->getMyAngle() - 157.5 < 0.0 ? (neighListMsg->getMyAngle() - 157.5) + 360.0 : neighListMsg->getMyAngle() - 157.5);
    double rightBottom = (neighListMsg->getMyAngle() + 157.5 > 360.0 ? (neighListMsg->getMyAngle() + 157.5) - 360.0 : neighListMsg->getMyAngle() + 157.5);
    
    // create working list
    vector<NeighPrioEntry *> baseOrderList;
    int i = 0;
    while (i < neighListMsg->getNeighbourNameListArraySize()) {
        NeighPrioEntry *neighPrioEntry = new NeighPrioEntry;
        
        neighPrioEntry->nodeMACAddress = neighListMsg->getNeighbourNameList(i);
        neighPrioEntry->neighIndex = i;
        neighPrioEntry->priority1 = 0;
        neighPrioEntry->priority2 = 0;
        neighPrioEntry->priority3 = 0;
        baseOrderList.push_back(neighPrioEntry);

        i++;
    }    

    // order priority1 by speed
    vector<NeighPrioEntry *> speedOrderList;
    for (i = 0; i < baseOrderList.size(); i++) {
        for (int j = i + 1; j < baseOrderList.size(); j++) {
            if (neighListMsg->getNeighbourSpeedList(baseOrderList[i]->neighIndex) > neighListMsg->getNeighbourSpeedList(baseOrderList[j]->neighIndex)) {
                NeighPrioEntry *temp = baseOrderList[i];
                baseOrderList[i] = baseOrderList[j];
                baseOrderList[j] = temp;
            }
        }
        speedOrderList.push_back(baseOrderList[i]);
    }
    priority1 = 1;
    prevSpeed = neighListMsg->getNeighbourSpeedList(speedOrderList[0]->neighIndex) 
    for (i = 0; i < speedOrderList.size(); i++) {
        if neighListMsg->getNeighbourSpeedList(speedOrderList[i]->neighIndex) > prevSpeed) {
            priority1++;
            speedOrderList[i]->priority1 = priority1;
            prevSpeed = neighListMsg->getNeighbourSpeedList(speedOrderList[i]->neighIndex)
        } else {
            speedOrderList[i]->priority1 = priority1;
        }
    }
    maxSpeedPriority = priority1;
    
    // order priority2 by RSSI
    vector<NeighPrioEntry *> rssiOrderList;
    for (i = 1; i <= maxSpeedPriority; i++)
        vector<NeighPrioEntry *> tempList;
        for (int j = 0; j < speedOrderList.size(); j++) {
            if (speedOrderList[j]->priority1 == i)) {
                tempList.push_back(speedOrderList[j])
            }
        }
        for (int j = 0; j < tempList.size(); j++) {
            for (int k = j + 1; k < tempList.size(); k++) {
                if (neighListMsg->getNeighbourRangeCatList(tempList[j]->neighIndex) > neighListMsg->getNeighbourRangeCatList(tempList[k]->neighIndex)
                    NeighPrioEntry *temp = tempList[j];
                    tempList[j] = tempList[k];
                    tempList[k] = temp;
                }
            }
        }
        for (int j = 0; j < tempList.size(); j++) {
            tempList[j]->priority2 = j;
            rssiOrderList.push_back(tempList[j])
        }
    }

    // show list
    cout << "Own Angle: " << neighListMsg->getMyAngle() << ", Speed: " << neighListMsg->getMySpeed() << endl;
    for (int i = 0; i < rssiOrderList.size(); i++) {
        cout << "Address: " << neighListMsg->getNeighbourNameList(rssiOrderList[i]->neighIndex)
                << ", Speed: " << neighListMsg->getNeighbourSpeedList(rssiOrderList[i]->neighIndex)
                << ", RangeCat: " << neighListMsg->getNeighbourRangeCatList(rssiOrderList[i]->neighIndex)
                << endl;
    }

    return rssiOrderList;  
}

vector<NeighPrioEntry *> KNeighbourMultipleOrderForwarding::prioritizeRSSIAngle(neighListMsg)
{
    // compute angle checking boundaries
    double leftTop = (neighListMsg->getMyAngle() - 22.5 < 0.0 ? (neighListMsg->getMyAngle() - 22.5) + 360.0 : neighListMsg->getMyAngle() - 22.5);
    double rightTop = (neighListMsg->getMyAngle() + 22.5 > 360.0 ? (neighListMsg->getMyAngle() + 22.5) - 360.0 : neighListMsg->getMyAngle() + 22.5);
    double leftMiddle = (neighListMsg->getMyAngle() - 90.0 < 0.0 ? (neighListMsg->getMyAngle() - 90.0) + 360.0 : neighListMsg->getMyAngle() - 90.0);
    double rightMiddle = (neighListMsg->getMyAngle() + 90.0 > 360.0 ? (neighListMsg->getMyAngle() + 90.0) - 360.0 : neighListMsg->getMyAngle() + 90.0);
    double leftBottom = (neighListMsg->getMyAngle() - 157.5 < 0.0 ? (neighListMsg->getMyAngle() - 157.5) + 360.0 : neighListMsg->getMyAngle() - 157.5);
    double rightBottom = (neighListMsg->getMyAngle() + 157.5 > 360.0 ? (neighListMsg->getMyAngle() + 157.5) - 360.0 : neighListMsg->getMyAngle() + 157.5);
    
    // create working list
    vector<NeighPrioEntry *> baseOrderList;
    int i = 0;
    while (i < neighListMsg->getNeighbourNameListArraySize()) {
        NeighPrioEntry *neighPrioEntry = new NeighPrioEntry;
        
        neighPrioEntry->nodeMACAddress = neighListMsg->getNeighbourNameList(i);
        neighPrioEntry->neighIndex = i;
        neighPrioEntry->priority1 = 0;
        neighPrioEntry->priority2 = 0;
        neighPrioEntry->priority3 = 0;
        baseOrderList.push_back(neighPrioEntry);

        i++;
    }    

    // order priority1 by RSSI
    vector<NeighPrioEntry *> rssiOrderList;
    for (int i = 1; i < 6; i++) {
        for (int j = 0; j < baseOrderList.size(); j++) {
            if (neighListMsg->getNeighbourRangeCatList(baseOrderList[j]->neighIndex) == i) {
                baseOrderList[k]->priority1 = i;
                rssiOrderList.push_back(baseOrderList[k]);
            }
        }
    }

    // order priority2 by angle
    vector<NeighPrioEntry *> rssiAndAngleOrderList;
    for (int i = 1; i < 6; i++) {
        vector<NeighPrioEntry *> tempList;
        for (int j = 0; j < rssiOrderList.size(); j++) {

            if (i == 1 && isAngleBetween(neighListMsg->getNeighbourAngleList(baseOrderList[j]->neighIndex), leftTop, rightTop)) {
                // same direction selection
                rssiOrderList[j]->priority2 = 1;
                tempList.push_back(rssiOrderList[j]);
                
            } else if (i == 2 && isAngleBetween(neighListMsg->getNeighbourAngleList(baseOrderList[j]->neighIndex), rightBottom, leftBottom)) {
                // opposite direction selection
                rssiOrderList[j]->priority2 = 2;
                tempList.push_back(rssiOrderList[j]);
             
            } else if (i == 3 && neighListMsg->getMyAngle() == 0.0 && neighListMsg->getNeighbourAngleList(j) == 0.0) {
                // stationary selection
                rssiOrderList[j]->priority2 = 3;
                tempList.push_back(rssiOrderList[j]);
                
            } else if (i == 4 && !(isAngleBetween(neighListMsg->getNeighbourAngleList(baseOrderList[j]->neighIndex), leftTop, rightTop)
                                   || isAngleBetween(neighListMsg->getNeighbourAngleList(baseOrderList[j]->neighIndex), rightBottom, leftBottom)
                                   || (neighListMsg->getMyAngle() == 0.0 && neighListMsg->getNeighbourAngleList(j) == 0.0))) {
                // other directions
			    rssiOrderList[j]->priority2 = 4;
				tempList.push_back(rssiOrderList[j]);
            }
        }

        for (int j = 0; j < tempList.size(); j++) {
	        for (int k = j + 1; k < tempList.size(); k++) {
                if (tempList[j]->priority2 > tempList[k]->priority2)
                    NeighPrioEntry *temp = tempList[j];
                    tempList[j] = tempList[k];
                    tempList[k] = temp;
                }
            }
        }
		
        for (int j = 0; j < tempList.size(); j++) {
            rssiAndAngleOrderList.push_back(tempList[j])
        }
		
	    // show list
	    cout << "Own Angle: " << neighListMsg->getMyAngle() << ", Speed: " << neighListMsg->getMySpeed() << endl;
	    for (int i = 0; i < rssiAndAngleOrderList.size(); i++) {
	        cout << "Address: " << neighListMsg->getNeighbourNameList(rssiAndAngleOrderList[i]->neighIndex)
	                << ", Speed: " << neighListMsg->getNeighbourSpeedList(rssiAndAngleOrderList[i]->neighIndex)
	                << ", RangeCat: " << neighListMsg->getNeighbourRangeCatList(rssiAndAngleOrderList[i]->neighIndex)
	                << endl;
	    }
    }
	return rssiAndAngleOrderList;  
}


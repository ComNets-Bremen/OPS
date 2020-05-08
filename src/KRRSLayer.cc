//
// The model implementation for the Randomized Rumor Spreading (RRS) layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 25-aug-2015
//
// New user parameters to control how often data is sent
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 02-may-2020
//

#include "KRRSLayer.h"

Define_Module(KRRSLayer);

void KRRSLayer::initialize(int stage)
{

    if (stage == 0) {
        // get parameters
        ownMACAddress = par("ownMACAddress").stringValue();
        nextAppID = 1;
        maximumCacheSize = par("maximumCacheSize");
        broadcastRRS = par("broadcastRRS");
        usedRNG = par("usedRNG");
        ageCache = par("ageCache");
        cacheSizeReportingFrequency = par("cacheSizeReportingFrequency");
        sendOnNeighReportingFrequency = par("sendOnNeighReportingFrequency");
        sendFrequencyWhenNotOnNeighFrequency = par("sendFrequencyWhenNotOnNeighFrequency");

        // set other parameters
        broadcastMACAddress = "FF:FF:FF:FF:FF:FF";

        //init other variables
        currentCacheSize = 0;
        if (!sendOnNeighReportingFrequency) {
            nextDataSendTime = simTime() + sendFrequencyWhenNotOnNeighFrequency;
        }
    } else if (stage == 1) {

    } else if (stage == 2) {

        // setup the trigger to age data
        if (ageCache) {
            ageDataTimeoutEvent = new cMessage("Age Data Timeout Event");
            ageDataTimeoutEvent->setKind(KRRSLAYER_EVENTTYPE_AGEDATA);
            scheduleAt(simTime() + 1.0, ageDataTimeoutEvent);
        }

        // create and setup cache size reporting trigger
        cacheSizeReportingTimeoutEvent = new cMessage("Cache Size Reporting Event");
        cacheSizeReportingTimeoutEvent->setKind(KRRSLAYER_CACHESIZE_REP_EVENT);
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
        EV_FATAL << KRRSLAYER_SIMMODULEINFO << "Something is radically wrong in initialisation \n";
    }
}

int KRRSLayer::numInitStages() const
{
    return 3;
}

void KRRSLayer::handleMessage(cMessage *msg)
{
    cGate *gate;
    char gateName[64];
    KRegisterAppMsg *regAppMsg;
    KDataMsg *omnetDataMsg;
    KFeedbackMsg *omnetFeedbackMsg;
    KNeighbourListMsg *neighListMsg;

    // self messages
    if (msg->isSelfMessage()) {

        // age data trigger fired
        if (msg->getKind() == KRRSLAYER_EVENTTYPE_AGEDATA) {

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
                    cacheList.remove(cacheEntry);

                    emit(cacheBytesRemovedSignal, cacheEntry->realPacketSize);
                    emit(currentCacheSizeBytesSignal, currentCacheSize);
                    emit(currentCacheSizeReportedCountSignal, (int) 1);

                    emit(currentCacheSizeBytesSignal2, currentCacheSize);

                    delete cacheEntry;
                }
            }

            // setup next age data trigger
            scheduleAt(simTime() + 1.0, msg);

        } else if (msg->getKind() == KRRSLAYER_CACHESIZE_REP_EVENT) {

            // report cache size
            emit(currentCacheSizeBytesPeriodicSignal, currentCacheSize);

            // setup next cache size reporting trigger
            scheduleAt(simTime() + cacheSizeReportingFrequency, cacheSizeReportingTimeoutEvent);

        } else {
            EV_INFO << KRRSLAYER_SIMMODULEINFO << "Received unexpected self message" << "\n";
            delete msg;
        }

    // messages from other layers
    } else {

       // get message arrival gate name
        gate = msg->getArrivalGate();
        strcpy(gateName, gate->getName());

        // app registration message arrived from the upper layer (app layer)
        if (strstr(gateName, "upperLayerIn") != NULL && (regAppMsg = dynamic_cast<KRegisterAppMsg*>(msg)) != NULL) {

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

        // data message arrived from the upper layer (app layer)
        } else if (strstr(gateName, "upperLayerIn") != NULL && (omnetDataMsg = dynamic_cast<KDataMsg*>(msg)) != NULL) {

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
                        && (currentCacheSize + omnetDataMsg->getRealPacketSize()) > maximumCacheSize
                        && cacheList.size() > 0) {
                    iteratorCache = cacheList.begin();
                    advance(iteratorCache, 0);
                    CacheEntry *removingCacheEntry = *iteratorCache;
                    iteratorCache = cacheList.begin();
                    while (iteratorCache != cacheList.end()) {
                        cacheEntry = *iteratorCache;
                        if (cacheEntry->validUntilTime < removingCacheEntry->validUntilTime) {
                            removingCacheEntry = cacheEntry;
                        }
                        iteratorCache++;
                    }
                    currentCacheSize -= removingCacheEntry->realPacketSize;
                    cacheList.remove(removingCacheEntry);

                    emit(cacheBytesRemovedSignal, removingCacheEntry->realPacketSize);
                    emit(currentCacheSizeBytesSignal, currentCacheSize);
                    emit(currentCacheSizeReportedCountSignal, (int) 1);

                    emit(currentCacheSizeBytesSignal2, currentCacheSize);

                    delete removingCacheEntry;

                }

                cacheEntry = new CacheEntry;
                cacheEntry->dataName = omnetDataMsg->getDataName();
                cacheEntry->goodnessValue = omnetDataMsg->getGoodnessValue();
                cacheEntry->realPayloadSize = omnetDataMsg->getRealPayloadSize();
                cacheEntry->dummyPayloadContent = omnetDataMsg->getDummyPayloadContent();
                cacheEntry->msgType = omnetDataMsg->getMsgType();
                cacheEntry->validUntilTime = omnetDataMsg->getValidUntilTime();

                cacheEntry->realPacketSize = omnetDataMsg->getRealPacketSize();

                // cacheEntry->finalDestinationNodeName = omnetDataMsg->getFinalDestinationNodeName();
                cacheEntry->initialOriginatorAddress = omnetDataMsg->getInitialOriginatorAddress();
                cacheEntry->destinationOriented = omnetDataMsg->getDestinationOriented();
                if (omnetDataMsg->getDestinationOriented()) {
                    cacheEntry->finalDestinationAddress = omnetDataMsg->getFinalDestinationAddress();
                }
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

        // feedback message arrived from the upper layer (app layer)
        } else if (strstr(gateName, "upperLayerIn") != NULL && (omnetFeedbackMsg = dynamic_cast<KFeedbackMsg*>(msg)) != NULL) {

            // with RRS, no feedback is considered

            delete msg;

        // neighbour list message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && (neighListMsg = dynamic_cast<KNeighbourListMsg*>(msg)) != NULL) {

            // if there are nodes in the neighbourhood and entries in the cache
            // send a randomly selected cached data out
            if (neighListMsg->getNeighbourNameListArraySize() > 0 && cacheList.size() > 0) {

                // but before that check what kind of data send frequency is selected
                // and whether its time to send
                if (sendOnNeighReportingFrequency || simTime() > nextDataSendTime) {

                    int cacheIndex = 0;
                    if (cacheList.size() > 1) {
                        cacheIndex = intuniform(0, (cacheList.size() - 1), usedRNG);
                    }

                    list<CacheEntry*>::iterator iteratorCache = cacheList.begin();
                    advance(iteratorCache, cacheIndex);
                    CacheEntry *cacheEntry = *iteratorCache;

                    KDataMsg *dataMsg = new KDataMsg();

                    dataMsg->setSourceAddress(ownMACAddress.c_str());

                    if (broadcastRRS) {
                        dataMsg->setDestinationAddress(broadcastMACAddress.c_str());
                    } else {
                        int neighbourIndex = intuniform(0, (neighListMsg->getNeighbourNameListArraySize() - 1), usedRNG);
                        dataMsg->setDestinationAddress(neighListMsg->getNeighbourNameList(neighbourIndex));
                    }

                    dataMsg->setDataName(cacheEntry->dataName.c_str());
                    dataMsg->setGoodnessValue(cacheEntry->goodnessValue);

                    dataMsg->setRealPayloadSize(cacheEntry->realPayloadSize);
                    dataMsg->setDummyPayloadContent(cacheEntry->dummyPayloadContent.c_str());
                    int realPacketSize = 6 + 6 + 2 + cacheEntry->realPayloadSize + 4 + 6 + 1;

                    dataMsg->setRealPacketSize(realPacketSize);
                    dataMsg->setByteLength(realPacketSize);

                    dataMsg->setMsgType(cacheEntry->msgType);
                    dataMsg->setValidUntilTime(cacheEntry->validUntilTime);

                    // dataMsg->setFinalDestinationNodeName(cacheEntry->finalDestinationNodeName.c_str());
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

                    if (!sendOnNeighReportingFrequency) {
                        nextDataSendTime = simTime() + sendFrequencyWhenNotOnNeighFrequency;
                    }
                }
            }

            delete msg;

        // data message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && (omnetDataMsg = dynamic_cast<KDataMsg*>(msg)) != NULL) {

            int found;

            // increment the travelled hop count
            omnetDataMsg->setHopsTravelled(omnetDataMsg->getHopsTravelled() + 1);
            omnetDataMsg->setHopCount(omnetDataMsg->getHopCount() + 1);

            emit(dataBytesReceivedSignal, (long) omnetDataMsg->getByteLength());
            emit(totalBytesReceivedSignal, (long) omnetDataMsg->getByteLength());
            emit(hopsTravelledSignal, (long) omnetDataMsg->getHopsTravelled());
            emit(hopsTravelledCountSignal, 1);

            // if destination oriented data sent around, then cache message only if not destined to self
            // or if no destination in data, cache all messages
            bool cacheData = TRUE;
            // if (omnetDataMsg->getDestinationOriented()
            //     && strstr(getParentModule()->getFullName(), omnetDataMsg->getFinalDestinationNodeName()) != NULL) {
            if (omnetDataMsg->getDestinationOriented() 
                && strstr(ownMACAddress.c_str(), omnetDataMsg->getFinalDestinationAddress()) != NULL) {
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
                        && (currentCacheSize + omnetDataMsg->getRealPacketSize()) > maximumCacheSize
                    && cacheList.size() > 0) {
                        iteratorCache = cacheList.begin();
                        advance(iteratorCache, 0);
                        CacheEntry *removingCacheEntry = *iteratorCache;
                        iteratorCache = cacheList.begin();
                        while (iteratorCache != cacheList.end()) {
                            cacheEntry = *iteratorCache;
                            if (cacheEntry->validUntilTime < removingCacheEntry->validUntilTime) {
                                removingCacheEntry = cacheEntry;
                            }
                            iteratorCache++;
                        }
                        currentCacheSize -= removingCacheEntry->realPacketSize;
                        cacheList.remove(removingCacheEntry);

                        emit(cacheBytesRemovedSignal, removingCacheEntry->realPacketSize);
                        emit(currentCacheSizeBytesSignal, currentCacheSize);
                        emit(currentCacheSizeReportedCountSignal, (int) 1);

                        emit(currentCacheSizeBytesSignal2, currentCacheSize);

                        delete removingCacheEntry;

                    }

                    cacheEntry = new CacheEntry;

                    cacheEntry->messageID = omnetDataMsg->getMessageID();
                    cacheEntry->dataName = omnetDataMsg->getDataName();
                    cacheEntry->realPayloadSize = omnetDataMsg->getRealPayloadSize();
                    cacheEntry->dummyPayloadContent = omnetDataMsg->getDummyPayloadContent();
                    cacheEntry->msgType = omnetDataMsg->getMsgType();
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

        // feedback message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && (omnetFeedbackMsg = dynamic_cast<KFeedbackMsg*>(msg)) != NULL) {

            // with RRS, no feedback is considered
            delete msg;

        // received some unexpected packet
        } else {

            EV_INFO << KRRSLAYER_SIMMODULEINFO << "Received unexpected packet" << "\n";
            delete msg;
        }
    }
}

void KRRSLayer::finish()
{
    // remove age data trigger
    if (ageCache) {
        cancelEvent(ageDataTimeoutEvent);
        delete ageDataTimeoutEvent;
        ageDataTimeoutEvent = NULL;
    }

    // remove triggers
    cancelEvent(cacheSizeReportingTimeoutEvent);
    delete cacheSizeReportingTimeoutEvent;
}

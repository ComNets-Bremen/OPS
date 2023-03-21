//
// The model implementation for the BubbleRAP Forwarding.
//
// @author : Akhil Simha Neela (neela@uni-bremen.de)
// @date   : 24-jan-2023
//

#include "KBubbleRAPForwarding.h"
#include <vector>

Define_Module(KBubbleRAPForwarding);

void KBubbleRAPForwarding::initialize(int stage) {

    if (stage == 0) {
        // get parameters
        nodeIndex = par("nodeIndex"); // Added to include the nodeIndex
        ownMACAddress = par("ownMACAddress").stringValue();
        nextAppID = 1;
        maximumCacheSize = par("maximumCacheSize");

        usedRNG = par("usedRNG");
        ageCache = par("ageCache");
        cacheSizeReportingFrequency = par("cacheSizeReportingFrequency");
        sendOnNeighReportingFrequency = par("sendOnNeighReportingFrequency");
        sendFrequencyWhenNotOnNeighFrequency = par(
                "sendFrequencyWhenNotOnNeighFrequency");


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
            ageDataTimeoutEvent->setKind(
            KBUBBLERAPFORWARDING_EVENTTYPE_AGEDATA);
            scheduleAt(simTime() + 1.0, ageDataTimeoutEvent);
        }

        // create and setup cache size reporting trigger
        cacheSizeReportingTimeoutEvent = new cMessage(
                "Cache Size Reporting Event");
        cacheSizeReportingTimeoutEvent->setKind(
        KBUBBLERAPFORWARDING_CACHESIZE_REP_EVENT);
        scheduleAt(simTime() + cacheSizeReportingFrequency,
                cacheSizeReportingTimeoutEvent);

        // setup statistics signals
        dataBytesReceivedSignal = registerSignal("fwdDataBytesReceived");
        totalBytesReceivedSignal = registerSignal("fwdTotalBytesReceived");
        hopsTravelledSignal = registerSignal("fwdHopsTravelled");
        hopsTravelledCountSignal = registerSignal("fwdHopsTravelledCount");

        cacheBytesRemovedSignal = registerSignal("fwdCacheBytesRemoved");
        cacheBytesAddedSignal = registerSignal("fwdCacheBytesAdded");
        cacheBytesUpdatedSignal = registerSignal("fwdCacheBytesUpdated");
        currentCacheSizeBytesSignal = registerSignal(
                "fwdCurrentCacheSizeBytes");
        currentCacheSizeReportedCountSignal = registerSignal(
                "fwdCurrentCacheSizeReportedCount");
        currentCacheSizeBytesPeriodicSignal = registerSignal(
                "fwdCurrentCacheSizeBytesPeriodic");

        currentCacheSizeBytesSignal2 = registerSignal(
                "fwdCurrentCacheSizeBytes2");

        dataBytesSentSignal = registerSignal("fwdDataBytesSent");
        totalBytesSentSignal = registerSignal("fwdTotalBytesSent");

    } else {
        EV_FATAL << KBUBBLERAPFORWARDING_SIMMODULEINFO
                        << "Something is radically wrong in initialisation \n";
    }
}

int KBubbleRAPForwarding::numInitStages() const {
    return 3;
}

void KBubbleRAPForwarding::handleMessage(cMessage *msg) {
    cGate *gate;
    char gateName[64];
    KRegisterAppMsg *regAppMsg;
    KDataMsg *omnetDataMsg;
    KFeedbackMsg *omnetFeedbackMsg;
    KNeighbourListMsg *neighListMsg;

    // self messages
    if (msg->isSelfMessage()) {

        // age data trigger fired
        if (msg->getKind() == KBUBBLERAPFORWARDING_EVENTTYPE_AGEDATA) {

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

        } else if (msg->getKind() == KBUBBLERAPFORWARDING_CACHESIZE_REP_EVENT) {

            // report cache size
            emit(currentCacheSizeBytesPeriodicSignal, currentCacheSize);

            // setup next cache size reporting trigger
            scheduleAt(simTime() + cacheSizeReportingFrequency,
                    cacheSizeReportingTimeoutEvent);

        } else {
            EV_INFO << KBUBBLERAPFORWARDING_SIMMODULEINFO
                           << "Received unexpected self message" << "\n";
            delete msg;
        }

        // messages from other layers
    } else {

        // get message arrival gate name
        gate = msg->getArrivalGate();
        strcpy(gateName, gate->getName());

        // app registration message arrived from the upper layer (app layer)
        if (strstr(gateName, "upperLayerIn") != NULL && (regAppMsg =
                dynamic_cast<KRegisterAppMsg*>(msg)) != NULL) {

            AppInfo *appInfo = NULL;
            int found = FALSE;
            list<AppInfo*>::iterator iteratorRegisteredApps =
                    registeredAppList.begin();
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
        } else if (strstr(gateName, "upperLayerIn") != NULL && (omnetDataMsg =
                dynamic_cast<KDataMsg*>(msg)) != NULL) {

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
                        && (currentCacheSize + omnetDataMsg->getRealPacketSize())
                                > maximumCacheSize && cacheList.size() > 0) {
                    iteratorCache = cacheList.begin();
                    advance(iteratorCache, 0);
                    CacheEntry *removingCacheEntry = *iteratorCache;
                    iteratorCache = cacheList.begin();
                    while (iteratorCache != cacheList.end()) {
                        cacheEntry = *iteratorCache;
                        if (cacheEntry->validUntilTime
                                < removingCacheEntry->validUntilTime) {
                            removingCacheEntry = cacheEntry;
                        }
                        iteratorCache++;
                    }
                    currentCacheSize -= removingCacheEntry->realPacketSize;
                    cacheList.remove(removingCacheEntry);

                    emit(cacheBytesRemovedSignal,
                            removingCacheEntry->realPacketSize);
                    emit(currentCacheSizeBytesSignal, currentCacheSize);
                    emit(currentCacheSizeReportedCountSignal, (int) 1);

                    emit(currentCacheSizeBytesSignal2, currentCacheSize);

                    delete removingCacheEntry;

                }

                cacheEntry = new CacheEntry;
                cacheEntry->dataName = omnetDataMsg->getDataName();
                cacheEntry->goodnessValue = omnetDataMsg->getGoodnessValue();
                cacheEntry->realPayloadSize =
                        omnetDataMsg->getRealPayloadSize();
                cacheEntry->dummyPayloadContent =
                        omnetDataMsg->getDummyPayloadContent();
                cacheEntry->msgType = omnetDataMsg->getMsgType();
                cacheEntry->validUntilTime = omnetDataMsg->getValidUntilTime();

                cacheEntry->realPacketSize = omnetDataMsg->getRealPacketSize();

                //cacheEntry->finalDestinationNodeName = omnetDataMsg->getFinalDestinationNodeName();
                cacheEntry->initialOriginatorAddress =
                        omnetDataMsg->getInitialOriginatorAddress();
                //cout << omnetDataMsg->getInitialOriginatorAddress() << endl;
                cacheEntry->destinationOriented =
                        omnetDataMsg->getDestinationOriented();
                if (omnetDataMsg->getDestinationOriented()) {
                    cacheEntry->finalDestinationAddress =
                            omnetDataMsg->getFinalDestinationAddress();
                }
                cacheEntry->hopsTravelled = 0;

                cacheEntry->msgUniqueID = omnetDataMsg->getMsgUniqueID();
                cacheEntry->initialInjectionTime =
                        omnetDataMsg->getInitialInjectionTime();

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
        } else if (strstr(gateName, "upperLayerIn") != NULL
                && (omnetFeedbackMsg = dynamic_cast<KFeedbackMsg*>(msg)) != NULL) {

            // with RRS, no feedback is considered

            delete msg;

            // Neighbor list message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && (neighListMsg =
                dynamic_cast<KNeighbourListMsg*>(msg)) != NULL) {

            int neighbourIndex = intuniform(0,
                    (neighListMsg->getNeighbourNameListArraySize() - 1),
                    usedRNG);

            // if there are nodes in the neighborhood and entries in the cache
            // send a randomly selected cached data out
            if (neighListMsg->getNeighbourNameListArraySize() > 0
                    && cacheList.size() > 0) {

                // but before that check what kind of data send frequency is selected
                // and whether its time to send
                if (sendOnNeighReportingFrequency
                        || simTime() > nextDataSendTime) {

                    int cacheIndex = 0;
                    if (cacheList.size() > 1) {
                        cacheIndex = intuniform(0, (cacheList.size() - 1),
                                usedRNG);
                    }

                    list<CacheEntry*>::iterator iteratorCache =
                            cacheList.begin();
                    advance(iteratorCache, cacheIndex);
                    CacheEntry *cacheEntry = *iteratorCache;

                    KDataMsg *dataMsg = new KDataMsg();

                    dataMsg->setSourceAddress(ownMACAddress.c_str());

                    dataMsg->setDataName(cacheEntry->dataName.c_str());
                    dataMsg->setGoodnessValue(cacheEntry->goodnessValue);

                    dataMsg->setRealPayloadSize(cacheEntry->realPayloadSize);
                    dataMsg->setDummyPayloadContent(
                            cacheEntry->dummyPayloadContent.c_str());
                    int realPacketSize = 6 + 6 + 2 + cacheEntry->realPayloadSize
                            + 4 + 6 + 1;

                    dataMsg->setRealPacketSize(realPacketSize);
                    dataMsg->setByteLength(realPacketSize);

                    dataMsg->setMsgType(cacheEntry->msgType);
                    dataMsg->setValidUntilTime(cacheEntry->validUntilTime);

                    // dataMsg->setFinalDestinationNodeName(cacheEntry->finalDestinationNodeName.c_str());
                    dataMsg->setInitialOriginatorAddress(
                            cacheEntry->initialOriginatorAddress.c_str());
                    dataMsg->setDestinationOriented(
                            cacheEntry->destinationOriented);
                    if (cacheEntry->destinationOriented) {
                        dataMsg->setFinalDestinationAddress(
                                cacheEntry->finalDestinationAddress.c_str());
                    }
                    dataMsg->setMessageID(cacheEntry->messageID.c_str());
                    dataMsg->setHopCount(cacheEntry->hopCount);
                    dataMsg->setGoodnessValue(cacheEntry->goodnessValue);
                    dataMsg->setHopsTravelled(cacheEntry->hopsTravelled);
                    dataMsg->setMsgUniqueID(cacheEntry->msgUniqueID);
                    dataMsg->setInitialInjectionTime(
                            cacheEntry->initialInjectionTime);

//                    // Implementation to Check if Node is worth sending message to.
//
                    int neighbourSize =
                            neighListMsg->getNeighbourNameListArraySize();
                    string destinationID =
                            cacheEntry->finalDestinationAddress.c_str();
                    for (int i = 0; i < neighbourSize; i++) {
                        cout << "--------------" << endl;
                        dataMsg->setDestinationAddress(
                                neighListMsg->getNeighbourNameList(i));
                        cout << "Number of Neighbors: " << neighbourSize
                                << endl;
                        cout << "Neighbor to " << ownMACAddress.c_str()
                                << " is "
                                << neighListMsg->getNeighbourNameList(i)
                                << '\n';
                        string neighbourID = neighListMsg->getNeighbourNameList(
                                i);

                        cout << "Destination :" << destinationID << endl;
                        if (isMsgSent(destinationID, neighbourID)) {
                            cout << "Success " << endl;
                            send(dataMsg->dup(), "lowerLayerOut");
                            emit(dataBytesSentSignal,
                                    (long) dataMsg->getByteLength());
                            emit(totalBytesSentSignal,
                                    (long) dataMsg->getByteLength());
                        }

                    }

                    if (!sendOnNeighReportingFrequency) {
                        nextDataSendTime = simTime()
                                + sendFrequencyWhenNotOnNeighFrequency;
                    }
                    // Changes for BubbleRAP End
                }
            }

            delete msg;

            // data message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && (omnetDataMsg =
                dynamic_cast<KDataMsg*>(msg)) != NULL) {

            int found;

            // increment the traveled hop count
            omnetDataMsg->setHopsTravelled(
                    omnetDataMsg->getHopsTravelled() + 1);
            omnetDataMsg->setHopCount(omnetDataMsg->getHopCount() + 1);

            emit(dataBytesReceivedSignal, (long) omnetDataMsg->getByteLength());
            emit(totalBytesReceivedSignal,
                    (long) omnetDataMsg->getByteLength());
            emit(hopsTravelledSignal, (long) omnetDataMsg->getHopsTravelled());
            emit(hopsTravelledCountSignal, 1);

            bool cacheData = TRUE;
            // if (omnetDataMsg->getDestinationOriented()&& strstr(getParentModule()->getFullName(), omnetDataMsg->getFinalDestinationNodeName()) != NULL) {
            if (omnetDataMsg->getDestinationOriented() && strstr(ownMACAddress.c_str(), omnetDataMsg->getFinalDestinationAddress()) != NULL) {
                cacheData = FALSE;
            }

            if (cacheData) {

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
                            && (currentCacheSize
                                    + omnetDataMsg->getRealPacketSize())
                                    > maximumCacheSize
                            && cacheList.size() > 0) {
                        iteratorCache = cacheList.begin();
                        advance(iteratorCache, 0);
                        CacheEntry *removingCacheEntry = *iteratorCache;
                        iteratorCache = cacheList.begin();
                        while (iteratorCache != cacheList.end()) {
                            cacheEntry = *iteratorCache;
                            if (cacheEntry->validUntilTime
                                    < removingCacheEntry->validUntilTime) {
                                removingCacheEntry = cacheEntry;
                            }
                            iteratorCache++;
                        }
                        currentCacheSize -= removingCacheEntry->realPacketSize;
                        cacheList.remove(removingCacheEntry);

                        emit(cacheBytesRemovedSignal,
                                removingCacheEntry->realPacketSize);
                        emit(currentCacheSizeBytesSignal, currentCacheSize);
                        emit(currentCacheSizeReportedCountSignal, (int) 1);

                        emit(currentCacheSizeBytesSignal2, currentCacheSize);

                        delete removingCacheEntry;

                    }

                    cacheEntry = new CacheEntry;

                    cacheEntry->messageID = omnetDataMsg->getMessageID();
                    cacheEntry->dataName = omnetDataMsg->getDataName();
                    cacheEntry->realPayloadSize =
                            omnetDataMsg->getRealPayloadSize();
                    cacheEntry->dummyPayloadContent =
                            omnetDataMsg->getDummyPayloadContent();
                    cacheEntry->msgType = omnetDataMsg->getMsgType();
                    cacheEntry->validUntilTime =
                            omnetDataMsg->getValidUntilTime();
                    cacheEntry->realPacketSize =
                            omnetDataMsg->getRealPacketSize();
                    cacheEntry->initialOriginatorAddress =
                            omnetDataMsg->getInitialOriginatorAddress();
                    cacheEntry->destinationOriented =
                            omnetDataMsg->getDestinationOriented();
                    if (omnetDataMsg->getDestinationOriented()) {
                        cacheEntry->finalDestinationAddress =
                                omnetDataMsg->getFinalDestinationAddress();
                    }
                    cacheEntry->goodnessValue =
                            omnetDataMsg->getGoodnessValue();

                    cacheEntry->msgUniqueID = omnetDataMsg->getMsgUniqueID();
                    cacheEntry->initialInjectionTime =
                            omnetDataMsg->getInitialInjectionTime();

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
            list<AppInfo*>::iterator iteratorRegisteredApps =
                    registeredAppList.begin();
            while (iteratorRegisteredApps != registeredAppList.end()) {
                appInfo = *iteratorRegisteredApps;
                if (strstr(omnetDataMsg->getDataName(),
                        appInfo->prefixName.c_str()) != NULL
                        && ((omnetDataMsg->getDestinationOriented()
                                && strstr(
                                        omnetDataMsg->getFinalDestinationAddress(),
                                        ownMACAddress.c_str()) != NULL)
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
        } else if (strstr(gateName, "lowerLayerIn") != NULL
                && (omnetFeedbackMsg = dynamic_cast<KFeedbackMsg*>(msg)) != NULL) {

            // with RRS, no feedback is considered
            delete msg;

            // received some unexpected packet
        } else {

            EV_INFO << KBUBBLERAPFORWARDING_SIMMODULEINFO
                           << "Received unexpected packet" << "\n";
            delete msg;
        }
    }
}

void KBubbleRAPForwarding::finish() {
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

// Check if Message needs to be sent or not
bool KBubbleRAPForwarding::isMsgSent(string destinationMAC,
        string NeigbourMAC) {
    // Output the MAC addresses of the destination, neighbor, and own device.
//    cout << "Destination:" << destinationMAC << " Neighbor:" << NeigbourMAC
//            << " Own:" << ownMACAddress << endl;

    // Copy the MAC addresses to local variables.
    string MACAddress = destinationMAC;
    string MACAddress2 = NeigbourMAC;
    std::string line, line2;

    // Open the file "outMAC.csv" for reading.
    std::ifstream input("outMAC.csv");

    // Create empty vectors to store the rank values for each device.
    vector<string> destRanks;
    vector<string> neighRanks;
    vector<string> ownRanks;

    for (std::string line; getline(input, line);) {

        // If the line contains the destination MAC address, extract the rank values.
        if (line.find(MACAddress) != std::string::npos) {
            //cout << "Destination MAC:" << line << endl;
            // Parse the line using a stringstream and add each rank value to the destRanks vector.
            stringstream ss(line);
            string destination;
            while (getline(ss, destination, ',')) {
                destRanks.push_back(destination);
            }
        }
        // If the line contains the neighbor MAC address, extract the rank values.
        if (line.find(MACAddress2) != std::string::npos) {
            //cout << "Neighbor MAC   :" << line << endl;
            stringstream ss2(line);
            string neighbour;
            while (getline(ss2, neighbour, ',')) {
                neighRanks.push_back(neighbour);
            }
        }
        // If the line contains the own MAC address, extract the rank values.
        if (line.find(ownMACAddress) != std::string::npos) {
            //cout << "Own MAC        :" << line << endl;
            stringstream ss3(line);
            string own;
            while (getline(ss3, own, ',')) {
                ownRanks.push_back(own);
            }
        }
    }
    // [0] = nodeID
    // [1] = Community
    // [2] = Global Rank
    // [3] = Local Rank
    // [4] = MAC Address
    // Lower the Rank more famous the node.

    // Check if the own and destination devices belong to the same community.
    if (ownRanks[1] == destRanks[1]) {
        // If the neighbor device also belongs to the same community and has a lower local rank, return true.
        if (neighRanks[1] == destRanks[1] && neighRanks[3] < ownRanks[2]) {
            //cout << "Success at Local Rank Level" << endl;
            return true;
        }
    } // If the own and destination devices belong to different communities,
      //check if the neighbor device has a lower global rank.
    else if (destRanks[1] == neighRanks[1] || neighRanks[2] < ownRanks[2]) {
        //cout << destRanks[1] << " == " << neighRanks[1] << "\n" << neighRanks[2]
                //<< "<" << ownRanks[2] << endl;
        // If the neighbor device belongs to the same community, return true.
//        if (destRanks[1] == neighRanks[1])
//            cout << "Success as in Same Community" << endl;
//        // If the neighbor device has a lower global rank, return true.
//        else if (neighRanks[2] < ownRanks[2])
//            cout << "Success at Global Rank Level" << endl;

        return true;

    } else {
        // If the own and destination devices belong to different communities
        // and the neighbor device has a higher global rank, return false.
        //cout << "Not in Same Community" << endl;
        return false;
    }

}

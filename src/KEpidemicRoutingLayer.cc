//
// The model implementation for the Epidemic Routing layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 02-may-2017
//
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

    } else if (stage == 1) {


    } else if (stage == 2) {

        // setup the trigger to age data
        ageDataTimeoutEvent = new cMessage("Age Data Timeout Event");
        ageDataTimeoutEvent->setKind(108);
        scheduleAt(simTime() + 1.0, ageDataTimeoutEvent);

        // setup the trigger to init synced neighbors based on antiEntropyInterval
        syncedNeighboursInitTimeoutEvent = new cMessage("Synced Neighbours Init Timeout Event");
        syncedNeighboursInitTimeoutEvent->setKind(110);
        scheduleAt(simTime() + 2.0, syncedNeighboursInitTimeoutEvent);


    } else {
        EV_FATAL << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << "Something is radically wrong in initialisation \n";
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
    KNeighbourListMsg *neighListMsg;

    // self messages
    if (msg->isSelfMessage()) {

        // age data trigger fired
        if (msg->getKind() == 108) {

            handleDataAgingTrigger(msg);

        // synced neighbour init trigger fired
        } else if (msg->getKind() == 110) {

            handleSyncedNeighbourRemovalTrigger(msg);

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

        // feedback message arrived from the upper layer (app layer)
        } else if (strstr(gateName, "upperLayerIn") != NULL && dynamic_cast<KFeedbackMsg*>(msg) != NULL) {

            // with Epidemic Routing, no feedback is considered

            delete msg;

        // neighbour list message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && dynamic_cast<KNeighbourListMsg*>(msg) != NULL) {

            handleNeighbourListMsgFromLowerLayer(msg);

        // data message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && dynamic_cast<KDataMsg*>(msg) != NULL) {

            handleDataMsgFromLowerLayer(msg);

        // feedback message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && dynamic_cast<KFeedbackMsg*>(msg) != NULL) {

            // with Epidemic Routing, no feedback is considered
            delete msg;

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

void KEpidemicRoutingLayer::handleDataAgingTrigger(cMessage *msg)
{

    // remove expired data items
    int originalSize = cacheList.size();
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
            
            EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << KEPIDEMICROUTINGLAYER_DEBUG << " :: Removing Expired Data Entry :: " 
                << cacheEntry->dataName << " :: Valid Until :: " << cacheEntry->validUntilTime << "\n";
                
                
            currentCacheSize -= cacheEntry->realPacketSize;
            cacheList.remove(cacheEntry);
            delete cacheEntry;

        }
    }
    
    EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << KEPIDEMICROUTINGLAYER_DEBUG << " :: New Cache Size :: " 
        << cacheList.size() << " :: Removed Count :: " << (originalSize - cacheList.size()) << "\n";

    // setup next age data trigger
    scheduleAt(simTime() + 1.0, msg);

}

void KEpidemicRoutingLayer::handleSyncedNeighbourRemovalTrigger(cMessage *msg)
{
    bool syncedNeighbourRemoved = TRUE;

    while (syncedNeighbourRemoved) {
        syncedNeighbourRemoved = FALSE;

        SyncedNeighbour *syncedNeighbour = NULL;
        list<SyncedNeighbour*>::iterator iteratorSyncedNeighbour;
        bool found = FALSE;
        iteratorSyncedNeighbour = syncedNeighbourList.begin();
        while (iteratorSyncedNeighbour != syncedNeighbourList.end()) {
            syncedNeighbour = *iteratorSyncedNeighbour;
            if ((syncedNeighbour->lastSyncTime + antiEntropyInterval) < simTime().dbl()) {
                found = TRUE;
                break;
            }

            iteratorSyncedNeighbour++;
        }

        if (found) {

            // EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << KEPIDEMICROUTINGLAYER_DEBUG << " :: Synced Neighbour Removal :: "
            //     << syncedNeighbour->nodeMACAddress << " :: Last Sync Time :: " << syncedNeighbour->lastSyncTime << "\n";

            syncedNeighbourList.remove(syncedNeighbour);
            delete syncedNeighbour;

            syncedNeighbourRemoved = TRUE;
        }
    }

    // setup next age data trigger
    scheduleAt(simTime() + 1.0, msg);
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

    EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " :: " << ownMACAddress << " :: Upper In :: Data Msg :: " << omnetDataMsg->getSourceAddress() << " :: "
        << omnetDataMsg->getDestinationAddress() << " :: " << omnetDataMsg->getDataName() << " :: " << omnetDataMsg->getGoodnessValue() <<"\n";

    // cout << "---omnetDataMsg->getRealPacketSize() " << omnetDataMsg->getRealPacketSize() << "\n";



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
            delete removingCacheEntry;

            // cout << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " --- removing cache entry, size " << currentCacheSize << "b \n";

        }

        cacheEntry = new CacheEntry;

        cacheEntry->messageID = omnetDataMsg->getDataName();
        cacheEntry->hopCount = maximumHopCount;
        cacheEntry->dataName = omnetDataMsg->getDataName();
        cacheEntry->realPayloadSize = omnetDataMsg->getRealPayloadSize();
        cacheEntry->dummyPayloadContent = omnetDataMsg->getDummyPayloadContent();
        cacheEntry->validUntilTime = omnetDataMsg->getValidUntilTime();
        cacheEntry->realPacketSize = omnetDataMsg->getRealPacketSize();
        cacheEntry->originatorNodeName = omnetDataMsg->getOriginatorNodeName();
        cacheEntry->destinationOriented = omnetDataMsg->getDestinationOriented();
        if (omnetDataMsg->getDestinationOriented()) {
            cacheEntry->finalDestinationNodeName = omnetDataMsg->getFinalDestinationNodeName();
        }
        cacheEntry->createdTime = simTime().dbl();
        cacheEntry->updatedTime = simTime().dbl();

        cacheList.push_back(cacheEntry);

        currentCacheSize += cacheEntry->realPacketSize;

        // cout << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " --- adding cache entry, size " << currentCacheSize << "b \n";
        
        
        // EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << KEPIDEMICROUTINGLAYER_DEBUG << " :: Adding App Generated Cache Entry :: "
        //     << cacheEntry->dataName << "\n";
        
        

    }

    cacheEntry->lastAccessedTime = simTime().dbl();

    delete msg;
}

void KEpidemicRoutingLayer::handleNeighbourListMsgFromLowerLayer(cMessage *msg)
{
    KNeighbourListMsg *neighListMsg = dynamic_cast<KNeighbourListMsg*>(msg);

    EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " :: Got neighbourhood msg :: Neigh Count :: " <<
                        neighListMsg->getNeighbourNameListArraySize() << " :: Cache Size :: "
                            << cacheList.size() << "\n";


    // if no neighbours or cache is empty, just return
    if (neighListMsg->getNeighbourNameListArraySize() == 0 || cacheList.size() == 0) {
        delete msg;
        return;
    }

    // make a summary vector msg that can be replicated to send to each of the neighbors
    KSummaryVectorMsg *baseSummaryVectorMsg = new KSummaryVectorMsg();
    baseSummaryVectorMsg->setSourceAddress(ownMACAddress.c_str());
    baseSummaryVectorMsg->setMessageIDHashVectorArraySize(cacheList.size());
    int i = 0;
    CacheEntry *cacheEntry;
    list<CacheEntry*>::iterator iteratorCache;
    iteratorCache = cacheList.begin();
    while (iteratorCache != cacheList.end()) {
        cacheEntry = *iteratorCache;
        baseSummaryVectorMsg->setMessageIDHashVector(i, cacheEntry->messageID.c_str());

        iteratorCache++;
        i++;
    }
    int realPacketSize = 6 + 6 + (cacheList.size() * KEPIDEMICROUTINGLAYER_MSG_ID_HASH_SIZE);
    baseSummaryVectorMsg->setRealPacketSize(realPacketSize);

    // send summary vector messages to all nodes to sync in a loop
    i = 0;
    while (i < neighListMsg->getNeighbourNameListArraySize()) {
        string nodeMACAddress = neighListMsg->getNeighbourNameList(i);

        // sync done only if my node ID is larger than the neighbors
        // node ID
        if (ownMACAddress > nodeMACAddress) {

            // if the sync was not done with neighbor, start the process by sending
            // a summary vector msg
            if (!syncDoneWithNeighbour(nodeMACAddress)) {

                KSummaryVectorMsg *summaryVectorMsg = baseSummaryVectorMsg->dup();
                summaryVectorMsg->setDestinationAddress(nodeMACAddress.c_str());
                send(summaryVectorMsg, "lowerLayerOut");

                updateNeighbourSyncStarted(nodeMACAddress);

                EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " :: " << ownMACAddress << " :: Lower Out :: Summary Vector Msg :: "
                    << summaryVectorMsg->getSourceAddress() << " :: " << summaryVectorMsg->getDestinationAddress()
                        << " :: Cached Entries " << summaryVectorMsg->getMessageIDHashVectorArraySize() << "\n";
                
                
                // EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << KEPIDEMICROUTINGLAYER_DEBUG << " :: Starting Neighbour Sync :: "
                //     << nodeMACAddress << " :: Entries in Cache to Sync :: " << cacheList.size() << "\n";
                                

            }
        }
        i++;
    }

    // remove the original summary vector msg (from which duplicates were made)
    delete baseSummaryVectorMsg;

    // delete the received neighbor list msg
    delete msg;
}

void KEpidemicRoutingLayer::handleDataMsgFromLowerLayer(cMessage *msg)
{
    KDataMsg *omnetDataMsg = dynamic_cast<KDataMsg*>(msg);

    bool found;

    EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " :: " << ownMACAddress << " :: Lower In :: Data Msg :: " << omnetDataMsg->getSourceAddress() << " :: "
        << omnetDataMsg->getDestinationAddress() << " :: " << omnetDataMsg->getDataName() << " :: " << omnetDataMsg->getGoodnessValue() << "\n";

    // if destination oriented data sent around, then cache message only if not destined to self
    // or if no destination in data, cache all messages
    bool cacheData;
    if (!omnetDataMsg->getDestinationOriented()) {
        cacheData = TRUE;
    } else if (omnetDataMsg->getDestinationOriented() && strstr(getParentModule()->getFullName(), omnetDataMsg->getFinalDestinationNodeName()) != NULL) {
        cacheData = FALSE;
    } else {
        cacheData = TRUE;
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
                delete removingCacheEntry;

                // cout << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " --- removing cache entry, size " << currentCacheSize << "b \n";

            }

            cacheEntry = new CacheEntry;

            cacheEntry->messageID = omnetDataMsg->getMessageID();
            cacheEntry->hopCount = omnetDataMsg->getHopCount();
            cacheEntry->dataName = omnetDataMsg->getDataName();
            cacheEntry->realPayloadSize = omnetDataMsg->getRealPayloadSize();
            cacheEntry->dummyPayloadContent = omnetDataMsg->getDummyPayloadContent();
            cacheEntry->validUntilTime = omnetDataMsg->getValidUntilTime();
            cacheEntry->realPacketSize = omnetDataMsg->getRealPacketSize();
            cacheEntry->originatorNodeName = omnetDataMsg->getOriginatorNodeName();
            cacheEntry->destinationOriented = omnetDataMsg->getDestinationOriented();
            if (omnetDataMsg->getDestinationOriented()) {
                cacheEntry->finalDestinationNodeName = omnetDataMsg->getFinalDestinationNodeName();
            }
            cacheEntry->createdTime = simTime().dbl();
            cacheEntry->updatedTime = simTime().dbl();

            cacheList.push_back(cacheEntry);

            currentCacheSize += cacheEntry->realPacketSize;

            // cout << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " --- adding cache entry, size " << currentCacheSize << "b \n";
            // cout << "omnetDataMsg->getRealPacketSize()" << omnetDataMsg->getRealPacketSize() << "\n";


            // EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << KEPIDEMICROUTINGLAYER_DEBUG << " :: Caching Received Data :: "
            //     << cacheEntry->dataName << " :: From :: " << omnetDataMsg->getSourceAddress() << "\n";

        } else {
            // EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << KEPIDEMICROUTINGLAYER_DEBUG << " :: Already Cached Data Received :: "
            //     << cacheEntry->dataName << " :: From :: " << omnetDataMsg->getSourceAddress() << "\n";
            
        }

        cacheEntry->lastAccessedTime = simTime().dbl();
    }

    // if registered app exist, send data msg to app
    AppInfo *appInfo = NULL;
    found = FALSE;
    list<AppInfo*>::iterator iteratorRegisteredApps = registeredAppList.begin();
    while (iteratorRegisteredApps != registeredAppList.end()) {
        appInfo = *iteratorRegisteredApps;
        if (strstr(omnetDataMsg->getDataName(), appInfo->prefixName.c_str()) != NULL) {
            found = TRUE;
            break;
        }
        iteratorRegisteredApps++;
    }
    if (found) {
        send(msg, "upperLayerOut");

        EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " :: " << ownMACAddress << " :: Upper Out :: Data Msg :: " << omnetDataMsg->getSourceAddress() << " :: "
            << omnetDataMsg->getDestinationAddress() << " :: " << omnetDataMsg->getDataName() << " :: " << omnetDataMsg->getGoodnessValue() << "\n";

    } else {
        delete msg;
    }
}

void KEpidemicRoutingLayer::handleSummaryVectorMsgFromLowerLayer(cMessage *msg)
{
    KSummaryVectorMsg *summaryVectorMsg = dynamic_cast<KSummaryVectorMsg*>(msg);

    EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " :: " << ownMACAddress << " :: Lower In :: Summary Vector Msg :: " << summaryVectorMsg->getSourceAddress() << " :: "
        << summaryVectorMsg->getDestinationAddress() << "\n";
    
    
    // EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << KEPIDEMICROUTINGLAYER_DEBUG << " :: Summary Vector Received From :: "
    //     << summaryVectorMsg->getSourceAddress() << " :: Summary Vector Data Count :: " << summaryVectorMsg->getMessageIDHashVectorArraySize()
    //         << "\n";

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
    int realPacketSize = 6 + 6 + (selectedMessageIDList.size() * KEPIDEMICROUTINGLAYER_MSG_ID_HASH_SIZE);
    dataRequestMsg->setRealPacketSize(realPacketSize);
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

    // EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << KEPIDEMICROUTINGLAYER_DEBUG << " :: Sending Request To :: "
    //     << summaryVectorMsg->getSourceAddress() << " :: Requested Data Count :: " << selectedMessageIDList.size() << "\n";

    delete msg;
}

void KEpidemicRoutingLayer::handleDataRequestMsgFromLowerLayer(cMessage *msg)
{
    KDataRequestMsg *dataRequestMsg = dynamic_cast<KDataRequestMsg*>(msg);

    EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " :: " << ownMACAddress << " :: Lower In :: Summary Vector Msg :: " << dataRequestMsg->getSourceAddress() << " :: "
        << dataRequestMsg->getDestinationAddress() << "\n";


    // EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << KEPIDEMICROUTINGLAYER_DEBUG << " :: Data Request Received From :: "
    //     << dataRequestMsg->getSourceAddress() << " :: Request Count:: "
    //         << dataRequestMsg->getMessageIDHashVectorArraySize() << "\n";

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
            dataMsg->setOriginatorNodeName(cacheEntry->originatorNodeName.c_str());
            dataMsg->setDestinationOriented(cacheEntry->destinationOriented);
            if (cacheEntry->destinationOriented) {
                dataMsg->setFinalDestinationNodeName(cacheEntry->finalDestinationNodeName.c_str());
            }
            dataMsg->setMessageID(cacheEntry->messageID.c_str());
            dataMsg->setHopCount(cacheEntry->hopCount);

            send(dataMsg, "lowerLayerOut");

            // EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << KEPIDEMICROUTINGLAYER_DEBUG << " :: Sending Requested Data :: "
            //     << cacheEntry->dataName << " :: To :: " << dataRequestMsg->getSourceAddress() << "\n";

        }

        i++;
    }

    delete msg;
}

bool KEpidemicRoutingLayer::syncDoneWithNeighbour(string nodeMACAddress)
{
    // check if node (neighbor) was seen before (and therefore sync done)
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

    if (found) {
        return TRUE;
    }

    return FALSE;
}

void KEpidemicRoutingLayer::updateNeighbourSyncStarted(string nodeMACAddress)
{
    if (syncDoneWithNeighbour(nodeMACAddress)) {
        return;
    }

    SyncedNeighbour *syncedNeighbour = new SyncedNeighbour;

    syncedNeighbour->nodeMACAddress = nodeMACAddress.c_str();
    syncedNeighbour->lastSyncTime = simTime().dbl();

    syncedNeighbourList.push_back(syncedNeighbour);
}

void KEpidemicRoutingLayer::finish()
{
    // cout << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << "Cache size " << currentCacheSize << ", Cache entries " << cacheList.size() << "\n";

    // remove age data trigger
    cancelEvent(ageDataTimeoutEvent);
    delete ageDataTimeoutEvent;

    // remove age data trigger
    cancelEvent(syncedNeighboursInitTimeoutEvent);
    delete syncedNeighboursInitTimeoutEvent;

}


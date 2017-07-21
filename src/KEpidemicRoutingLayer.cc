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
                
            currentCacheSize -= cacheEntry->realPacketSize;
            cacheList.remove(cacheEntry);
            delete cacheEntry;

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

        }

        cacheEntry = new CacheEntry;

        cacheEntry->messageID = omnetDataMsg->getDataName();
        cacheEntry->hopCount = 0;
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
        cacheEntry->goodnessValue = omnetDataMsg->getGoodnessValue();
        cacheEntry->createdTime = simTime().dbl();
        cacheEntry->updatedTime = simTime().dbl();

        cacheList.push_back(cacheEntry);

        currentCacheSize += cacheEntry->realPacketSize;

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
    baseSummaryVectorMsg->setByteLength(realPacketSize);

    // send summary vector messages to all nodes to sync in a loop
    i = 0;
    while (i < neighListMsg->getNeighbourNameListArraySize()) {
        string nodeMACAddress = neighListMsg->getNeighbourNameList(i);

        // // sync initiation done only if my node ID is larger than the neighbors node ID
        // // note: opposit way is done when this sync is completed
        // if (ownMACAddress < nodeMACAddress) {
        //     i++;
        //     continue;
        // }

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

    // if destination oriented data sent around and this node is the destination
    // or if maximum hop count is reached
    // then cache or else don't cache
    bool cacheData = TRUE;
    if ((omnetDataMsg->getDestinationOriented() 
         && strstr(getParentModule()->getFullName(), omnetDataMsg->getFinalDestinationNodeName()) != NULL)
        || omnetDataMsg->getHopCount() >= maximumHopCount) {

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
                delete removingCacheEntry;

            }

            cacheEntry = new CacheEntry;

            cacheEntry->messageID = omnetDataMsg->getMessageID();
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
            cacheEntry->goodnessValue = omnetDataMsg->getGoodnessValue();
            cacheEntry->createdTime = simTime().dbl();
            cacheEntry->updatedTime = simTime().dbl();

            cacheList.push_back(cacheEntry);

            currentCacheSize += cacheEntry->realPacketSize;

        } else {

        }

        cacheEntry->hopCount = omnetDataMsg->getHopCount() + 1;
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

    EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " :: " << ownMACAddress << " :: Lower Out :: Data Request Msg :: " << dataRequestMsg->getSourceAddress() << " :: "
        << dataRequestMsg->getDestinationAddress() << "\n";

    delete msg;
}

void KEpidemicRoutingLayer::handleDataRequestMsgFromLowerLayer(cMessage *msg)
{
    KDataRequestMsg *dataRequestMsg = dynamic_cast<KDataRequestMsg*>(msg);

    EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " :: " << ownMACAddress << " :: Lower In :: Data Request Msg :: " << dataRequestMsg->getSourceAddress() << " :: "
        << dataRequestMsg->getDestinationAddress() << "\n";

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
            dataMsg->setOriginatorNodeName(cacheEntry->originatorNodeName.c_str());
            dataMsg->setDestinationOriented(cacheEntry->destinationOriented);
            if (cacheEntry->destinationOriented) {
                dataMsg->setFinalDestinationNodeName(cacheEntry->finalDestinationNodeName.c_str());
            }
            dataMsg->setMessageID(cacheEntry->messageID.c_str());
            dataMsg->setHopCount(cacheEntry->hopCount);
            dataMsg->setGoodnessValue(cacheEntry->goodnessValue);

            send(dataMsg, "lowerLayerOut");

            EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << " :: " << ownMACAddress << " :: Lower Out :: Data Msg :: " << dataMsg->getSourceAddress() << " :: "
                << dataMsg->getDestinationAddress() << "\n";


            // EV_INFO << KEPIDEMICROUTINGLAYER_SIMMODULEINFO << KEPIDEMICROUTINGLAYER_DEBUG << " :: Sending Requested Data :: "
            //     << cacheEntry->dataName << " :: To :: " << dataRequestMsg->getSourceAddress() << "\n";

        }

        i++;
    }

    delete msg;
}

bool KEpidemicRoutingLayer::syncDoneWithNeighbour(string nodeMACAddress)
{
    // check if node (neighbor) was seen before and sync done
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

    if (found && (syncedNeighbour->lastSyncTime + antiEntropyInterval) > simTime().dbl()) {
        return TRUE;
    }

    return FALSE;
}

void KEpidemicRoutingLayer::updateNeighbourSyncStarted(string nodeMACAddress)
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
        
        // if sync entry not there, create an entry
        syncedNeighbour = new SyncedNeighbour;

        syncedNeighbour->nodeMACAddress = nodeMACAddress.c_str();
        syncedNeighbourList.push_back(syncedNeighbour);
    }
    
    // update sync started time
    syncedNeighbour->lastSyncTime = simTime().dbl();

}

void KEpidemicRoutingLayer::finish()
{

    // remove age data trigger
    cancelEvent(ageDataTimeoutEvent);
    delete ageDataTimeoutEvent;

}


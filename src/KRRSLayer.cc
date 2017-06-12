//
// The model implementation for the Randomized Rumor Spreading (RRS) layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 25-aug-2015
//
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
        currentCacheSize = 0;
        destinationOrientedData = par("destinationOrientedData");

        // set other parameters
        broadcastMACAddress = "FF:FF:FF:FF:FF:FF";
        
    } else if (stage == 1) {

		
    } else if (stage == 2) {

		// setup the trigger to age data
        ageDataTimeoutEvent = new cMessage("Age Data Timeout Event");
        ageDataTimeoutEvent->setKind(108);
        scheduleAt(simTime() + 1.0, ageDataTimeoutEvent);

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
		if (msg->getKind() == 108) {

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
					delete cacheEntry;
				}
			}

        	// setup next age data trigger
        	scheduleAt(simTime() + 1.0, msg);
			
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

			EV_INFO << KRRSLAYER_SIMMODULEINFO << " :: " << ownMACAddress << " :: Upper In :: Data Msg :: " << omnetDataMsg->getSourceAddress() << " :: " 
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
                    
                    // cout << KRRSLAYER_SIMMODULEINFO << " --- removing cache entry, size " << currentCacheSize << "b \n";
                    
                }

				cacheEntry = new CacheEntry;
			    cacheEntry->dataName = omnetDataMsg->getDataName();
			    cacheEntry->goodnessValue = omnetDataMsg->getGoodnessValue();
			    cacheEntry->realPayloadSize = omnetDataMsg->getRealPayloadSize();
			    cacheEntry->dummyPayloadContent = omnetDataMsg->getDummyPayloadContent();
				cacheEntry->msgType = omnetDataMsg->getMsgType();
				cacheEntry->validUntilTime = omnetDataMsg->getValidUntilTime();
    
			    cacheEntry->realPacketSize = omnetDataMsg->getRealPacketSize();
			
                cacheEntry->finalDestinationNodeName = omnetDataMsg->getFinalDestinationNodeName();
                
				cacheEntry->createdTime = simTime().dbl();
				cacheEntry->updatedTime = simTime().dbl();
				
				cacheList.push_back(cacheEntry);
                
                currentCacheSize += cacheEntry->realPacketSize;
                
                // cout << KRRSLAYER_SIMMODULEINFO << " --- adding cache entry, size " << currentCacheSize << "b \n";
                
			}
			
			cacheEntry->lastAccessedTime = simTime().dbl();
			
			delete msg;

    	// feedback message arrived from the upper layer (app layer)
    	} else if (strstr(gateName, "upperLayerIn") != NULL && (omnetFeedbackMsg = dynamic_cast<KFeedbackMsg*>(msg)) != NULL) {

			// with RRS, no feedback is considered

			delete msg;
			
    	// neighbour list message arrived from the lower layer (link layer)
    	} else if (strstr(gateName, "lowerLayerIn") != NULL && (neighListMsg = dynamic_cast<KNeighbourListMsg*>(msg)) != NULL) {

		    EV_INFO << KRRSLAYER_SIMMODULEINFO << " :: Got neighbourhood msg :: Neigh Count :: " << 
								neighListMsg->getNeighbourNameListArraySize() << " :: Cache Size :: " 
									<< cacheList.size() << "\n";


			if (neighListMsg->getNeighbourNameListArraySize() > 0 && cacheList.size() > 0) {
				
				// if there are nodes in the neighbourhood and entries in the cache
				// send a randomly selected cached data out
				
				int cacheIndex = 0;
				if (cacheList.size() > 1) {
					cacheIndex = intuniform(0, (cacheList.size() - 1));
				}

				// EV_INFO << KRRSLAYER_SIMMODULEINFO << " :: Cache Size :: " << cacheList.size()
				// 	<< " :: Index Chosen :: " << cacheIndex << " \n";
				//
				
			    list<CacheEntry*>::iterator iteratorCache = cacheList.begin();
				advance(iteratorCache, cacheIndex);
				CacheEntry *cacheEntry = *iteratorCache;
				
		        KDataMsg *dataMsg = new KDataMsg();

		        dataMsg->setSourceAddress(ownMACAddress.c_str());
		        dataMsg->setDestinationAddress(broadcastMACAddress.c_str());
		        dataMsg->setDataName(cacheEntry->dataName.c_str());
		        dataMsg->setGoodnessValue(cacheEntry->goodnessValue);
		        dataMsg->setRealPayloadSize(cacheEntry->realPayloadSize);
		        dataMsg->setDummyPayloadContent(cacheEntry->dummyPayloadContent.c_str());
		        dataMsg->setRealPacketSize(cacheEntry->realPacketSize);
		        dataMsg->setByteLength(cacheEntry->realPacketSize);
				dataMsg->setMsgType(cacheEntry->msgType);
				dataMsg->setValidUntilTime(cacheEntry->validUntilTime);
                dataMsg->setFinalDestinationNodeName(cacheEntry->finalDestinationNodeName.c_str());

		        send(dataMsg, "lowerLayerOut");

				EV_INFO << KRRSLAYER_SIMMODULEINFO << " :: " << ownMACAddress << " :: Lower Out :: Data Msg :: " << dataMsg->getSourceAddress() << " :: " 
					<< dataMsg->getDestinationAddress() << " :: " << dataMsg->getDataName() << " :: " << dataMsg->getGoodnessValue() << "\n";
			}

			delete msg;
			
    	// data message arrived from the lower layer (link layer)
    	} else if (strstr(gateName, "lowerLayerIn") != NULL && (omnetDataMsg = dynamic_cast<KDataMsg*>(msg)) != NULL) {

            int found;
            
			EV_INFO << KRRSLAYER_SIMMODULEINFO << " :: " << ownMACAddress << " :: Lower In :: Data Msg :: " << omnetDataMsg->getSourceAddress() << " :: " 
				<< omnetDataMsg->getDestinationAddress() << " :: " << omnetDataMsg->getDataName() << " :: " << omnetDataMsg->getGoodnessValue() << "\n";

            // if destination oriented data sent around, then cache message only if not destined to self
            // or if no destination in data, cache all messages
            if(!destinationOrientedData || (strstr(getParentModule()->getFullName(), omnetDataMsg->getFinalDestinationNodeName()) == NULL)) {

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
                    
                        // cout << KRRSLAYER_SIMMODULEINFO << " --- removing cache entry, size " << currentCacheSize << "b \n";
                    
                    }

                    cacheEntry = new CacheEntry;
                    cacheEntry->dataName = omnetDataMsg->getDataName();
                    cacheEntry->goodnessValue = omnetDataMsg->getGoodnessValue();
                    cacheEntry->realPayloadSize = omnetDataMsg->getRealPayloadSize();
                    cacheEntry->dummyPayloadContent = omnetDataMsg->getDummyPayloadContent();
                    cacheEntry->msgType = omnetDataMsg->getMsgType();
                    cacheEntry->validUntilTime = omnetDataMsg->getValidUntilTime();
    
                    cacheEntry->realPacketSize = omnetDataMsg->getRealPacketSize();

                    cacheEntry->finalDestinationNodeName = omnetDataMsg->getFinalDestinationNodeName();
			
                    cacheEntry->createdTime = simTime().dbl();
                    cacheEntry->updatedTime = simTime().dbl();

                    cacheList.push_back(cacheEntry);
                
                    currentCacheSize += cacheEntry->realPacketSize;
                
                    // cout << KRRSLAYER_SIMMODULEINFO << " --- adding cache entry, size " << currentCacheSize << "b \n";
                    // cout << "omnetDataMsg->getRealPacketSize()" << omnetDataMsg->getRealPacketSize() << "\n";
                
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
				
				EV_INFO << KRRSLAYER_SIMMODULEINFO << " :: " << ownMACAddress << " :: Upper Out :: Data Msg :: " << omnetDataMsg->getSourceAddress() << " :: " 
					<< omnetDataMsg->getDestinationAddress() << " :: " << omnetDataMsg->getDataName() << " :: " << omnetDataMsg->getGoodnessValue() << "\n";
				
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
    // cout << KRRSLAYER_SIMMODULEINFO << "Cache size " << currentCacheSize << ", Cache entries " << cacheList.size() << "\n";
    
	// remove age data trigger
	cancelEvent(ageDataTimeoutEvent);
	delete ageDataTimeoutEvent;

}

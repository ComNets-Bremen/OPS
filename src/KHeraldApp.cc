//
// The model implementation for the Herald application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de), Anna Förster (afoerster@uni-bremen.de)
// @date   : 15-aug-2016, updated 6-febr-2018
//
//

#include "KHeraldApp.h"

#include "KBaseNodeInfo.h"

Define_Module(KHeraldApp);

vector<int> popularityList;
vector<KBaseNodeInfo*> heraldNodeInfoList;


void KHeraldApp::initialize(int stage)
{
    if (stage == 0) {

        // get parameters
        nodeIndex = par("nodeIndex");
        ownMACAddress = par("ownMACAddress").stringValue();
        usedRNG = par("usedRNG");
        dataGenerationInterval = par("dataGenerationInterval");
        popularityAssignmentPercentage = par("popularityAssignmentPercentage");
        dataSizeInBytes = par("dataSizeInBytes");
        ttl = par("ttl");
        totalSimulationTime = SimTime::parse(getEnvir()->getConfig()->getConfigValue("sim-time-limit")).dbl();
		notificationCount = totalSimulationTime/dataGenerationInterval;
		totalNumNodes = getParentModule()->getParentModule()->par("numNodes");
		
        // add own address to global list to use for random destination selections
        ownNodeInfo = new KBaseNodeInfo();
        ownNodeInfo->nodeAddress = ownMACAddress;
        ownNodeInfo->nodeHeraldAppModule = this;
        heraldNodeInfoList.push_back(ownNodeInfo);

        // setup prefix
        strcpy(prefixName, "/herald");

        // The following procedure is followed when assigning goodness values to each notification
        // - introduce popularity of messages. The “pop” value represents the percentage of the complete
        //   network which will love this packet (e.g. joke).
        // - at initialisation, create pop-values for all messages. 90% of the messages get a pop-value of
        //   0, 10% of the messages get a random pop-value between 1 and 20.
        // - after creating all the messages with their pop-values, allow each user to decide whether she
        //   likes the message or not. There are only two decisions possible: IGNORE (corresponds to a value
        //   of 0) and LOVE (correponds to a value of 100).
        // - the decision itself is taken according to Fig. 3b and Equation (2) of the UBM paper. The
        //   resulting value is compared to 90: if the value is greater or equal to 90, LOVE (100). If not, IGNORE (0).
        // - The final “like” values (only 0 and 100s possible) are stored and used later at simulation time
        //   to represent the reaction of the user and to send to Keetchi as goodness values.


        // assign popularity values for the popularityAssignmentPercentage of notificationCount
        if (popularityList.size() == 0) {
            for (int i = 0; i < notificationCount; i++) {
                double perc = uniform(0.0, 100.0, usedRNG);
                int popularity = 0;

                if (perc <= popularityAssignmentPercentage) {
                    popularity = 100;
                }

                popularityList.push_back(popularity);
            }
            
        }
		
        // setup the event notification array
        for (int i = 0; i < notificationCount; i++)
        {
        	int like = 0;
        	if (popularityList[i] > 90)
        		like = 100;
        	myLikenesses.push_back(like);
        	timesMessagesReceived.push_back(0);
        	
        	int goodness = like;
        }

    } else if (stage == 1) {


    } else if (stage == 2) {

        // create and setup app registration trigger
        appRegistrationEvent = new cMessage("App Registration Event");
        appRegistrationEvent->setKind(KHERALDAPP_REGISTRATION_EVENT);
        scheduleAt(simTime(), appRegistrationEvent);

        // this is a round-robin scheduling of traffic: in a row, everybody gets a chance to send the next packet.
    	dataTimeoutEvent = new cMessage("Data Timeout Event");
        dataTimeoutEvent->setKind(KHERALDAPP_DATASEND_EVENT);
        // add 0.1 secs to the first sending to avoid the simulation to send one more message than expected.
        scheduleAt(simTime() + (dataGenerationInterval * nodeIndex) + 0.1, dataTimeoutEvent);
        nextGenerationNotification = nodeIndex;

        // registering statistic signals
        likedDataBytesReceivedSignal = registerSignal("appLikedDataBytesReceived");
        nonLikedDataBytesReceivedSignal = registerSignal("appNonLikedDataBytesReceived");
        duplicateDataBytesReceivedSignal = registerSignal("appDuplicateDataBytesReceived");
        totalDataBytesReceivedSignal = registerSignal("appTotalDataBytesReceived");

        likedDataBytesMaxReceivableSignal = registerSignal("appLikedDataBytesMaxReceivable");
        nonLikedDataBytesMaxReceivableSignal = registerSignal("appNonLikedDataBytesMaxReceivable");
        totalDataBytesMaxReceivableSignal = registerSignal("appTotalDataBytesMaxReceivable");

        likedDataCountReceivedSignal = registerSignal("appLikedDataCountReceived");
        nonLikedDataCountReceivedSignal = registerSignal("appNonLikedDataCountReceived");
        duplicateDataCountReceivedSignal = registerSignal("appDuplicateDataCountReceived");
        totalDataCountReceivedSignal = registerSignal("appTotalDataCountReceived");

        likedDataCountMaxReceivableSignal = registerSignal("appLikedDataCountMaxReceivable");
        nonLikedDataCountMaxReceivableSignal = registerSignal("appNonLikedDataCountMaxReceivable");
        totalDataCountMaxReceivableSignal = registerSignal("appTotalDataCountMaxReceivable");

        likedDataReceivedDelaySignal = registerSignal("appLikedDataReceivedDelay");
        nonLikedDataReceivedDelaySignal = registerSignal("appNonLikedDataReceivedDelay");
        totalDataReceivedDelaySignal = registerSignal("appTotalDataReceivedDelay");

        likedDataCountReceivedForAvgDelayCompSignal = registerSignal("appLikedDataCountReceivedForAvgDelayComp");
        nonLikedDataCountReceivedForAvgDelayCompSignal = registerSignal("appNonLikedDataCountReceivedForAvgDelayComp");
        totalDataCountReceivedForAvgDelayCompSignal = registerSignal("appTotalDataCountReceivedForAvgDelayComp");

        likedDataCountReceivedForRatioCompSignal = registerSignal("appLikedDataCountReceivedForRatioComp");
        nonLikedDataCountReceivedForRatioCompSignal = registerSignal("appNonLikedDataCountReceivedForRatioComp");
        totalDataCountReceivedForRatioCompSignal = registerSignal("appTotalDataCountReceivedForRatioComp");

        likedDataCountMaxReceivableForRatioCompSignal = registerSignal("appLikedDataCountMaxReceivableForRatioComp");
        nonLikedDataCountMaxReceivableForRatioCompSignal = registerSignal("appNonLikedDataCountMaxReceivableForRatioComp");
        totalDataCountMaxReceivableForRatioCompSignal = registerSignal("appTotalDataCountMaxReceivableForRatioComp");


        likedDataHopsForAvgHopsCompSignal = registerSignal("appLikedDataHopsForAvgHopsComp");
        nonLikedDataHopsForAvgHopsCompSignal = registerSignal("appNonLikedDataHopsForAvgHopsComp");
        totalDataHopsForAvgHopsCompSignal = registerSignal("appTotalDataHopsForAvgHopsComp");

        likedDataHopsCountForAvgHopsCompSignal = registerSignal("appLikedDataHopsCountForAvgHopsComp");
        nonLikedDataHopsCountForAvgHopsCompSignal = registerSignal("appNonLikedDataHopsCountForAvgHopsComp");
        totalDataHopsCountForAvgHopsCompSignal = registerSignal("appTotalDataHopsCountForAvgHopsComp");

        likedDataReceivedDelaySignal2 = registerSignal("appLikedDataReceivedDelay2");
        nonLikedDataReceivedDelaySignal2 = registerSignal("appNonLikedDataReceivedDelay2");
        totalDataReceivedDelaySignal2 = registerSignal("appTotalDataReceivedDelay2");

        totalDataBytesReceivedSignal2 = registerSignal("appTotalDataBytesReceived2");
        totalDataCountReceivedSignal2 = registerSignal("appTotalDataCountReceived2");

    } else {
        EV_FATAL << KHERALDAPP_SIMMODULEINFO << "Something is radically wrong\n";

    }

}

int KHeraldApp::numInitStages() const
{
    return 3;
}

void KHeraldApp::handleMessage(cMessage *msg)
{
    // check message
    if (msg->isSelfMessage() && msg->getKind() == KHERALDAPP_REGISTRATION_EVENT) {
        // send app registration message the forwarding layer
        KRegisterAppMsg *regAppMsg = new KRegisterAppMsg("Herald App Registration");
        regAppMsg->setAppName("Herald");
        regAppMsg->setPrefixName(prefixName);

        send(regAppMsg, "lowerLayerOut");

    } else if (msg->isSelfMessage() && msg->getKind() == KHERALDAPP_DATASEND_EVENT) {

		// mark this message as received by this node
		timesMessagesReceived[nextGenerationNotification]++;
		
		//setup the data message for sending down to forwarding layer
		char tempString[128];
		sprintf(tempString, "D item-%0d", KHERALDAPP_START_ITEM_ID + nextGenerationNotification);
		
		KDataMsg *dataMsg = new KDataMsg(tempString);

		dataMsg->setSourceAddress("");
		dataMsg->setDestinationAddress("");
		
		sprintf(tempString, "/herald/item-%0d", KHERALDAPP_START_ITEM_ID + nextGenerationNotification);
		
		dataMsg->setDataName(tempString);
		dataMsg->setGoodnessValue(myLikenesses[nextGenerationNotification]);
		dataMsg->setRealPayloadSize(dataSizeInBytes);
		dataMsg->setMsgUniqueID(nextGenerationNotification);
        dataMsg->setInitialInjectionTime(simTime());
		
		sprintf(tempString, "Details of item-%0d", KHERALDAPP_START_ITEM_ID + nextGenerationNotification);
		dataMsg->setDummyPayloadContent(tempString);
		
		dataMsg->setByteLength(dataSizeInBytes);
		dataMsg->setMsgType(0);
		dataMsg->setValidUntilTime(ttl);

        dataMsg->setInitialOriginatorAddress(ownMACAddress.c_str());
        dataMsg->setDestinationOriented(false);
        dataMsg->setFinalDestinationAddress("");

		send(dataMsg, "lowerLayerOut");
		
		// schedule again after a complete round robin of all nodes
		nextGenerationNotification += totalNumNodes;	
	    scheduleAt(simTime() + (dataGenerationInterval * totalNumNodes), msg);

	    // generate receivable stat update to other nodes
	    bool likedData = false;
	    if (dataMsg->getGoodnessValue() >= 100) {
	        likedData = true;
	    }
	    for (int i = 0; i < heraldNodeInfoList.size(); i++) {
	        if (heraldNodeInfoList[i] == ownNodeInfo) {
	            continue;
	        }

	        KStatisticsMsg *statMsg = new KStatisticsMsg("StatMsg");
	        statMsg->setLikedData(likedData);
	        statMsg->setDataCountReceivable(1);
	        statMsg->setDataBytesReceivable(dataSizeInBytes);
	        sendDirect(statMsg, heraldNodeInfoList[i]->nodeHeraldAppModule, "statIn");
	    }

    } else if (dynamic_cast<KDataMsg*>(msg) != NULL) {

        // message received from outside so, process received data message
        KDataMsg *dataMsg = check_and_cast<KDataMsg *>(msg);

        // increment number of times seen
        int i = dataMsg->getMsgUniqueID();
        timesMessagesReceived[i]++;
        
        // compute stats
         if (timesMessagesReceived[i] > 1) {
            emit(duplicateDataCountReceivedSignal, (int) 1);
            emit(totalDataCountReceivedSignal, (int) 1);
            emit(duplicateDataBytesReceivedSignal, dataMsg->getRealPayloadSize());
            emit(totalDataBytesReceivedSignal, dataMsg->getRealPayloadSize());

        } else {

            simtime_t diff = simTime() - dataMsg->getInitialInjectionTime();
            if (myLikenesses[i] >= 100) {
                emit(likedDataCountReceivedSignal, (int) 1);
                emit(likedDataBytesReceivedSignal, dataMsg->getRealPayloadSize());
                emit(likedDataReceivedDelaySignal, diff);
                emit(likedDataCountReceivedForAvgDelayCompSignal, (int) 1);
                emit(likedDataCountReceivedForRatioCompSignal, (int) 1);

                emit(likedDataHopsForAvgHopsCompSignal, dataMsg->getHopsTravelled());
                emit(likedDataHopsCountForAvgHopsCompSignal, (int) 1);

                emit(likedDataReceivedDelaySignal2, diff);

            } else {
                emit(nonLikedDataCountReceivedSignal, (int) 1);
                emit(nonLikedDataBytesReceivedSignal, dataMsg->getRealPayloadSize());
                emit(nonLikedDataReceivedDelaySignal, diff);
                emit(nonLikedDataCountReceivedForAvgDelayCompSignal, (int) 1);
                emit(nonLikedDataCountReceivedForRatioCompSignal, (int) 1);

                emit(nonLikedDataHopsForAvgHopsCompSignal, dataMsg->getHopsTravelled());
                emit(nonLikedDataHopsCountForAvgHopsCompSignal, (int) 1);

                emit(nonLikedDataReceivedDelaySignal2, diff);

            }
            emit(totalDataCountReceivedSignal, (int) 1);
            emit(totalDataBytesReceivedSignal, dataMsg->getRealPayloadSize());
            emit(totalDataReceivedDelaySignal, diff);
            emit(totalDataCountReceivedForAvgDelayCompSignal, (int) 1);
            emit(totalDataCountReceivedForRatioCompSignal, (int) 1);

            emit(totalDataHopsForAvgHopsCompSignal, dataMsg->getHopsTravelled());
            emit(totalDataHopsCountForAvgHopsCompSignal, (int) 1);

            emit(totalDataReceivedDelaySignal2, diff);

            emit(totalDataCountReceivedSignal2, (int) 1);
            emit(totalDataBytesReceivedSignal2, dataMsg->getRealPayloadSize());


        }

        delete msg;

    } else if (dynamic_cast<KStatisticsMsg*>(msg) != NULL) {
        // message received from outside so, process received statistics message
        KStatisticsMsg *statMsg = check_and_cast<KStatisticsMsg *>(msg);

        // update receivables
        if (statMsg->getLikedData()) {
            emit(likedDataCountMaxReceivableSignal, statMsg->getDataCountReceivable());
            emit(likedDataBytesMaxReceivableSignal, statMsg->getDataBytesReceivable());
            emit(likedDataCountMaxReceivableForRatioCompSignal, statMsg->getDataCountReceivable());
        } else {
            emit(nonLikedDataCountMaxReceivableSignal, statMsg->getDataCountReceivable());
            emit(nonLikedDataBytesMaxReceivableSignal, statMsg->getDataBytesReceivable());
            emit(nonLikedDataCountMaxReceivableForRatioCompSignal, statMsg->getDataCountReceivable());
        }
        emit(totalDataCountMaxReceivableSignal, statMsg->getDataCountReceivable());
        emit(totalDataBytesMaxReceivableSignal, statMsg->getDataBytesReceivable());
        emit(totalDataCountMaxReceivableForRatioCompSignal, statMsg->getDataCountReceivable());

        delete statMsg;

    } else {

        EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<Received unexpected packet \n";
        delete msg;
    }

}

void KHeraldApp::finish()
{
    cancelAndDelete(appRegistrationEvent);

    cancelEvent(dataTimeoutEvent);
    delete dataTimeoutEvent;

    myLikenesses.clear();
    if (popularityList.size() > 0)
    	popularityList.clear();
    timesMessagesReceived.clear();

    if (heraldNodeInfoList.size() > 0) {
        for (int i = 0; i < heraldNodeInfoList.size(); i++) {
            KBaseNodeInfo* nodeInfo = heraldNodeInfoList[i];
            delete nodeInfo;
        }
        heraldNodeInfoList.clear();
    }
}

//
// The model implementation for the Herald application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de), Anna Förster (afoerster@uni-bremen.de)
// @date   : 15-aug-2016, updated 6-febr-2018
//
//

#include "KMessengerApp.h"

#include "KBaseNodeInfo.h"

Define_Module(KMessengerApp);

vector<KBaseNodeInfo*> nodeInfoList;

void KMessengerApp::initialize(int stage)
{
    if (stage == 0) {

        // get parameters
        nodeIndex = par("nodeIndex");
        ownMACAddress = par("ownMACAddress").stringValue();
        usedRNG = par("usedRNG");
        dataGenerationInterval = par("dataGenerationInterval");
        dataSizeInBytes = par("dataSizeInBytes");
        logging = par("logging");
        ttl = par("ttl");
        totalSimulationTime = SimTime::parse(getEnvir()->getConfig()->getConfigValue("sim-time-limit")).dbl();
		notificationCount = totalSimulationTime/dataGenerationInterval;
		totalNumNodes = getParentModule()->getParentModule()->par("numNodes");

        // add own address to global list to use for random destination selections
        ownNodeInfo = new KBaseNodeInfo();
        ownNodeInfo->nodeAddress = ownMACAddress;
        ownNodeInfo->nodeMessengerAppModule = this;
        nodeInfoList.push_back(ownNodeInfo);

        for (int i = 0; i < notificationCount; i++) {
            timesMessagesReceived.push_back(0);
        }

    } else if (stage == 1) {


    } else if (stage == 2) {

        // create and setup app registration trigger
        appRegistrationEvent = new cMessage("App Registration Event");
        appRegistrationEvent->setKind(KMESSENGERAPP_REGISTRATION_EVENT);
        scheduleAt(simTime(), appRegistrationEvent);

        // this is a round-robin scheduling of traffic: in a row, everybody gets a chance to send the next packet.
    	dataTimeoutEvent = new cMessage("Data Timeout Event");
        dataTimeoutEvent->setKind(KMESSENGERAPP_DATASEND_EVENT);
        nextGenerationIndex = nodeIndex;
        // add 0.1 secs to the first sending to avoid the simulation to send one more message than expected.
        scheduleAt(simTime() + (dataGenerationInterval * nextGenerationIndex) + 0.1, dataTimeoutEvent);

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

    } else {
        EV_FATAL << KMESSENGERAPP_SIMMODULEINFO << "Something is radically wrong\n";
    }

}

int KMessengerApp::numInitStages() const
{
    return 3;
}

void KMessengerApp::handleMessage(cMessage *msg)
{

    // check message
    if (msg->isSelfMessage() && msg->getKind() == KMESSENGERAPP_REGISTRATION_EVENT) {
        // send app registration message the forwarding layer
        KRegisterAppMsg *regAppMsg = new KRegisterAppMsg("Messenger App Registration");
        regAppMsg->setAppName("Messenger");
        regAppMsg->setPrefixName(prefixName);

        send(regAppMsg, "lowerLayerOut");

    } else if (msg->isSelfMessage() && msg->getKind() == KMESSENGERAPP_DATASEND_EVENT) {

        // find random destination to send
        bool found = FALSE;
        KBaseNodeInfo* selectedNodeInfo = nodeInfoList[0];
        while (!found) {
            int destIndex = intuniform(0, (nodeInfoList.size() - 1), usedRNG);
            selectedNodeInfo = nodeInfoList[destIndex];
            if (selectedNodeInfo->nodeAddress != ownMACAddress) {
                found = TRUE;
            }
        }

        //setup the data message for sending down to forwarding layer
        char tempString[128];
        sprintf(tempString, "D item-%0d", KMESSENGERAPP_START_ITEM_ID + nextGenerationIndex);

        KDataMsg *dataMsg = new KDataMsg(tempString);

        dataMsg->setSourceAddress("");
        dataMsg->setDestinationAddress("");

        sprintf(tempString, "/messenger/item-%0d", KMESSENGERAPP_START_ITEM_ID + nextGenerationIndex);

        dataMsg->setDataName(tempString);
        dataMsg->setGoodnessValue(100);
        dataMsg->setRealPayloadSize(dataSizeInBytes);
        dataMsg->setMsgUniqueID(nextGenerationIndex);
        dataMsg->setInitialInjectionTime(simTime());

        sprintf(tempString, "Details of item-%0d", KMESSENGERAPP_START_ITEM_ID + nextGenerationIndex);
        dataMsg->setDummyPayloadContent(tempString);
		
        dataMsg->setByteLength(dataSizeInBytes);
        dataMsg->setMsgType(0);
        dataMsg->setValidUntilTime(ttl);

        dataMsg->setInitialOriginatorAddress(ownMACAddress.c_str());
        dataMsg->setDestinationOriented(true);
        dataMsg->setFinalDestinationAddress(selectedNodeInfo->nodeAddress.c_str());

        send(dataMsg, "lowerLayerOut");

		// schedule again
		nextGenerationIndex += totalNumNodes;	
	    scheduleAt(simTime() + (dataGenerationInterval * totalNumNodes), msg);
        
        // make receivable node generate stats
        KStatisticsMsg *statMsg = new KStatisticsMsg("StatMsg");
        statMsg->setLikedDataCountReceivable(1);
        statMsg->setLikedDataBytesReceivable(dataSizeInBytes);
        sendDirect(statMsg, selectedNodeInfo->nodeMessengerAppModule, "statIn");

        //cout << KMESSENGERAPP_SIMMODULEINFO << " sending: " << (nextGenerationIndex - totalNumNodes) << " own addr " << ownMACAddress << " dest addr " << selectedNodeInfo->nodeAddress << "\n";

    } else if (dynamic_cast<KDataMsg*>(msg) != NULL) {

        // message received from outside so, process received data message
        KDataMsg *dataMsg = check_and_cast<KDataMsg *>(msg);

        // check if it is a duplicate receipt
        int i = dataMsg->getMsgUniqueID();
        timesMessagesReceived[i]++;

        // compute stats
        simtime_t diff = 0;
        if (timesMessagesReceived[i] > 1) {
            emit(duplicateDataCountReceivedSignal, (int) 1);
            emit(totalDataCountReceivedSignal, (int) 1);
            emit(duplicateDataBytesReceivedSignal, dataMsg->getRealPayloadSize());
            emit(totalDataBytesReceivedSignal, dataMsg->getRealPayloadSize());

        } else {
            emit(likedDataCountReceivedSignal, (int) 1);
            emit(totalDataCountReceivedSignal, (int) 1);
            emit(likedDataBytesReceivedSignal, dataMsg->getRealPayloadSize());
            emit(totalDataBytesReceivedSignal, dataMsg->getRealPayloadSize());

            diff = simTime() - dataMsg->getInitialInjectionTime();
            emit(likedDataReceivedDelaySignal, diff);
            emit(totalDataReceivedDelaySignal, diff);
            emit(likedDataCountReceivedForAvgDelayCompSignal, (int) 1);
            emit(totalDataCountReceivedForAvgDelayCompSignal, (int) 1);

            emit(likedDataCountReceivedForRatioCompSignal, (int) 1);
            emit(totalDataCountReceivedForRatioCompSignal, (int) 1);
        }

        //cout << KMESSENGERAPP_SIMMODULEINFO << " receiving: " << dataMsg->getMsgUniqueID() << " own addr " << ownMACAddress << " dest addr " << dataMsg->getFinalDestinationAddress() << " msg delay " << diff << "\n";

        delete msg;
        
    } else if (dynamic_cast<KStatisticsMsg*>(msg) != NULL) {
        // message received from outside so, process received statistics message
        KStatisticsMsg *statMsg = check_and_cast<KStatisticsMsg *>(msg);

        // update receivables
        emit(likedDataCountMaxReceivableSignal, statMsg->getLikedDataCountReceivable());
        emit(totalDataCountMaxReceivableSignal, statMsg->getLikedDataCountReceivable());

        emit(likedDataBytesMaxReceivableSignal, statMsg->getLikedDataBytesReceivable());
        emit(totalDataBytesMaxReceivableSignal, statMsg->getLikedDataBytesReceivable());

        emit(likedDataCountMaxReceivableForRatioCompSignal, statMsg->getLikedDataCountReceivable());
        emit(totalDataCountMaxReceivableForRatioCompSignal, statMsg->getLikedDataCountReceivable());

        //cout << KMESSENGERAPP_SIMMODULEINFO << " receivable stat update: " << " own addr " << ownMACAddress << "\n";

        delete statMsg;

    } else {

        EV_INFO << KMESSENGERAPP_SIMMODULEINFO << ">!<Received unexpected packet \n";
        delete msg;
    }

}

void KMessengerApp::finish()
{
    delete appRegistrationEvent;

    cancelEvent(dataTimeoutEvent);
    delete dataTimeoutEvent;

    if (nodeInfoList.size() > 0) {
        for (int i = 0; i < nodeInfoList.size(); i++) {
            KBaseNodeInfo* nodeInfo = nodeInfoList[i];
            delete nodeInfo;
        }
        nodeInfoList.clear();
    }
}

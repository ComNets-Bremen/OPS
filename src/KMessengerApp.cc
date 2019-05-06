//
// The model implementation for the Herald application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de), Anna Förster (afoerster@uni-bremen.de)
// @date   : 15-aug-2016, updated 6-febr-2018
//
//

#include "KMessengerApp.h"

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
        ownNodeInfo.nodeAddress = ownMACAddress;
        string signalName = ownMACAddress + "-likedDataReceivable";
        ownNodeInfo.likedDataReceivableSignalID = registerSignal(signalName);
        subscribe(ownNodeInfo.likedDataReceivableSignalID, this);
        nodeInfoList.push_back(ownNodeInfo);

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
        likedDataBytesReceivedSignal = registerSignal("likedDataBytesReceivedApp");
        nonLikedDataBytesReceivedSignal = registerSignal("nonLikedDataBytesReceivedApp");
        duplicateDataBytesReceivedSignal = registerSignal("duplicateDataBytesReceivedApp");
        totalDataBytesReceivedSignal = registerSignal("totalDataBytesReceivedApp");
        likedDataCountReceivedSignal = registerSignal("likedDataCountReceivedApp");
        nonLikedDataCountReceivedSignal = registerSignal("nonLikedDataCountReceivedApp");
        duplicateDataCountReceivedSignal = registerSignal("duplicateDataCountReceivedApp");
        totalDataCountReceivedSignal = registerSignal("totalDataCountReceivedApp");
        likedDataBytesMaxReceivableSignal = registerSignal("likedDataBytesMaxReceivableApp");
        nonLikedDataBytesMaxReceivableSignal = registerSignal("nonLikedDataBytesMaxReceivableApp");
        totalDataBytesMaxReceivableSignal = registerSignal("totalDataBytesMaxReceivableApp");
        likedDataCountMaxReceivableSignal = registerSignal("likedDataCountMaxReceivableApp");
        nonLikedDataCountMaxReceivableSignal = registerSignal("nonLikedDataCountMaxReceivableApp");
        totalDataCountMaxReceivableSignal = registerSignal("totalDataCountMaxReceivableApp");
        likedDataReceivedAvgDelaySignal = registerSignal("likedDataReceivedAvgDelayApp");
        nonLikedDataReceivedAvgDelaySignal = registerSignal("nonLikedDataReceivedAvgDelayApp");
        totalDataReceivedAvgDelaySignal = registerSignal("totalDataReceivedAvgDelayApp");
        likedDataDeliveryRatioSignal = registerSignal("likedDataDeliveryRatioApp");
        nonLikedDataDeliveryRatioSignal = registerSignal("nonLikedDataDeliveryRatioApp");
        totalDataDeliveryRatioSignal = registerSignal("totalDataDeliveryRatioApp");

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
	    scheduleAt(simTime() + dataGenerationInterval, msg);
        
        // make receivable node generate stats
        KStatMsg *statMsg = new KStatMsg("StatMsg");
        statMsg->likedDataCountReceivable = 1;
        statMsg->likedDataBytesReceivable = dataSizeInBytes;
        emit(selectedNodeInfo->likedDataReceivableSignalID, statMsg);

    } else if (dynamic_cast<KDataMsg*>(msg) != NULL) {

        // message received from outside so, process received data message
        KDataMsg *dataMsg = check_and_cast<KDataMsg *>(msg);

        // generate stats

        
        delete msg;
        
    } else {

        EV_INFO << KMESSENGERAPP_SIMMODULEINFO << ">!<Received unexpected packet \n";
        delete msg;
    }

}

void KMessengerApp::receiveSignal(cComponent *source, simsignal_t signalID, cObject *value, cObject *details)
{
    if (signalID == ownNodeInfo.likedDataReceivableSignalID) {
        KStatisticsMsg *statMsg = check_and_cast<Packet *>(value);

        ownNodeInfo->likedDataBytesMaxReceivable += statMsg->likedDataBytesReceivable;
        ownNodeInfo->likedDataCountMaxReceivable += statMsg->likedDataCountReceivable;

        // generate stats
        

        delete statMsg;
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

//
// The model implementation for the Herald application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de), Anna Förster (afoerster@uni-bremen.de)
// @date   : 15-aug-2016, updated 6-febr-2018
//
//

#include "KMessengerApp.h"

Define_Module(KMessengerApp);

vector<string> addressList;

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
        addressList.push_back(ownMACAddress);

		if (logging) {EV_INFO << KMESSENGERAPP_SIMMODULEINFO << ">!<NLB>!<C>!<" << notificationCount << "\n";}
		
        // setup the event notification array
        for (int i = 0; i < notificationCount; i++)
        {
        	timesMessagesReceived.push_back(0);

        	if(logging)
        	{
        		char dataName[128];
        		sprintf(dataName, "/messengers/item-%0d", KMESSENGERAPP_START_ITEM_ID + i);
        		EV_INFO << KMESSENGERAPP_SIMMODULEINFO << ">!<NE>!<" 
        			<< dataName << ">!<"
                	<< "100" << ">!<" << "100" << ">!<"
                	<< "100" << ">!<"
                	<< "L" << ">!<"
                	<< ttl << "\n";
        	}
        }
        
        if (logging) {EV_INFO << KMESSENGERAPP_SIMMODULEINFO << ">!<NLE>!<C>!<" << notificationCount << "\n";}
        
        nextGenerationNotification = 0;
  

    } else if (stage == 1) {


    } else if (stage == 2) {

        // create and setup app registration trigger
        appRegistrationEvent = new cMessage("App Registration Event");
        appRegistrationEvent->setKind(KMESSENGERAPP_REGISTRATION_EVENT);
        scheduleAt(simTime(), appRegistrationEvent);

        // this is a round-robin scheduling of traffic: in a row, everybody gets a chance to send the next packet.
            
    	dataTimeoutEvent = new cMessage("Data Timeout Event");
        dataTimeoutEvent->setKind(KMESSENGERAPP_DATASEND_EVENT);
        // add 0.1 secs to the first sending to avoid the simulation to send one more message than expected.
        scheduleAt(simTime() + dataGenerationInterval*nodeIndex + 0.1, dataTimeoutEvent);
        nextGenerationNotification = nodeIndex;

        if (logging) {EV_INFO << KMESSENGERAPP_SIMMODULEINFO << ">!<SUTG "<< notificationCount << " DI" << "\n";}

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

        if (logging) {EV_INFO << KMESSENGERAPP_SIMMODULEINFO << ">!<GAR" << "\n";}

    } else if (msg->isSelfMessage() && msg->getKind() == KMESSENGERAPP_DATASEND_EVENT) {

		// mark this message as received by this node
		timesMessagesReceived[nextGenerationNotification]++;
        
        // find random destination to send
        string finalDestinationAddress = ownMACAddress;
        while(finalDestinationAddress != ownMACAddress) {
            int destIndex = intuniform(0, (addressList.size() - 1), usedRNG);
            finalDestinationAddress = addressList[destIndex];
        }

		//setup the data message for sending down to forwarding layer
		char tempString[128];
		sprintf(tempString, "D item-%0d", KMESSENGERAPP_START_ITEM_ID + nextGenerationNotification);
		
		KDataMsg *dataMsg = new KDataMsg(tempString);

		dataMsg->setSourceAddress("");
		dataMsg->setDestinationAddress("");
		
		sprintf(tempString, "/messenger/item-%0d", KMESSENGERAPP_START_ITEM_ID + nextGenerationNotification);
		
		dataMsg->setDataName(tempString);
		dataMsg->setGoodnessValue(100);
		dataMsg->setRealPayloadSize(dataSizeInBytes);
		dataMsg->setMsgUniqueID(nextGenerationNotification);
		
		sprintf(tempString, "Details of item-%0d", KMESSENGERAPP_START_ITEM_ID + nextGenerationNotification);
		dataMsg->setDummyPayloadContent(tempString);
		
		dataMsg->setByteLength(dataSizeInBytes);
		dataMsg->setMsgType(0);
		dataMsg->setValidUntilTime(ttl);

        dataMsg->setInitialOriginatorAddress(ownMACAddress.c_str());
        dataMsg->setDestinationOriented(true);
        dataMsg->setFinalDestinationAddress(finalDestinationAddress.c_str());

		send(dataMsg, "lowerLayerOut");

		if (logging) {EV_INFO << KMESSENGERAPP_SIMMODULEINFO << ">!<GD>!<" << dataMsg->getDataName() << "\n";}
		
		// schedule again after a complete round robin of all nodes
		nextGenerationNotification += totalNumNodes;	
	    scheduleAt(simTime() + dataGenerationInterval*totalNumNodes, msg);


    } else if (dynamic_cast<KDataMsg*>(msg) != NULL) {

        // message received from outside so, process received data message
        KDataMsg *dataMsg = check_and_cast<KDataMsg *>(msg);

        if (logging) {EV_INFO << KMESSENGERAPP_SIMMODULEINFO << ">!<RD>!<" << dataMsg->getDataName() << "\n";}

        // increment number of times seen
        timesMessagesReceived[dataMsg->getMsgUniqueID()]++;
        
        delete msg;

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

    addressList.clear();
    timesMessagesReceived.clear();
}

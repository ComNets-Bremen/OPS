//
// The model implementation for the Herald application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de), Anna Förster (afoerster@uni-bremen.de)
// @date   : 15-aug-2016, updated 6-febr-2018
//
//

#include "KHeraldApp.h"

Define_Module(KHeraldApp);

vector<int> popularityList;


void KHeraldApp::initialize(int stage)
{
    if (stage == 0) {

        // get parameters
        nodeIndex = par("nodeIndex");
        usedRNG = par("usedRNG");
        dataGenerationInterval = par("dataGenerationInterval");
        popularityAssignmentPercentage = par("popularityAssignmentPercentage");
        dataSizeInBytes = par("dataSizeInBytes");
        logging = par("logging");
        ttl = par("ttl");
        totalSimulationTime = SimTime::parse(getEnvir()->getConfig()->getConfigValue("sim-time-limit")).dbl();
		notificationCount = totalSimulationTime/dataGenerationInterval;
		totalNumNodes = getParentModule()->getParentModule()->par("numNodes");
		
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

		if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<NLB>!<C>!<" << notificationCount << "\n";}
		
        // setup the event notification array
        for (int i = 0; i < notificationCount; i++)
        {
        	int like = 0;
        	if (popularityList[i] > 90)
        		like = 100;
        	myLikenesses.push_back(like);
        	timesMessagesReceived.push_back(0);
        	
        	int goodness = like;
        	
        	if(logging)
        	{
        		char dataName[128];
        		sprintf(dataName, "/herald/item-%0d", KHERALDAPP_START_ITEM_ID + i);
        		EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<NE>!<" 
        			<< dataName << ">!<"
                	<< popularityList[i] << ">!<" << like << ">!<"
                	<< goodness << ">!<"
                	<< (goodness == 100 ? "L" : "I") << ">!<"
                	<< ttl << "\n";
        	}
        }
        
        if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<NLE>!<C>!<" << notificationCount << "\n";}
        
        nextGenerationNotification = 0;
  

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
        scheduleAt(simTime() + dataGenerationInterval*nodeIndex + 0.1, dataTimeoutEvent);
        nextGenerationNotification = nodeIndex;

        if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<SUTG "<< notificationCount << " DI" << "\n";}


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

        if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<GAR" << "\n";}

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
		
		sprintf(tempString, "Details of item-%0d", KHERALDAPP_START_ITEM_ID + nextGenerationNotification);
		dataMsg->setDummyPayloadContent(tempString);
		
		dataMsg->setByteLength(dataSizeInBytes);
		dataMsg->setMsgType(0);
		dataMsg->setValidUntilTime(ttl);

		send(dataMsg, "lowerLayerOut");

		if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<GD>!<" << dataMsg->getDataName() << "\n";}
		
		// schedule again after a complete round robin of all nodes
		nextGenerationNotification += totalNumNodes;	
	    scheduleAt(simTime() + dataGenerationInterval*totalNumNodes, msg);


    } else if (dynamic_cast<KDataMsg*>(msg) != NULL) {

        // message received from outside so, process received data message
        KDataMsg *dataMsg = check_and_cast<KDataMsg *>(msg);

        if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<RD>!<" << dataMsg->getDataName() << "\n";}

        // increment number of times seen
        timesMessagesReceived[dataMsg->getMsgUniqueID()]++;
        
        delete msg;

    } else {

        EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<Received unexpected packet \n";
        delete msg;
    }

}

void KHeraldApp::finish()
{
    delete appRegistrationEvent;

    cancelEvent(dataTimeoutEvent);
    delete dataTimeoutEvent;


    myLikenesses.clear();
    if (popularityList.size() > 0)
    	popularityList.clear();
    timesMessagesReceived.clear();

}


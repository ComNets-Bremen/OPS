//
// The model implementation for the Herald application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 15-aug-2016
//
//

#include "KHeraldApp.h"

Define_Module(KHeraldApp);

void KHeraldApp::initialize(int stage)
{
    if (stage == 0) {

        // get parameters
        nodeIndex = par("nodeIndex");
        notificationCount = par("notificationCount");
        usedRNG = par("usedRNG");
		
		totalSimulationTime = SimTime::parse(getEnvir()->getConfig()->getConfigValue("sim-time-limit")).dbl();

    } else if (stage == 1) {
		
		// assign own goodness values
		for (int i = 0; i < notificationCount; i++) {
			int goodnessValue = ((int) uniform(0.0, 5.0, usedRNG))  * 25;
			
			notificationItem[i].goodnessValue = goodnessValue;
		}
		
		// dump the notification list with given goodness value
        EV_INFO << KHERALDAPP_SIMMODULEINFO << " :: Notification List Begin :: Count :: " << notificationCount << "\n";
		for (int i = 0; i < notificationCount; i++) {
	        EV_INFO << KHERALDAPP_SIMMODULEINFO << " :: Notification Entry :: " << notificationItem[i].dataName << " :: " 
				<< notificationItem[i].goodnessValue << " :: " << notificationItem[i].validUntilTime << "\n";		
		}
        EV_INFO << KHERALDAPP_SIMMODULEINFO << " :: Notification List End :: Count :: " << notificationCount << "\n";
		

    } else if (stage == 2) {

        // create and setup app registration trigger
        appRegistrationEvent = new cMessage("App Registration Event");
        appRegistrationEvent->setKind(92);
        scheduleAt(simTime(), appRegistrationEvent);

        // create and setup data generation trigger (only the given index)
        if (nodeIndex == dataGeneratingNodeIndex) {
            dataTimeoutEvent = new cMessage("Data Timeout Event");
            dataTimeoutEvent->setKind(93);
            scheduleAt(simTime() + dataGenerationInterval, dataTimeoutEvent);
			
            EV_INFO << KHERALDAPP_SIMMODULEINFO << " :: Setting up to generate "<< notificationCount << " data items" << "\n";
        }

        // create and setup feedback generation trigger
		maxFeedbackGenerationInterval = totalSimulationTime / notificationCount;
		feedbackGenerationInterval = uniform(1.0, maxFeedbackGenerationInterval, usedRNG);
        feedbackTimeoutEvent = new cMessage("Feedback Timeout Event");
        feedbackTimeoutEvent->setKind(94);
        scheduleAt(simTime() + feedbackGenerationInterval, feedbackTimeoutEvent);


    } else {
        EV_FATAL << KHERALDAPP_SIMMODULEINFO << "Something is radically wrong\n";
    }


    //cout << "Coming out of BruitclerApp::initialize" << "\n";
}

int KHeraldApp::numInitStages() const
{
    return 3;
}

void KHeraldApp::handleMessage(cMessage *msg)
{

    // check message
    if (msg->isSelfMessage() && msg->getKind() == 92) {
        // send app registration message the forwarding layer
        KRegisterAppMsg *regAppMsg = new KRegisterAppMsg("Herald App Registration");
        regAppMsg->setAppName("Herald");
        regAppMsg->setPrefixName(prefixName);

        send(regAppMsg, "lowerLayerOut");

        EV_INFO << KHERALDAPP_SIMMODULEINFO << " :: Generated App Registration" << "\n";
		
    } else if (msg->isSelfMessage() && msg->getKind() == 93) {
        // timeout for data (notification) send event occured
        // so, generate a data message
		
		lastGeneratedNotification++;

		notificationItem[lastGeneratedNotification].timesDataMsgReceived = 1;
		
        KDataMsg *dataMsg = new KDataMsg(notificationItem[lastGeneratedNotification].dataMsgName);

        dataMsg->setSourceAddress("");
        dataMsg->setDestinationAddress("");
        dataMsg->setDataName(notificationItem[lastGeneratedNotification].dataName);
        dataMsg->setGoodnessValue(notificationItem[lastGeneratedNotification].goodnessValue);
        dataMsg->setRealPayloadSize(notificationItem[lastGeneratedNotification].realPayloadSize);
        dataMsg->setDummyPayloadContent(notificationItem[lastGeneratedNotification].dummyPayloadContent);
        dataMsg->setByteLength(notificationItem[lastGeneratedNotification].dataByteLength);
		dataMsg->setMsgType(0);
		dataMsg->setValidUntilTime(notificationItem[lastGeneratedNotification].validUntilTime);

        send(dataMsg, "lowerLayerOut");

        EV_INFO << KHERALDAPP_SIMMODULEINFO << " :: Generated Data :: " << dataMsg->getDataName() << "\n";

        // setup next data generation trigger only if limit has not exceeded
		if ((lastGeneratedNotification + 1) < notificationCount) {
            scheduleAt(simTime() + dataGenerationInterval, msg);
        }

		
    } else if (msg->isSelfMessage() && msg->getKind() == 94) {
        // timeout for feedback send event occured so, generate a feedback message


		// randomly select a received notification to send a feedback
		int notificationIndex = -1;
		int found = FALSE;
		for (int i = 0; i < notificationCount; i++) {
			
			if (notificationItem[i].timesDataMsgReceived > 0 && notificationItem[i].validUntilTime > simTime().dbl()
				&& !notificationItem[i].feedbackGenerated) {
				found = TRUE;
				notificationIndex = i;
				break;
			}
		}
		
		if (found) {
				
        	KFeedbackMsg *feedbackMsg = new KFeedbackMsg(notificationItem[notificationIndex].feedbackMsgName);

        	feedbackMsg->setSourceAddress("");
        	feedbackMsg->setDestinationAddress("");
        	feedbackMsg->setDataName(notificationItem[notificationIndex].dataName);
        	feedbackMsg->setGoodnessValue(notificationItem[notificationIndex].goodnessValue);
        	feedbackMsg->setByteLength(notificationItem[notificationIndex].feedbackByteLength);

        	send(feedbackMsg, "lowerLayerOut");

        	EV_INFO << KHERALDAPP_SIMMODULEINFO << " :: Generated Feedback :: " << feedbackMsg->getDataName() << "\n";
			
			notificationItem[notificationIndex].feedbackGenerated = TRUE;
		}
		
        // setup next feedback generation trigger
		feedbackGenerationInterval = uniform(1.0, maxFeedbackGenerationInterval, usedRNG);
        scheduleAt(simTime() + feedbackGenerationInterval, msg);

    } else if (dynamic_cast<KDataMsg*>(msg) != NULL) {

        // message received from outside so, process received data message
        KDataMsg *dataMsg = check_and_cast<KDataMsg *>(msg);

        EV_INFO << KHERALDAPP_SIMMODULEINFO << " :: Received Data :: " << dataMsg->getDataName() << "\n";

        // check if data seen before
		int notificationIndex = -1;
		for (int i = 0; i < notificationCount; i++) {
			if (notificationItem[i].dataName == dataMsg->getDataName()) {
				notificationIndex = i;
				break;
			}
		}
		
		if (notificationIndex >= 0) {
			notificationItem[notificationIndex].timesDataMsgReceived++;
		}
		
        delete msg;

    } else {

        EV_INFO << KHERALDAPP_SIMMODULEINFO << " :: Received unexpected packet \n";
        delete msg;
    }

}

void KHeraldApp::finish()
{
	delete appRegistrationEvent;
	
	// remove data generation event
    if (nodeIndex == dataGeneratingNodeIndex) {
		if (lastGeneratedNotification < notificationCount) {
			cancelEvent(dataTimeoutEvent);
		}
        delete dataTimeoutEvent;
	}
	
	// remove feedback generation event
	cancelEvent(feedbackTimeoutEvent);
	delete feedbackTimeoutEvent;
	
}

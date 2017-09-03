//
// The model implementation for the Bruit application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 18-sep-2015
//
//

#include "KBruitApp.h"

Define_Module(KBruitApp);

void KBruitApp::initialize(int stage)
{
    if (stage == 0) {

        // get parameters
        nodeIndex = par("nodeIndex");
        operationMode = par("operationMode");
        dataGeneratingNodeIndex = par("dataGeneratingNodeIndex");
        numberOfData = par("numberOfData");
        appPrefix = par("appPrefix").stringValue();
        appNameGenPrefix = par("appNameGenPrefix").stringValue();
        dataGenerationInterval = par("dataGenerationInterval");
        feedbackGenerationInterval = par("feedbackGenerationInterval");
        logging = par("logging");

        // setup stat collection signals
        dataSendSignal = registerSignal("dataSendSignal");
        dataReceiveSignal = registerSignal("dataReceiveSignal");
        feedbackSendSignal = registerSignal("feedbackSendSignal");

    } else if (stage == 1) {

    } else if (stage == 2) {

        // create and setup app registration trigger
        appRegistrationEvent = new cMessage("App Registration Event");
        appRegistrationEvent->setKind(92);
        scheduleAt(simTime(), appRegistrationEvent);

        // create and setup data generation trigger (in operation mode 2, only the given index)
        if (operationMode == 1 || (operationMode == 2 && nodeIndex == dataGeneratingNodeIndex)) {
            dataTimeoutEvent = new cMessage("Data Timeout Event");
            dataTimeoutEvent->setKind(93);
            scheduleAt(simTime() + dataGenerationInterval, dataTimeoutEvent);
        }

        if (operationMode == 2 && nodeIndex == dataGeneratingNodeIndex) {
            if (logging) {EV_INFO << KBRUITAPP_SIMMODULEINFO << "SUTG "<< numberOfData << " DI" << "\n";}
        }

        // create and setup feedback generation trigger
        feedbackTimeoutEvent = new cMessage("Feedback Timeout Event");
        feedbackTimeoutEvent->setKind(94);
        scheduleAt(simTime() + feedbackGenerationInterval, feedbackTimeoutEvent);

        generatedDataCount = 0;

    } else {
        EV_FATAL << KBRUITAPP_SIMMODULEINFO << "Something is radically wrong\n";
    }


    //cout << "Coming out of BruitclerApp::initialize" << "\n";
}

int KBruitApp::numInitStages() const
{
    return 3;
}

void KBruitApp::handleMessage(cMessage *msg)
{

    // check message
    if (msg->isSelfMessage() && msg->getKind() == 92) {
        // send app registration message the forwarding layer
        KRegisterAppMsg *regAppMsg = new KRegisterAppMsg("Bruit App Registration");
        regAppMsg->setAppName("Bruit");
        regAppMsg->setPrefixName(appPrefix.c_str());

        send(regAppMsg, "lowerLayerOut");

        if (logging) {EV_INFO << KBRUITAPP_SIMMODULEINFO << ">!<GAR" << "\n";}

    } else if (msg->isSelfMessage() && msg->getKind() == 93) {
        // timeout for data send event occured
        // so, generate a data message
        char dummyPayloadContent[64], dataName[64], msgName[64];
        int itemNum;

        itemNum = (rand() % 99999) + 1;
        sprintf(dataName, "%s/%s%05d", appPrefix.c_str(), appNameGenPrefix.c_str(), itemNum);
        sprintf(dummyPayloadContent, "Details of %s%05d", appNameGenPrefix.c_str(), itemNum);
        sprintf(msgName, "D %s%05d", appNameGenPrefix.c_str(), itemNum);

        KDataMsg *dataMsg = new KDataMsg(msgName);

        dataMsg->setSourceAddress("");
        dataMsg->setDestinationAddress("");
        dataMsg->setDataName(dataName);
        dataMsg->setGoodnessValue(100);
        dataMsg->setRealPayloadSize(512);
        dataMsg->setDummyPayloadContent(dummyPayloadContent);
        dataMsg->setByteLength(592);
		dataMsg->setMsgType(0);
		dataMsg->setValidUntilTime(0.0);

        send(dataMsg, "lowerLayerOut");

        generatedDataCount++;

        // send stat signal
        emit(dataSendSignal, 1);

        if (logging) {EV_INFO << KBRUITAPP_SIMMODULEINFO << ">!<GD>!<" << dataMsg->getDataName() << "\n";}

        // get the next data generation interval and setup next data generation trigger
        // only if limit has not exceeded
        if (operationMode == 1
                || (operationMode == 2 && nodeIndex == dataGeneratingNodeIndex && generatedDataCount < numberOfData)) {
            dataGenerationInterval = par("dataGenerationInterval");
            scheduleAt(simTime() + dataGenerationInterval, msg);
        }

    } else if (msg->isSelfMessage() && msg->getKind() == 94) {
        // timeout for feedback send event occured
        // so, generate a feedback message

        // if no data names in list, no feedbacks
        if (knownDataNameList.size() > 0) {

            // get a uniformly distributed random number to identify
            // the data for which a feedback is sent
            int dataNameIndex = rand() % knownDataNameList.size();
            list<string*>::iterator dataNameIterator = knownDataNameList.begin();
            advance(dataNameIterator, dataNameIndex);
            string *dataName = *dataNameIterator;
            char msgName[64];

            // get a uniformly distributed random number to identify
            // the goodness value
            int goodnessValue = (rand() % 100) + 1;

            sprintf(msgName, "F %s", (*dataName).c_str());

            KFeedbackMsg *feedbackMsg = new KFeedbackMsg(msgName);

            feedbackMsg->setSourceAddress("");
            feedbackMsg->setDestinationAddress("");
            feedbackMsg->setDataName((*dataName).c_str());
            feedbackMsg->setGoodnessValue(goodnessValue);
            feedbackMsg->setByteLength(78);

            send(feedbackMsg, "lowerLayerOut");

            // send stat signal
            emit(feedbackSendSignal, 1);

            // setup next feedback generation trigger
            if (logging) {EV_INFO << KBRUITAPP_SIMMODULEINFO << ">!<GF>!<" << *dataName << "\n";}

        }

        // get next feedback generation interval and setup next feedback generation trigger
        feedbackGenerationInterval = par("feedbackGenerationInterval");
        scheduleAt(simTime() + feedbackGenerationInterval, msg);

    } else if (dynamic_cast<KDataMsg*>(msg) != NULL) {

        // message received from outside
        // so, process received data message
        KDataMsg *dataMsg = check_and_cast<KDataMsg *>(msg);

        // send stat signal
        emit(dataReceiveSignal, 1);

        if (logging) {EV_INFO << KBRUITAPP_SIMMODULEINFO << ">!<RD>!<" << dataMsg->getDataName() << "\n";}

        // check if data seen before
        int found = 0;
        list<string*>::iterator iteratorDataName = knownDataNameList.begin();
        while (iteratorDataName != knownDataNameList.end()) {
            string *dataName = *iteratorDataName;
            if (*dataName == dataMsg->getDataName()) {
                found = 1;
                break;
            }
            iteratorDataName++;
        }
        if (!found) {
            // if name not in list, add it to list
            string *dataName = new string(dataMsg->getDataName());
            knownDataNameList.push_back(dataName);
        }

        delete msg;

    } else {

        EV_INFO << KBRUITAPP_SIMMODULEINFO << ">!<Received unexpected packet \n";
        delete msg;
    }

}

void KBruitApp::finish()
{
	delete appRegistrationEvent;
    if (operationMode == 1 || (operationMode == 2 && nodeIndex == dataGeneratingNodeIndex)) {
		cancelEvent(dataTimeoutEvent);
        delete dataTimeoutEvent;
	}
	cancelEvent(feedbackTimeoutEvent);
	delete feedbackTimeoutEvent;
	
}

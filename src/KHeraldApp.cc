//
// The model implementation for the Herald application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 15-aug-2016
//
//

#include "KHeraldApp.h"

Define_Module(KHeraldApp);

list<int> popularityList;
list<int> nodeIndexList;
int nextGeneratorNodeIndex;
int nextGenerationNotification;
double nextGenerationTime;

void KHeraldApp::initialize(int stage)
{
    if (stage == 0) {

        // get parameters
        nodeIndex = par("nodeIndex");
        notificationCount = par("notificationCount");
        usedRNG = par("usedRNG");
        dataGenerationInterval = par("dataGenerationInterval");
        dataGeneratingNodeIndex = par("dataGeneratingNodeIndex");
        popularityAssignmentPercentage = par("popularityAssignmentPercentage");
        dataSizeInBytes = par("dataSizeInBytes");
        logging = par("logging");

        totalSimulationTime = SimTime::parse(getEnvir()->getConfig()->getConfigValue("sim-time-limit")).dbl();

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
        //   resulting value is compared to 90: if the value is greater or equal to 90, LOVE. If not, IGNORE.
        // - The final “like” values (only 0 and 100s possible) are stored and used later at simulation time
        //   to represent the reaction of the user and to send to Keetchi as goodness values.


        // assign popularity values for the popularityAssignmentPercentage of notificationCount
        if (popularityList.size() == 0) {
            for (int i = 0; i < notificationCount; i++) {
                // double perc = (double) (i + 1) / notificationCount * 100.0;
                double perc = uniform(0.0, 100.0, usedRNG);
                int popularity = 0;

                if (perc <= popularityAssignmentPercentage) {
                    popularity = 100;
                }

                //
                //
                // if (perc <= popularityAssignmentPercentage) {
                //     popularity = intuniform(1, 20, usedRNG);
                // }

                popularityList.push_back(popularity);
            }
        }

        // setup the event notification array
        for (int i = 0; i < notificationCount; i++) {
            NotificationItem *notificationItem = new NotificationItem();

            notificationItem->itemNumber = 22000 + i;
            sprintf(notificationItem->dataName, "/herald/item-%0d", notificationItem->itemNumber);
            sprintf(notificationItem->dummyPayloadContent, "Details of item-%0d", notificationItem->itemNumber);
            sprintf(notificationItem->dataMsgName, "D item-%0d", notificationItem->itemNumber);
            sprintf(notificationItem->feedbackMsgName, "F item-%0d", notificationItem->itemNumber);

            list<int>::iterator iteratorPopularity = popularityList.begin();
            advance(iteratorPopularity, i);
            notificationItem->popularity = *iteratorPopularity;
            
            notificationItem->likeness = 0;
            if (notificationItem->popularity == 100) {
                notificationItem->likeness = 100;
            }
            
            //
            // notificationItem->likeness = intuniform(notificationItem->popularity, 100, usedRNG);
            if (notificationItem->likeness >= 90) {
                notificationItem->goodnessValue = 100;
            } else {
                notificationItem->goodnessValue = 0;
            }

            notificationItem->realPayloadSize = dataSizeInBytes;
            notificationItem->dataByteLength = dataSizeInBytes;
            notificationItem->feedbackByteLength = 78;
            notificationItem->validUntilTime = 99999999.0;
            notificationItem->timesDataMsgReceived = 0;
            notificationItem->feedbackGenerated = FALSE;

            notificationList.push_back(notificationItem);
        }
        
        // update current node index (used to generate notifications later)
        nodeIndexList.push_back(nodeIndex);
        nextGeneratorNodeIndex = 0;
        nextGenerationNotification = 0;
        nextGenerationTime = dataGenerationInterval;

    } else if (stage == 1) {

        // dump the notification list with given popularity, likeness and goodness value
        if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<NLB>!<C>!<" << notificationCount << "\n";}
        for (int i = 0; i < notificationCount; i++) {
            list<NotificationItem*>::iterator it = notificationList.begin();
            advance(it, i);
            NotificationItem *notificationItem = *it;

            if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<NE>!<" << notificationItem->dataName << ">!<"
                << notificationItem->popularity << ">!<" << notificationItem->likeness << ">!<"
                << notificationItem->goodnessValue << ">!<"
                << (notificationItem->goodnessValue == 100 ? "L" : "I") << ">!<"
                << notificationItem->validUntilTime << "\n";}
        }
        if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<NLE>!<C>!<" << notificationCount << "\n";}


    } else if (stage == 2) {

        // create and setup app registration trigger
        appRegistrationEvent = new cMessage("App Registration Event");
        appRegistrationEvent->setKind(92);
        scheduleAt(simTime(), appRegistrationEvent);

        // create and setup data generation trigger
        // == -1 -> every node gets a chance to generate one or more
        // >=  0 -> only the given node
        if (dataGeneratingNodeIndex == -1 || nodeIndex == dataGeneratingNodeIndex) {
            
            dataTimeoutEvent = new cMessage("Data Timeout Event");
            dataTimeoutEvent->setKind(93);
            scheduleAt(simTime() + dataGenerationInterval, dataTimeoutEvent);

            if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<SUTG "<< notificationCount << " DI" << "\n";}
        }

        // create and setup feedback generation trigger
        maxFeedbackGenerationInterval = totalSimulationTime / notificationCount;
        feedbackGenerationInterval = uniform(1.0, maxFeedbackGenerationInterval, usedRNG);
        feedbackTimeoutEvent = new cMessage("Feedback Timeout Event");
        feedbackTimeoutEvent->setKind(94);
        // feedback sending stopped
        // scheduleAt(simTime() + feedbackGenerationInterval, feedbackTimeoutEvent);


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

        if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<GAR" << "\n";}

    } else if (msg->isSelfMessage() && msg->getKind() == 93) {

        // timeout for data (notification) send event occured
        // so, generate a data message
        if ((nextGenerationNotification < notificationCount)
            && (nodeIndex == dataGeneratingNodeIndex 
                || (dataGeneratingNodeIndex == -1 && nodeIndex == nextGeneratorNodeIndex))
            && simTime().dbl() >= nextGenerationTime) {

                list<NotificationItem*>::iterator it = notificationList.begin();
                advance(it, nextGenerationNotification);
                NotificationItem *notificationItem = *it;

                notificationItem->timesDataMsgReceived = 1;

                KDataMsg *dataMsg = new KDataMsg(notificationItem->dataMsgName);

                dataMsg->setSourceAddress("");
                dataMsg->setDestinationAddress("");
                dataMsg->setDataName(notificationItem->dataName);
                dataMsg->setGoodnessValue(notificationItem->goodnessValue);
                dataMsg->setRealPayloadSize(notificationItem->realPayloadSize);
                dataMsg->setDummyPayloadContent(notificationItem->dummyPayloadContent);
                dataMsg->setByteLength(notificationItem->dataByteLength);
                dataMsg->setMsgType(0);
                dataMsg->setValidUntilTime(notificationItem->validUntilTime);

                send(dataMsg, "lowerLayerOut");

                if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<GD>!<" << dataMsg->getDataName() << "\n";}
                
                nextGeneratorNodeIndex++;
                if (nextGeneratorNodeIndex >= nodeIndexList.size()) {
                    nextGeneratorNodeIndex = 0;
                }
                nextGenerationNotification++;
                nextGenerationTime = simTime().dbl() + dataGenerationInterval;
        }
        
        // setup next data generation trigger
        scheduleAt(simTime() + dataGenerationInterval, msg);

        // if (lastGeneratedNotification == (notificationCount - 1)) {
            // cout << simTime() << " " << getParentModule()->getFullName() << " " << "message generated - " << lastGeneratedNotification << "\n";
        // }

    } else if (msg->isSelfMessage() && msg->getKind() == 94) {
        // timeout for feedback send event occured so, generate a feedback message


        // randomly select a received notification to send a feedback
        int notificationIndex = -1;
        int found = FALSE;
        for (int i = 0; i < notificationCount; i++) {

            list<NotificationItem*>::iterator it = notificationList.begin();
            advance(it, i);
            NotificationItem *notificationItem = *it;

            if (notificationItem->timesDataMsgReceived > 0 && notificationItem->validUntilTime > simTime().dbl()
                && !notificationItem->feedbackGenerated) {
                found = TRUE;
                notificationIndex = i;
                break;
            }
        }

        if (found) {
            list<NotificationItem*>::iterator it = notificationList.begin();
            advance(it, notificationIndex);
            NotificationItem *notificationItem = *it;

            KFeedbackMsg *feedbackMsg = new KFeedbackMsg(notificationItem->feedbackMsgName);

            feedbackMsg->setSourceAddress("");
            feedbackMsg->setDestinationAddress("");
            feedbackMsg->setDataName(notificationItem->dataName);
            feedbackMsg->setGoodnessValue(notificationItem->goodnessValue);
            feedbackMsg->setByteLength(notificationItem->feedbackByteLength);
            feedbackMsg->setFeedbackType(KHERALDAPP_MSGTYPE_PREFERENCE);

            send(feedbackMsg, "lowerLayerOut");

            if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<GF>!<" << feedbackMsg->getDataName() << ">!<T-Pre\n";}

            notificationItem->feedbackGenerated = TRUE;
        }

        // setup next feedback generation trigger
        feedbackGenerationInterval = uniform(1.0, maxFeedbackGenerationInterval, usedRNG);
        scheduleAt(simTime() + feedbackGenerationInterval, msg);

    } else if (dynamic_cast<KDataMsg*>(msg) != NULL) {

        // message received from outside so, process received data message
        KDataMsg *dataMsg = check_and_cast<KDataMsg *>(msg);

        if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<RD>!<" << dataMsg->getDataName() << "\n";}

        // check if data seen before
        int notificationIndex = -1;
        for (int i = 0; i < notificationCount; i++) {
            list<NotificationItem*>::iterator it = notificationList.begin();
            advance(it, i);
            NotificationItem *notificationItem = *it;

            if (strcmp(notificationItem->dataName, dataMsg->getDataName()) == 0) {
                notificationIndex = i;
                break;
            }
        }

        if (notificationIndex >= 0) {
            list<NotificationItem*>::iterator it = notificationList.begin();
            advance(it, notificationIndex);
            NotificationItem *notificationItem = *it;

            notificationItem->timesDataMsgReceived++;
        }

        // feedback sending stopped
        //
        // KFeedbackMsg *feedbackMsg = new KFeedbackMsg(dataMsg->getDataName());
        //
        // feedbackMsg->setSourceAddress("");
        // feedbackMsg->setDestinationAddress("");
        // feedbackMsg->setDataName(dataMsg->getDataName());
        // feedbackMsg->setGoodnessValue(dataMsg->getGoodnessValue());
        // feedbackMsg->setByteLength(78);
        // feedbackMsg->setFeedbackType(KHERALDAPP_MSGTYPE_IMMEDIATE);
        //
        // send(feedbackMsg, "lowerLayerOut");
        //
        // if (logging) {EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<GF>!<" << feedbackMsg->getDataName() << ">!<T-Imm\n";}

        delete msg;

    } else {

        EV_INFO << KHERALDAPP_SIMMODULEINFO << ">!<Received unexpected packet \n";
        delete msg;
    }

}

void KHeraldApp::finish()
{
    delete appRegistrationEvent;

    // remove data generation event
    if (nodeIndex == dataGeneratingNodeIndex || dataGeneratingNodeIndex == -1) {
        cancelEvent(dataTimeoutEvent);
        delete dataTimeoutEvent;
    }

    // remove feedback generation event
    cancelEvent(feedbackTimeoutEvent);
    delete feedbackTimeoutEvent;

    // remove notification events
    while (notificationList.size() > 0) {
        list<NotificationItem*>::iterator it = notificationList.begin();
        advance(it, 0);
        NotificationItem *notificationItem = *it;

        notificationList.remove(notificationItem);
        delete notificationItem;
    }
    
    // remove popularity values
    
    // remove node numbers

}


//
// The model implementation for link adaptations between
// the used link technology (below) and the upper layer.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 12-oct-2015
//
//

#include "KLinkAdaptLayer.h"

Define_Module(KLinkAdaptLayer);

void KLinkAdaptLayer::initialize(int stage)
{
    if (stage == 0) {
        // get parameters
        ownMACAddress = par("ownMACAddress").stringValue();

        // set other parameters
        broadcastMACAddress = "FF:FF:FF:FF:FF:FF";

        // setup stat collection signals
        dataSendSignal = registerSignal("dataSendSignal");
        dataReceiveSignal = registerSignal("dataReceiveSignal");
        feedbackSendSignal = registerSignal("feedbackSendSignal");
        feedbackReceiveSignal = registerSignal("feedbackReceiveSignal");

        // init stat collection variables
        dataSentCount = 0;
        dataReceivedCount = 0;
        feedbackSentCount = 0;
        feedbackReceivedCount = 0;

    } else if (stage == 1) {
    } else if (stage == 2) {

//        // start stat dump timer
//      cMessage *statDumpEvent = new cMessage("Statistics Dump Event");
//      statDumpEvent->setKind(194);
//      scheduleAt(simTime() + 1.0, statDumpEvent);

    } else {
        EV_FATAL << KLINKADAPTLAYER_SIMMODULEINFO << "Something is radically wrong\n";
    }

}

int KLinkAdaptLayer::numInitStages() const
{
    return 3;
}

void KLinkAdaptLayer::handleMessage(cMessage *msg)
{
    cGate *gate;
    char gateName[64];
    KDataMsg *dataMsg;
    KFeedbackMsg *feedbackMsg;
    KNeighbourListMsg *neighListMsg;

    // get through what gate the
    gate = msg->getArrivalGate();
    strcpy(gateName, gate->getName());

    if (strcmp(gateName, "upperLayerIn") == 0 && (dataMsg = dynamic_cast<KDataMsg*>(msg)) != NULL) {
        // received data packet from upper layer, so send it to lower layer

        // set values
        dataMsg->setSourceAddress(ownMACAddress.c_str());
        dataMsg->setDestinationAddress(broadcastMACAddress.c_str());

        // send the packet
        send(dataMsg, "lowerLayerOut");

        // increment data sent count
        dataSentCount++;

    } else if (strcmp(gateName, "upperLayerIn") == 0 && (feedbackMsg = dynamic_cast<KFeedbackMsg*>(msg)) != NULL) {
        // received feedback packet from upper layer, so send it out

        // set values
        feedbackMsg->setSourceAddress(ownMACAddress.c_str());
        feedbackMsg->setDestinationAddress(broadcastMACAddress.c_str());

        // send the packet
        send(feedbackMsg, "lowerLayerOut");

        // increment data sent count
        feedbackSentCount++;

    } else if (strcmp(gateName, "lowerLayerIn") == 0 && (dataMsg = dynamic_cast<KDataMsg*>(msg)) != NULL) {
        // received data packet from lower layer, so send it to upper layer

        // send the packet
        send(dataMsg, "upperLayerOut");

        // increment data received count
        dataReceivedCount++;

    } else if (strcmp(gateName, "lowerLayerIn") == 0 && (feedbackMsg = dynamic_cast<KFeedbackMsg*>(msg)) != NULL) {
        // received feedback packet from lower layer, so send it to upper layer

        // send the packet
        send(feedbackMsg, "upperLayerOut");

        // increment data received count
        feedbackReceivedCount++;



    } else if (strcmp(gateName, "lowerLayerIn") == 0 && (neighListMsg = dynamic_cast<KNeighbourListMsg*>(msg)) != NULL) {
        // received neighbour list packet from lower layer, so send it to upper layer

        // send the packet
        send(neighListMsg, "upperLayerOut");


    } else if (msg->isSelfMessage() && msg->getKind() == 194) {
        // timer triggered for dumping stats

       // emit all stat dump signals
        emit(dataSendSignal, dataSentCount);
        emit(dataReceiveSignal, dataReceivedCount);
        emit(feedbackSendSignal, feedbackSentCount);
        emit(feedbackReceiveSignal, feedbackReceivedCount);

        // init stat collection variables again
        dataSentCount = 0;
        dataReceivedCount = 0;
        feedbackSentCount = 0;
        feedbackReceivedCount = 0;

        // set stat dump timer again
        scheduleAt(simTime() + 1.0, msg);

    } else {
        EV_FATAL << KLINKADAPTLAYER_SIMMODULEINFO << " Something is radically wrong !!! \n";
        delete msg;

    }

}

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

        logging = par("logging");

        // set other parameters
        broadcastMACAddress = "FF:FF:FF:FF:FF:FF";


    } else if (stage == 1) {
    } else if (stage == 2) {


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
    // KDataMsg *dataMsg;
    // KFeedbackMsg *feedbackMsg;
    // KNeighbourListMsg *neighListMsg;

    // get through what gate the
    gate = msg->getArrivalGate();
    strcpy(gateName, gate->getName());


    if (strcmp(gateName, "upperLayerIn") == 0) {
        // if pkt from upper layer, then send to the lower layer
        send(msg, "lowerLayerOut");

    } else if (strcmp(gateName, "lowerLayerIn") == 0) {
        // if pkt from lower layer, then send to the upper layer
        send(msg, "upperLayerOut");

    } else {
        EV_FATAL << KLINKADAPTLAYER_SIMMODULEINFO << " Something is radically wrong !!! \n";
        delete msg;

    }

    //
    // if (strcmp(gateName, "upperLayerIn") == 0 && (dataMsg = dynamic_cast<KDataMsg*>(msg)) != NULL) {
    //     // received data packet from upper layer, so send it to lower layer
    //
    //     // set values
    //     dataMsg->setSourceAddress(ownMACAddress.c_str());
    //
    //     // send the packet
    //     send(dataMsg, "lowerLayerOut");
    //
    //
    // } else if (strcmp(gateName, "upperLayerIn") == 0 && (feedbackMsg = dynamic_cast<KFeedbackMsg*>(msg)) != NULL) {
    //     // received feedback packet from upper layer, so send it out
    //
    //     // set values
    //     feedbackMsg->setSourceAddress(ownMACAddress.c_str());
    //
    //     // send the packet
    //     send(feedbackMsg, "lowerLayerOut");
    //
    // } else if (strcmp(gateName, "lowerLayerIn") == 0 && (dataMsg = dynamic_cast<KDataMsg*>(msg)) != NULL) {
    //     // received data packet from lower layer, so send it to upper layer
    //
    //     // send the packet
    //     send(dataMsg, "upperLayerOut");
    //
    // } else if (strcmp(gateName, "lowerLayerIn") == 0 && (feedbackMsg = dynamic_cast<KFeedbackMsg*>(msg)) != NULL) {
    //     // received feedback packet from lower layer, so send it to upper layer
    //
    //     // send the packet
    //     send(feedbackMsg, "upperLayerOut");
    //
    // } else if (strcmp(gateName, "lowerLayerIn") == 0 && (neighListMsg = dynamic_cast<KNeighbourListMsg*>(msg)) != NULL) {
    //     // received neighbour list packet from lower layer, so send it to upper layer
    //
    //     // send the packet
    //     send(neighListMsg, "upperLayerOut");
    //
    // } else {
    //     EV_FATAL << KLINKADAPTLAYER_SIMMODULEINFO << " Something is radically wrong !!! \n";
    //     delete msg;
    //
    // }

}

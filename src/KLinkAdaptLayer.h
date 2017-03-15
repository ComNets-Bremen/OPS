//
// The model implementation for link adaptations between
// the used link technology (below) and the upper layer.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 12-oct-2015
//
//

#ifndef KLINKADAPTLAYER_H_
#define KLINKADAPTLAYER_H_

#include <omnetpp.h>
#include <cstdlib>
#include <string>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;

class KLinkAdaptLayer: public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;

    private:
        string ownMACAddress;
        string broadcastMACAddress;

        // statistics generation signals
        simsignal_t dataSendSignal;
        simsignal_t dataReceiveSignal;
        simsignal_t feedbackSendSignal;
        simsignal_t feedbackReceiveSignal;

        // statistics collection variables
        int dataSentCount;
        int dataReceivedCount;
        int feedbackSentCount;
        int feedbackReceivedCount;



};

#define KLINKADAPTLAYER_SIMMODULEINFO       " :: " << simTime() << " :: " << getParentModule()->getFullName() << " :: KLinkAdaptLayer"

#endif /* KLINKADAPTLAYER_H_ */

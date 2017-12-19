//
// The model implementation for the Keetchi layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 12-oct-2015
//
//

#ifndef KKEETCHILAYER_H_
#define KKEETCHILAYER_H_

#include <omnetpp.h>
#include <cstdlib>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"
#include "KLKeetchi.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;

class KKeetchiLayer: public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();

    private:
        string ownMACAddress;
        double agingInterval;
        int maximumCacheSize;
        double neighbourhoodChangeSignificanceThreshold;
        double coolOffDuration;
        double learningConst;
        double backoffTimerIncrementFactor;
        int logging;

        KLKeetchi *keetchiAPI;
        void processUpperLayerInDataMsg(KDataMsg *dataMsg);
        void processUpperLayerInFeedbackMsg(KFeedbackMsg *feedbackMsg);
        void processLowerLayerInNeighbourListMsg(KNeighbourListMsg *neighListMsg);
        void processLowerLayerInDataMsg(KDataMsg *dataMsg);
        void processLowerLayerInFeedbackMsg(KFeedbackMsg *feedbackMsg);
        KLDataMsg* createKeetchiAPIDataMsgFromOMNETDataMsg(KDataMsg* omnetDataMsg, int msgDirection, int fromWhere, int toWhere);
        KLFeedbackMsg* createKeetchiAPIFeedbackMsgFromOMNETFeedbackMsg(KFeedbackMsg* omnetFeedbackMsg, int msgDirection, int fromWhere, int toWhere);
        KDataMsg* createOMNETDataMsgFromKeetchiAPIDataMsg(KLDataMsg* keetchiAPIDataMsg);
        KFeedbackMsg* createOMNETFeedbackMsgFromKeetchiAPIFeedbackMsg(KLFeedbackMsg* keetchiAPIFeedbackMsg);

        cMessage *ageDataTimeoutEvent;

        string broadcastMACAddress;

};
#define KKEETCHILAYER_SIMMODULEINFO       ">!<" << simTime() << ">!<" << getParentModule()->getFullName()

#endif /* KKEETCHILAYER_H_ */

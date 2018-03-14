//
// The model is the implementation of the Basic User Behavior Model application
// for Keetchi Simulator
//
// @date   : 26-06-2016
// @author : Anas bin Muslim (anas1@uni-bremen.de)
//
// Change History:
// Asanga Udugama (adu@comnets.uni-bremen.de)
// - Log entry includes model name
// - Unwanted code, comments cleanup

#ifndef KBASICUBMAPP_H_
#define KBASICUBMAPP_H_

#include <omnetpp.h>
#include "KOPSMsg_m.h"
#include "KUserBehavior.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif


using namespace std;

#define KBASICUBMAPP_SIMMODULEINFO  " KBasicUBMApp>!<" << simTime() << ">!<" << this->getParentModule()->getFullName()

class KBasicUBMApp : public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();
        
    private:
        int sameMsgCount;                     // Count of the same message which is received from multiple different nodes
                
        cMessage *appRegistrationEvent;       // Message for registering application at forwarding layer
        
        std::vector<std::string> notifications; // Buffer for notifications. UBM accesses this from time to time
        
        string appPrefix;                     // Prefix to be appended in the message names so the message will reach its specific app layer application 
        string appNameGenPrefix;              // Same as appPrefix but it is an application Name
        KDataMsg *neighborDataMsg;
        
        int logging;
};

#endif /* KBASICUBMAPP_H_ */

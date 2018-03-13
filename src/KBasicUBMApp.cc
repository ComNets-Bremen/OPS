//
// The model is the implementation of the Basic UBM application
// for Keetchi Simulator
//
// @date   : 26-06-2016
// @author : Anas bin Muslim (anas1@uni-bremen.de)
//
//

#include "KBasicUBMApp.h"

Define_Module(KBasicUBMApp);

void KBasicUBMApp::initialize(int stage)
{
    if (stage == 0) {

        // get parameters
        usedRNG = par("usedRNG");
        nodeIndex = par("nodeIndex");
        appPrefix = par("appPrefix").stringValue();
        appNameGenPrefix = par("appNameGenPrefix").stringValue();
        //expectedNodeTypes = par("expectedNodeTypes").stringValue();
        logging = par("logging");

    } else if (stage == 1) {

    } else if (stage == 2) {
		
        // create and setup app registration trigger
        appRegistrationEvent = new cMessage("App Registration Event");
        appRegistrationEvent->setKind(92);
        scheduleAt(simTime(), appRegistrationEvent);
		
    } else {
        EV << KBASICUBMAPP_SIMMODULEINFO << "Something is radically wrong\n";
    }

}

int KBasicUBMApp::numInitStages() const
{
    return 3;
}

void KBasicUBMApp::handleMessage(cMessage *msg)
{
	KDataMsg *dataMsg;
	KReactionMsg * reactionMsg;

	// Check if message is self message
	if (msg->isSelfMessage() && msg->getKind() == 92) {
				
		// send app registration message the keetchi layer
        KRegisterAppMsg *regAppMsg = new KRegisterAppMsg("Basic UBM App Registration");
        
        regAppMsg->setAppName("basicubm");
        regAppMsg->setPrefixName(appPrefix.c_str());
        
        send(regAppMsg, "lowerLayerOut");
        
        if (logging) {EV_INFO << KBASICUBMAPP_SIMMODULEINFO << ">!<GAR" << "\n";}
        
        //EV << KBASICUBMAPP_SIMMODULEINFO << "Generated App Registration" << "\n";
	}
	
	// If the message is not a self message
	else if (dynamic_cast<KDataMsg*>(msg) != NULL){
		
        dataMsg = check_and_cast<KDataMsg *>(msg);
        
		std::string mName = dataMsg->getName();
		std::string mDataName = dataMsg->getDataName();
		
		//If msg is from UBM layer of same node's UBM; send to lower layer
		if (dataMsg->getMsgType() == 21){

			bool found = false;
			string datNam = dataMsg->getDataName();

			for(int i=0;i<notifications.size();i++){
				if (datNam == notifications[i]){
					found = true;
				}
			}
			
			if (!found){
				notifications.push_back(datNam);
					
				KDataMsg *lowerLayerMsg = dataMsg->dup();
				
				lowerLayerMsg->setDataName(datNam.replace(datNam.begin(), datNam.begin(), appPrefix).c_str());
				lowerLayerMsg->setMsgType(31);
			
				send(lowerLayerMsg, "lowerLayerOut");
			
				if(strstr(getParentModule()->getFullName(), dataMsg->getFinalDestinationNodeName()) != NULL) {
					//EV_INFO << KBASICUBMAPP_SIMMODULEINFO << " :: Received Data :: " << dataMsg->getDataName() << " :: At Destination :: "
				 	//   << dataMsg->getFinalDestinationNodeName() << " \n";
				}
			}
		}
		
		//If msg is from another node; send to UBM layer
		else if (dataMsg->getMsgType() == 31){

			string datNam = dataMsg->getDataName();
			bool found = false;
			
			for(int i=0;i<notifications.size();i++){
				if (datNam == notifications[i]){
					found = true;
				}
			}
			if(!found){
				notifications.push_back(dataMsg->getDataName());
				
				//Send msg to UBM
				KDataMsg *upperLayerMsg;
				upperLayerMsg = dataMsg->dup();
				upperLayerMsg->setDataName(datNam.replace(datNam.begin(), datNam.begin()+10, "").c_str());
				send(upperLayerMsg, "upperLayerOut");
				
			}
			else{
				//std::cout<<KBASICUBMAPP_SIMMODULEINFO<<"MSG rcvd multiple times; DISCARDED"<<endl;
			}
		}
	}
	
	else if (dynamic_cast<KReactionMsg*>(msg) != NULL){
		reactionMsg = check_and_cast<KReactionMsg *>(msg);
		string reactionDataName = reactionMsg->getDataName();
		
		KReactionMsg *UBMReactionMsg = reactionMsg->dup();
		reactionMsg->setDataName(reactionDataName.replace(reactionDataName.begin()+9, reactionDataName.begin()+9, appPrefix).c_str());
		
		//std::cout << KBASICUBMAPP_SIMMODULEINFO << "Reaction msg sent to FWD layer :: " << reactionMsg->getDataName() <<endl;		

		send(UBMReactionMsg, "lowerLayerOut");
	}
	
    delete msg;
}

void KBasicUBMApp::finish()
{
	
}

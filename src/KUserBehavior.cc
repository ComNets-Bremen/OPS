//
// The model is the implementation of the User Behavior layer
// for OPS Simulator
//
// @date   : 13-07-2016
// @author : Anas bin Muslim (anas1@uni-bremen.de)
//
// Change History:
// Asanga Udugama (adu@comnets.uni-bremen.de)
// - Fixed file open check for properties.txt
// - Changed function parameters to IReactiveMobility

#include "KUserBehavior.h"

Define_Module(KUserBehavior);

void KUserBehavior::initialize(int stage)
{
    if (stage == 0) {
			
        // get parameters from ini/ned file
        
        //isResponder = par("isResponder").stringValue();	// TO BE IMPLEMENTED LATER (FUTURE WORKS)
        
        usedRNG = par("usedRNG");
        react = par("react");
        reactToEventThreshold = par("reactToEventThreshold");
        appPrefix = par("appPrefix").stringValue();
        appNameGenPrefix = par("appNameGenPrefix").stringValue();
        logging = par("logging");
        
        // setup stat collection signals
        //dataSendSignal = registerSignal("dataSendSignal");
        //dataReceiveSignal = registerSignal("dataReceiveSignal");
        //feedbackSendSignal = registerSignal("feedbackSendSignal");
        
        angryBitSignal = registerSignal("angryBitSignal");
        
    } else if (stage == 1) {
        std::string details;
        std::ifstream infile;
        
        /* select preference properties for the node
           randomly from properties text file  */
        bool keywordsFound = getKeywords();
        if (!keywordsFound) {
            EV << "Could not open (keywords) properties.txt file. create it." << std::endl;
            endSimulation();
        }
        
        // For Debugging
		/*std::cout<<KUSERBEHAVIOR_SIMMODULEINFO<<"** Properties :: ";
		for (int j=0; j<properties.size(); j++){
			std::cout<<properties[j]<<", ";
		}
		std::cout<<"** "<<endl;*/
        
    } else if (stage == 2) {
		
		// Calculate the reactions based on keyword matching
		computePreReactions();
		notificationCount = notifications.size();
		
		// For Debugging
		/*std::cout<<endl<<KUSERBEHAVIOR_SIMMODULEINFO<<"PRE-COMPUTING REACTIONS"<<endl;
		for (int i=0; i<notifications.size(); i++){
			std::cout<<"		EVENT KEYWORDS	 ::	";
			for (int j=0; j<notifications[i].properties.size(); j++){
				std::cout<<notifications[i].properties[j]<<", ";
			}
			std::cout<<endl;
			std::cout<<"		Start, End	 ::	"<<notifications[i].startTime<<", "<<notifications[i].endTime<<endl;
			std::cout<<"		Radius		 ::	"<<notifications[i].radius<<endl;
			std::cout<<"		Coordinates	 ::	"<<notifications[i].eventCoord.x<<","<<notifications[i].eventCoord.y<<","<<notifications[i].eventCoord.z<<endl;
			std::cout<<"		Popularity	 ::	"<<notifications[i].popularity<<endl;
			std::cout<<"		REACTION	 ::	"<<notifications[i].reaction<<endl<<endl;
		}*/
		
		int likeness = 0, goodness = 0;
		std::string eventLogName;
		
		 // dump the notification list with given popularity, likeness and goodness value
        if (logging) {EV_INFO << KUSERBEHAVIOR_SIMMODULEINFO << ">!<NLB>!<C>!<" << notificationCount << "\n";}
        for (int i = 0; i < notificationCount; i++) {
			likeness = goodness = 0;
			if(notifications[i].reaction == reactionType::save){
				likeness = 100;
			}
			if(likeness >= 90){
				goodness = 100;
			}
			
			eventLogName = "/basicubm-" + notifications[i].rawDescription;
			
			if (logging) {EV_INFO << KUSERBEHAVIOR_SIMMODULEINFO << ">!<NE>!<" << eventLogName << ">!<"
                << notifications[i].popularity << ">!<" << likeness << ">!<"
                << goodness << ">!<"
                << (goodness == 100 ? "L" : "I") << ">!<"
                << notifications[i].endTime << "\n";}
        }
        if (logging) {EV_INFO << KUSERBEHAVIOR_SIMMODULEINFO << ">!<NLE>!<C>!<" << notificationCount << "\n";}
		
		
    } else {
        EV << KUSERBEHAVIOR_SIMMODULEINFO << "Something is radically wrong\n";
    }
}

int KUserBehavior::numInitStages() const
{
    return 3;
}

void KUserBehavior::handleMessage(cMessage *msg)
{
	KDataMsg *dataMsg;
    std::string msgDataName;

	if (dynamic_cast<KDataMsg*>(msg) != NULL){
		
		///////////////////////////////////////////
		/* EXTRACTING EVENT INFORMATION FROM MSG */
		bool emergency = false;
		dataMsg = check_and_cast<KDataMsg *>(msg);
		
		std::string msgName = dataMsg->getName();
		std::string msgDataName = dataMsg->getDataName();
		
		/* Msg came from notif gen to this node; send to others and react
		 * OR 
		 * If Msg came from another node to this node; react
		 */
		
		if(dataMsg->getMsgType() == 11 || dataMsg->getMsgType() == 31){
			
			std::string payload = dataMsg->getDummyPayloadContent();
			std::string keyword, locAndTime;
			
			int indx;
			std::vector <int> locTime;
			event receivedEvent;
			
			for (indx=0; indx<strlen(payload.c_str()); indx++){
				if(isdigit(payload.at(indx))){
					break;
				}
			}
			
			std::string properties = payload.substr(0, indx);
			stringstream get(properties);
			get>>keyword;
			while(get){
				if (keyword == "emergency"){
					emergency = true;
					receivedEvent.emergencyEvent = true;
				}
				receivedEvent.properties.push_back(keyword);
				get>>keyword;
			}
			
			locAndTime = payload.substr(indx, strlen(payload.c_str()));
			stringstream tl(locAndTime);
			tl>>keyword;
			while(tl){
				if(keyword.find("m") != std::string::npos){
					receivedEvent.radius = stoi(keyword.substr(0, strlen(keyword.c_str())-1));
					locTime.push_back(stoi(keyword.substr(0, strlen(keyword.c_str())-1)));
				}
				else if (keyword.find("p") != std::string::npos){
					receivedEvent.popularity = stoi(keyword.substr(0, strlen(keyword.c_str())-1));
					locTime.push_back(stoi(keyword.substr(0, strlen(keyword.c_str())-1)));
				}
				else{
					locTime.push_back(stoi(keyword));
					
				}
				tl>>keyword;
			}
			
			if (emergency && (locTime.size() == 7)){
				receivedEvent.eventCoord.x = locTime[0];
				receivedEvent.eventCoord.y = locTime[1];
				receivedEvent.eventCoord.z = locTime[2];
				receivedEvent.radius = locTime[3];
				receivedEvent.startTime = locTime[4];
				receivedEvent.endTime = locTime[5];
				receivedEvent.popularity = locTime[6];
			}
			else if (!emergency && (locTime.size() >= 5)){
				receivedEvent.eventCoord.x = locTime[0];
				receivedEvent.eventCoord.y = locTime[1];
				receivedEvent.eventCoord.z = locTime[2];
				receivedEvent.startTime = locTime[locTime.size()-3];
				receivedEvent.endTime = locTime[locTime.size()-2];
				receivedEvent.popularity = locTime[locTime.size()-1];
			}			
			/* EVENT INFORMATION EXTRACTED FROM MSG */
			///////////////////////////////////////////
			
						
			///////////////////////////////////////////
			/*         Reaction computation          */
			receivedEvent = computeEventReaction(receivedEvent);
			///////////////////////////////////////////
			
			
			/* If Msg came from Notif Gen
			 * send to others and then react
			 */
			if(dataMsg->getMsgType() == 11){

				/////////////////////////////////////
				/*  FORWARDING MSG TO NEIGHBORS    */
				
				int itemNum = (rand() % 99999) + 1;
				
				std::string llMsgName = appNameGenPrefix + to_string(itemNum);
				std::string llMsgDataName = /*appNameGenPrefix + "/" + */dataMsg->getDataName();//to_string(itemNum);
								
				KDataMsg *lowerLayerMsg = new KDataMsg(llMsgName.c_str());
				
				lowerLayerMsg->setDataName(llMsgDataName.c_str());
				lowerLayerMsg->setMsgType(21);
				lowerLayerMsg->setSourceAddress("");
				lowerLayerMsg->setDestinationAddress("");
				lowerLayerMsg->setGoodnessValue(dataMsg->getGoodnessValue());
				lowerLayerMsg->setDummyPayloadContent(dataMsg->getDummyPayloadContent());
				lowerLayerMsg->setValidUntilTime(dataMsg->getValidUntilTime());
				lowerLayerMsg->setDestinationOriented(false);
				send(lowerLayerMsg, "lowerLayerOut");
				
                if (logging) {EV_INFO << KUSERBEHAVIOR_SIMMODULEINFO << ">!<GD>!<" << lowerLayerMsg->getDataName() << "\n";}


				//std::cout << KUSERBEHAVIOR_SIMMODULEINFO << " sent to others " << lowerLayerMsg->getDataName() << std::endl;
				
				/*      MSG SENT TO NEIGHBORS       */
				//////////////////////////////////////
				
								

				/* React to Msg only if it has all the values
				* i.e. 
				* 1. Coordinates
				* 2. Start time
				* 3. End time
				* 4. Popularity
				* 5. Radius (for emergency messages) */
				
				/////////////////////////
				// REACTING TO NORMAL MSG
				
				if(!receivedEvent.emergencyEvent){

					if(react == 1 && receivedEvent.reaction == reactionType::save){
						float diceThrow = uniform(0, 1, usedRNG);

						if(diceThrow > reactToEventThreshold){
					
							if(simTime().dbl() < receivedEvent.endTime){
								reactToEvent(receivedEvent);
								sendReactionMsg(receivedEvent.reaction, llMsgDataName);
							}
							else{
								/* Set angry bit here */
								emit(angryBitSignal, 1);
								sendReactionMsg(reactionType::ignore, llMsgDataName);
							}
						}
						else{
							sendReactionMsg(receivedEvent.reaction, llMsgDataName);
						}
					}
					else{
						sendReactionMsg(receivedEvent.reaction, llMsgDataName);
					}
				}
				///////////////////////////
				
				////////////////////////////
				// REACTING TO EMERGENCY MSG
				else{
					if(simTime().dbl() < receivedEvent.endTime){
						reactToEvent(receivedEvent);		
						sendReactionMsg(reactionType::save, llMsgDataName);
					}
					else{							
						sendReactionMsg(reactionType::ignore, llMsgDataName);
					}
				}
				//////////////////////////
			}
			
			/* If Msg came from one of the Neighbors
			 * only react to it accordingly
			 */
			else if(dataMsg->getMsgType() == 31){
				
		        if (logging) {EV_INFO << KUSERBEHAVIOR_SIMMODULEINFO << ">!<RD>!<" << dataMsg->getDataName() << "\n";}
			
				/////////////////////////
				// REACTING TO NORMAL MSG
				if(!receivedEvent.emergencyEvent){

					if(react == 1 && receivedEvent.reaction == reactionType::save){
						float diceThrow = uniform(0, 1, usedRNG);
					
						if(diceThrow > reactToEventThreshold){
					
							if(simTime().dbl() < receivedEvent.endTime){
								reactToEvent(receivedEvent);
							
								sendReactionMsg(receivedEvent.reaction, msgDataName);
							}
							else{
								/* Set angry bit here */
								emit(angryBitSignal, 1);
								sendReactionMsg(reactionType::ignore, msgDataName);
							}
						}
						else{
							sendReactionMsg(receivedEvent.reaction, msgDataName);
						}
					}
					else{
						sendReactionMsg(receivedEvent.reaction, msgDataName);
					}
				}
				///////////////////////////
				
				////////////////////////////
				// REACTING TO EMERGENCY MSG
				else{
					if(simTime().dbl() < receivedEvent.endTime){
						reactToEvent(receivedEvent);		
						sendReactionMsg(reactionType::save, msgDataName);
					}
					else{							
						sendReactionMsg(reactionType::ignore, msgDataName);
					}
				}
				//////////////////////////
			}
		}
		
		// Unexpected Message Received
		else{
			
			EV_INFO << KUSERBEHAVIOR_SIMMODULEINFO << ">!<Received unexpected packet \n";

			//EV << KUSERBEHAVIOR_SIMMODULEINFO << "Unexpected Message Received" << endl;
		}
	}
	
    delete msg;
}

void KUserBehavior::finish(){
	
}

void KUserBehavior::moveAway(){
	
}

bool KUserBehavior::computePreReactions(){

	//Reading all events from file and calculating the reactions
	
	int idx;
	std::string details, props;
	std::ifstream infile;
	
	//Reading events from file
	infile.open("events.txt",std::ios::in);
	if(!(infile.is_open())){
		EV<<"Something wrong with Events File"<<endl;
		return false;
	}
	while(!infile.eof()){
		event tmpEvent;
		std::getline(infile, details);
		
		tmpEvent.rawDescription = details;
				
		for(idx = 0; idx < strlen(details.c_str()); idx++){
			if(isdigit(details.at(idx))){
				break;
			}
		}
		
		props = details.substr(0, idx);

		char *pch;
		pch = strtok(const_cast<char*>(props.c_str()), " ");
		while(pch!=NULL){
			if(strcmp(pch, "emergency") == 0 && tmpEvent.emergencyEvent==false){
				tmpEvent.emergencyEvent = true;
			}
			tmpEvent.properties.push_back(pch);
			pch = strtok(NULL, " ");
		}
		
		// Compute all event values in struct and push it back on vector
		props = details.substr(idx, strlen(details.c_str()));
		
		bool popPresent = false;
		bool radPresent = false;
		bool timesPresent = false;
		bool coordPresent = false;
		
		int popIndx, radIndx, timesIndx, coordIndx;
		popIndx = radIndx = timesIndx = coordIndx = -1;
		
		std::vector <std::string> crdTimePop;
		
		pch = strtok(const_cast<char*>(props.c_str()), " ");
		while(pch!=NULL){
			crdTimePop.push_back(pch);
			pch = strtok(NULL, " ");
		}
		
		for (int i=0; i<crdTimePop.size(); i++){
			if(crdTimePop[i].find("m") != std::string::npos){
				radPresent = true;
				radIndx = i;
				tmpEvent.radius = std::stof(crdTimePop[radIndx].erase(crdTimePop[radIndx].find("m")));
			}
			else if(crdTimePop[i].find("p") != std::string::npos){
				popPresent = true;
				popIndx = i;
				if(tmpEvent.emergencyEvent==true){
					tmpEvent.popularity = 100;
				}
				else{
					tmpEvent.popularity = std::stof(crdTimePop[popIndx].erase(crdTimePop[popIndx].find("p")));
				}
			}
		}
		
		if(popPresent && popIndx == crdTimePop.size()-1){
			if(radPresent && radIndx == popIndx-3 && crdTimePop.size() == 7){
				timesPresent = true;
				timesIndx = popIndx-2;
				tmpEvent.startTime = std::stof(crdTimePop[timesIndx]);
				tmpEvent.endTime = std::stof(crdTimePop[timesIndx+1]);
				tmpEvent.eventCoord.x = std::stof(crdTimePop[0]);
				tmpEvent.eventCoord.y = std::stof(crdTimePop[1]);
				tmpEvent.eventCoord.z = std::stof(crdTimePop[2]);
			}
			else if(!radPresent && crdTimePop.size() == 6 && popIndx == 5){
				timesPresent = coordPresent = true;
				coordIndx = 0;
				timesIndx = 3;
				tmpEvent.startTime = std::stof(crdTimePop[timesIndx]);
				tmpEvent.endTime = std::stof(crdTimePop[timesIndx+1]);
				tmpEvent.eventCoord.x = std::stof(crdTimePop[0]);
				tmpEvent.eventCoord.y = std::stof(crdTimePop[1]);
				tmpEvent.eventCoord.z = std::stof(crdTimePop[2]);
			}
		}
		
		// for popularity present; compute reaction
		tmpEvent = computeEventReaction(tmpEvent);
		
		notifications.push_back(tmpEvent);
		
	} //while !eof loop	
	
	return true;
}

event KUserBehavior::computeEventReaction(event rEvent){
		
	int keysMatch = 0;
	float reactionVal;
	
	for(int i=0; i<rEvent.properties.size(); i++){
		for(int j=0; j<properties.size(); j++){
			if(rEvent.properties[i] == properties[j]){
				keysMatch++;
			}
		}
	}
	
	float lowerLimit = rEvent.popularity + (keysMatch*100)/rEvent.properties.size();
	
	if(lowerLimit <= 100){
		reactionVal = uniform(lowerLimit, 100, usedRNG);
	}
	else if (lowerLimit > 100){
		reactionVal = 100;
	}
	
	if(reactionVal > 0 && reactionVal < 90){
		rEvent.reaction = reactionType::ignore;
	}
	else if(reactionVal >= 90 && reactionVal < 99.5){
		rEvent.reaction = reactionType::comment;
	}
	else if(reactionVal >= 99.5){
		rEvent.reaction = reactionType::save;
	}
	
	return rEvent;
}

void KUserBehavior::reactToEvent(event rEvent){

	KDataMsg *eventMsg;
	
	if(rEvent.reaction == 3){
		if(!rEvent.emergencyEvent && rEvent.startTime < rEvent.endTime && rEvent.endTime > simTime().dbl()){			
			
			//Move to the location			
			std::string DName, DPLC;
			
			for(int i=0; i<rEvent.properties.size(); i++){
				DName = DName + rEvent.properties[i] + " ";
			}
			
			DPLC = to_string(rEvent.eventCoord.x)+ " " + to_string(rEvent.eventCoord.y) + " " + to_string(rEvent.eventCoord.z) + " " + to_string(rEvent.startTime);
			
			eventMsg = new KDataMsg("event");
			eventMsg->setDataName(DName.c_str());
			eventMsg->setSourceAddress("");
			eventMsg->setDestinationAddress("");
			eventMsg->setMsgType(0);
			eventMsg->setDummyPayloadContent(DPLC.c_str());
			eventMsg->setValidUntilTime(rEvent.endTime);
			
			cMessage * msg;
			msg = check_and_cast<cMessage *>(eventMsg);
			
			cModule *parent = this->getParentModule();
			inet::ExtendedSWIMMobility *mobilityModule = dynamic_cast <inet::ExtendedSWIMMobility*>(parent->getSubmodule("mobility"));
			mobilityModule->setNewTargetPosition(eventMsg->getDummyPayloadContent(), eventMsg->getMsgType(),
                                                eventMsg->getValidUntilTime(), eventMsg->getDataName());
		}
		
		else if(rEvent.emergencyEvent){
			
			// Move away from location
			std::string DName, DPLC;
			
			for(int i=0; i<rEvent.properties.size(); i++){
				DName = DName + rEvent.properties[i] + " ";
			}
			
			DPLC = to_string(rEvent.eventCoord.x)+ " " + to_string(rEvent.eventCoord.y) + " " + to_string(rEvent.eventCoord.z) + " " + to_string(rEvent.startTime) + " " + to_string(rEvent.radius);

			eventMsg = new KDataMsg("event");
			eventMsg->setDataName(DName.c_str());
			eventMsg->setSourceAddress("");
			eventMsg->setDestinationAddress("");
			eventMsg->setMsgType(1);
			eventMsg->setDummyPayloadContent(DPLC.c_str());
			eventMsg->setValidUntilTime(rEvent.endTime);
			
			cMessage * msg;
			msg = check_and_cast<cMessage *>(eventMsg);
			
			cModule *parent = this->getParentModule();
			inet::ExtendedSWIMMobility *mobilityModule = dynamic_cast <inet::ExtendedSWIMMobility*>(parent->getSubmodule("mobility"));
			mobilityModule->setNewTargetPosition(eventMsg->getDummyPayloadContent(), eventMsg->getMsgType(),
                                                eventMsg->getValidUntilTime(), eventMsg->getDataName());
			
		}
		
		delete eventMsg;	
	}

}

void KUserBehavior::sendReactionMsg(reactionType response, std::string eventName){
	KReactionMsg *reactionMsg = new KReactionMsg();
	reactionMsg->setSourceAddress("");
	reactionMsg->setDestinationAddress("");
	reactionMsg->setReaction(response);
	std::string reactionMsgDataName = "/reaction-" + eventName;
	reactionMsg->setDataName(reactionMsgDataName.c_str());

	send(reactionMsg, "lowerLayerOut");
	
	if (logging) {EV_INFO << KUSERBEHAVIOR_SIMMODULEINFO << ">!<GF>!<" << reactionMsg->getDataName() << ">!<T-Pre\n";}

	
}

bool KUserBehavior::getKeywords(){
	
	std::string keywords;
	std::ifstream infile;
	std::vector <std::string> keywordsList;
	
	infile.open("properties.txt", std::ios::in);
	if(!(infile.is_open())) {
        return false;
    }

    while(!infile.eof()) {
		std::getline(infile, keywords);
		if (keywords.empty()) {
			continue;
        }
        else{
			keywordsList.push_back(keywords);
		}
	}
	infile.close();
	int r = intuniform(0, keywordsList.size()-1, usedRNG);
	
	stringstream get(keywordsList[r]);
	get>>keywords;
	while(get){
		properties.push_back(keywords);
		get>>keywords;
	}
    
	return true;
}

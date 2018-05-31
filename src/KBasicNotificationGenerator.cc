//
// The model is the implementation of a basic Notification Generator
// for OPS Simulator
//
// @date   : 10-07-2016
// @author : Anas bin Muslim (anas1@uni-bremen.de)
//
// Change History:
// Asanga Udugama (adu@comnets.uni-bremen.de)
// - Fixed handleMessage() if statement where and was with one & (115 & 116 check)
// - Fixed file open check for locations.txt and events.txt
// - Unwanted code, comments cleanup
// - Locations and events files as parameters
// - use of interface
// - change of class name

/* Msg types::
 * 11  = Event msg to UBM
 * 112 = Send Event Msg
 * 113 = Create Emergency Indicator
 * 114 = Delete Emergency Indicator
 * 115 = Create Normal Event Indicator
 * 116 = Delete Normal Event Indicator
 */
 
#include "KBasicNotificationGenerator.h"

Define_Module(KBasicNotificationGenerator);

void KBasicNotificationGenerator::initialize(int stage)
{
    if (stage == 0) {
		
		/* Stage 0 Steps
		 * - Get the initialization file parameters
		 * - Count the number of locations in provided file
		 * - Save all the locaitons in a vector
		 */
		 
		int temp;
        std::ifstream infile;       // For reading a text file
        
        nodes = count = 0;
        
        // Getting parameter values from ini file
        usedRNG = par("usedRNG");
        radius = par("radius");
        interEventTimeDuration = par("interEventTimeDuration");
        notificationGenDistribution = par("notificationGenDistribution");
        appPrefix = par("appPrefix").stringValue();
        appNameGenPrefix = par("appNameGenPrefix").stringValue();
        logging = par("logging");
        locationsFilePath = par("locationsFilePath").stringValue();
        eventsFilePath = par("eventsFilePath").stringValue();
        dataSizeInBytes = par("dataSizeInBytes");

		// Counting the number of locations in text file
		std::vector <int> tempLoc;
		
		infile.open(locationsFilePath,std::ios::in);
    	if(!(infile.is_open())) {
            EV << " " << locationsFilePath << " not found. create the file manually.\n";
            endSimulation();
        }

		while (!infile.eof()){
			infile >> temp;
			tempLoc.push_back(temp);
			count ++;
		}
		infile.close();
		
		count = count / 4;  // count = number of locations in file
		
		// Saving the locations inside vector
		std::vector <int> tLoc;
		for (int i = 0;i < count; i++){
			tLoc.clear();
			tLoc.push_back(tempLoc[i*4 + 0]);
			tLoc.push_back(tempLoc[i*4 + 1]);
			tLoc.push_back(tempLoc[i*4 + 2]);
			locations.push_back(tLoc);
		}
		
    } else if (stage == 1) {
		
		/* Stage 1 Steps
		 * - Read notifications from events file
		 * - Create circles at all locations for identification
		 */
		 
		//Reading notifications from file
		eventsRead = readEvents();
        if (!eventsRead) {
            EV << " " << eventsFilePath << " not found. create the file.\n";
            endSimulation();
        }
		
		// Indicate locations with black circles
		std::string locationName;
		
		cCanvas *canvas = getParentModule()->getCanvas();
		cOvalFigure * circle[count];
		
		for(int i = 0; i < count; i++){
			locationName = std::to_string(locations[i][0]) + " " + std::to_string(locations[i][1]);
			
			circle[i] = new cOvalFigure(locationName.c_str());
			circle[i]->setBounds(cFigure::Rectangle(locations[i][0]-radius/2, locations[i][1]-radius/2, radius, radius));
			circle[i]->setLineWidth(3);
			circle[i]->setLineColor(cFigure::BLACK);
			
			canvas->addFigure(circle[i]);
			
		}

    } else if (stage == 2) {

		/* Stage 1 Steps
		 * - Counting number of nodes present in simulation
		 * - Setting up first event to send notification
		 */

		// Counting number of nodes present in simulation
		cSimulation *currentSimulation = getSimulation();
		int maxId = currentSimulation->getLastComponentId();

		for (int currentID = 0; currentID <= maxId; currentID++) {
			cModule *currentModule = currentSimulation->getModule(currentID);
			if (currentModule != NULL && dynamic_cast<IUBM*>(currentModule) != NULL){
				nodes++;
				nodesID.push_back(currentID);
			}
		}
		
        // Setup event to send notification
        simtime_t firstEventSendTime;
        
        cMessage *sendEvent = new cMessage("sendEvent");
        sendEvent->setKind(112);
        
        if (notificationGenDistribution == KBASICNOTIFICATIONGENERATOR_DISTR_EXPONENTIAL) {
			firstEventSendTime = simTime() + exponential(interEventTimeDuration, usedRNG);
		} else if (notificationGenDistribution == KBASICNOTIFICATIONGENERATOR_DISTR_UNIFORM) {
			firstEventSendTime = simTime().dbl() + uniform(0.0, (interEventTimeDuration * 2), usedRNG);
		} else {
			firstEventSendTime = simTime().dbl() + interEventTimeDuration;
		}
        
        scheduleAt(firstEventSendTime, sendEvent);
		
		if (logging) {EV_INFO << KBASICNOTIFICATIONGENERATOR_SIMMODULEINFO << ">!<SUTG "<< allEvents.size() << " DI" << "\n";}
        
    } else if (stage == 3){
		
	} else {
        EV << "Something is radically wrong\n";
    }
}

int KBasicNotificationGenerator::numInitStages() const
{
    return 3;
}

void KBasicNotificationGenerator::handleMessage(cMessage *msg)
{
	int rndm, idx;
	
	if(msg->isSelfMessage() && msg->getKind() == 113){
		
		// creating a circle on location
		float x, y, radi;
		std::string temp;
		 
		cCanvas *canvas = getParentModule()->getCanvas();
		cOvalFigure * circle = new cOvalFigure(msg->getName());
		stringstream crd(msg->getName());
		crd >> temp >> temp;
		x = stof(temp);
		crd >> temp;
		y = stof(temp);
		crd >> temp >> temp;
		temp = temp.substr(0, strlen(temp.c_str())-1);
		radi = stof(temp);
				
		circle->setBounds(cFigure::Rectangle(x-radi/2, y-radi/2, radi, radi));
        circle->setLineWidth(3);
        circle->setLineColor(cFigure::RED);
        canvas->addFigure(circle);
	}	
	
	else if (msg->isSelfMessage() && msg->getKind() == 114) {
		cCanvas * cnv = getParentModule()->getCanvas();
		cOvalFigure *cc = check_and_cast<cOvalFigure *>(cnv->getFigure(msg->getName()));
		cc->setLineColor(cFigure::BLACK);
	}
	
	else if (msg->isSelfMessage() && msg->getKind() == 115){
		float x, y, radi;
		std::string payload, temp;
		payload = msg->getName();
		for(idx = 0; idx < strlen(payload.c_str()); idx++){
			if(isdigit(payload.at(idx))){
				break;
			}
		}
		payload = payload.substr(idx, strlen(payload.c_str()));
		stringstream tokenizer(payload);
		
		tokenizer >> temp;
		x = stof(temp);
		tokenizer >> temp;
		y = stof(temp);
			
		cCanvas *canvas = getParentModule()->getCanvas();
		cOvalFigure * circle = new cOvalFigure(msg->getName());
		circle->setBounds(cFigure::Rectangle(x-10/2, y-10/2, 10, 10));
		circle->setLineWidth(3);
		circle->setLineColor(cFigure::GREEN);
		canvas->addFigure(circle);
	}
	
	else if (msg->isSelfMessage() && msg->getKind() == 116){
		cCanvas *canvas = getParentModule()->getCanvas();
		cOvalFigure *deleteFig = check_and_cast<cOvalFigure *>(canvas->getFigure(msg->getName()));
		deleteFig->setLineColor(cFigure::BLACK);
	}
	
	// Self message; Send event
	else if (msg->isSelfMessage() && msg->getKind() == 112) {
		
		if(eventsRead==false){
			EV_FATAL << KBASICNOTIFICATIONGENERATOR_SIMMODULEINFO << "Events cannot be read" << endl;
		}
		
		else if(eventsRead){
			int itemNum;
			
			double validtime = 0;
			bool emergency = false;
			
			std::vector <std::string> keywords;
			std::string properties, details, temp;
			
			char msgName[64], dataName[256], dummyPayloadContent[256] = {0};

			itemNum = (rand() % 99999) + 1;
			
			sprintf(msgName, "%s-%d", appPrefix.c_str(), itemNum);
				
			if(allEvents.size() > 0){
				
				rndm = intuniform(0,allEvents.size()-1, usedRNG);
				sprintf(dataName, "%s", allEvents[rndm].c_str());
				sprintf(dummyPayloadContent, "%s", allEvents[rndm].c_str());
				
				allEvents.erase(allEvents.begin()+rndm);
				
				//check whether notifications is emergency or regular; 
				//then extract details to be set in messages
				properties = dummyPayloadContent;
				for(idx = 0; idx < strlen(properties.c_str()); idx++){
					if(isdigit(properties.at(idx))){
						break;
					}
				}
				
				// Check for emergency event
				temp = properties.substr(0, idx);
				stringstream type(temp);
				while (type){
					type>>temp;
					if(strcmp(temp.c_str(), "emergency") == 0){
						emergency = true;
						break;
					}
				}
				
				details = properties.substr(idx,strlen(properties.c_str()));
				
				// Calculate the total time for which this msg is valid
				bool startTimePresent, endTimePresent, popularityPresent, radiusPresent;
				startTimePresent = endTimePresent = popularityPresent = radiusPresent = false;
				
				int startTimeIdx, endTimeIdx, popularityIdx, radiusIdx;
				startTimeIdx = endTimeIdx = popularityIdx = radiusIdx = -1;
				
				std::vector <string> tempvec;
				
				stringstream values(details);
				values >> details;
				while (values){
					tempvec.push_back(details);
					values>>details;
				}
				
				for (int i=0; i<tempvec.size(); i++){
					if (tempvec[i].find("m") != std::string::npos){
						radiusPresent = true;
						radiusIdx = i;
					}
					if (tempvec[i].find("p") != std::string::npos){
						popularityPresent = true;
						popularityIdx = i;
					}
				}
				
				if(popularityIdx - radiusIdx == 1){
					startTimePresent = true;
					startTimeIdx = radiusIdx + 1;
				}
				else if (tempvec.size() >= 6 && popularityIdx >= 5){
					startTimePresent = endTimePresent = true;
					startTimeIdx = popularityIdx - 2;
					endTimeIdx = popularityIdx - 1;
				}
				
				// if times are present in message details
				if(startTimePresent && endTimePresent){
					validtime = stof(tempvec[endTimeIdx]) - stof(tempvec[startTimeIdx]);
				}
				
				// if times are not present in msg details
				else{
					validtime = 0;
				}
				
				// Create a new message to be sent which contains event information	
				KDataMsg *eventMsg = new KDataMsg(msgName);
				eventMsg->setDataName(dataName);
				eventMsg->setSourceAddress("");
				eventMsg->setDestinationAddress("");
				
				if(popularityPresent)
					eventMsg->setGoodnessValue(stof(tempvec[popularityIdx].erase( tempvec[popularityIdx].find("p") )));
				
				eventMsg->setDummyPayloadContent(dummyPayloadContent);
				eventMsg->setValidUntilTime(validtime);
				eventMsg->setDestinationOriented(false);
				eventMsg->setMsgType(11);
                
        		eventMsg->setRealPayloadSize(dataSizeInBytes);
        		eventMsg->setByteLength(dataSizeInBytes);

				// Animations for emergency events
				if(emergency){
					if(stof(tempvec[endTimeIdx]) > simTime().dbl()){
						
						if(stof(tempvec[startTimeIdx]) > simTime().dbl()){
							cMessage * createEmergencyIndication = new cMessage(dummyPayloadContent);
							createEmergencyIndication->setKind(113);
							
							scheduleAt(stof(tempvec[startTimeIdx]), createEmergencyIndication);
						}
						
						else if(stof(tempvec[startTimeIdx]) <= simTime().dbl()){
							cMessage * createEmergencyIndication = new cMessage(dummyPayloadContent);
							createEmergencyIndication->setKind(113);
							
							scheduleAt(simTime().dbl(), createEmergencyIndication);
						}
					
        	        	cMessage * deleteEmergencyIndication = new cMessage(dummyPayloadContent);
    	            	deleteEmergencyIndication->setKind(114);
                		
                		scheduleAt(stof(tempvec[endTimeIdx]), deleteEmergencyIndication);
                	}
				}
				
				// Animations for normal events
				else{
					if(stof(tempvec[endTimeIdx]) > simTime().dbl()){
						
						if(stof(tempvec[startTimeIdx]) > simTime().dbl()){
							cMessage *createEventIndication = new cMessage(dummyPayloadContent);
							createEventIndication->setKind(115);
							
							scheduleAt(stof(tempvec[startTimeIdx]), createEventIndication);
						}
						
						else if(stof(tempvec[startTimeIdx]) <= simTime().dbl()){
							cMessage *createEventIndication = new cMessage(dummyPayloadContent);
							createEventIndication->setKind(115);
							
							scheduleAt(simTime().dbl(), createEventIndication);
						}
						
						cMessage * deleteEventIndication = new cMessage(dummyPayloadContent);
						deleteEventIndication->setKind(116);
						
						scheduleAt(stof(tempvec[endTimeIdx]), deleteEventIndication);
					}					
				}
				
				int randomNo = intuniform(0, nodesID.size()-1, usedRNG);
				
				// Get a random node to which the notification will be sent				
				cSimulation *currentSimulation = getSimulation();
				
				cModule *targetModule = currentSimulation->getModule(nodesID[randomNo])->getParentModule();
									
				// Send the message containing notification details
				cPacket *outPktCopy =  dynamic_cast<cPacket*>(eventMsg);
				sendDirect(outPktCopy, targetModule, "notificationGenIn");
				
				// Schedule another notification to be sent
				cMessage *sendEvent = new cMessage("sendEvent");
				sendEvent->setKind(112);
				
				double nextNotificationGenTime = 0.0;
				if (notificationGenDistribution == KBASICNOTIFICATIONGENERATOR_DISTR_EXPONENTIAL) {
					nextNotificationGenTime = simTime().dbl() + exponential(interEventTimeDuration, usedRNG);
				} else if (notificationGenDistribution == KBASICNOTIFICATIONGENERATOR_DISTR_UNIFORM) {
					nextNotificationGenTime = simTime().dbl() + uniform(0.0, (interEventTimeDuration * 2), usedRNG);
		        } else {
					nextNotificationGenTime = simTime().dbl() + interEventTimeDuration;
				}
				
				scheduleAt(nextNotificationGenTime, sendEvent);
			}
		} else {
			EV << KBASICNOTIFICATIONGENERATOR_SIMMODULEINFO << "Notifications cant be Generated; Verify the events file" <<endl;
			EV << KBASICNOTIFICATIONGENERATOR_SIMMODULEINFO << "           \\_('-')_/           " <<endl;
		}
		
		
	}
	delete msg;
}

//Read events from text file
bool KBasicNotificationGenerator::readEvents()
{
	int i=0;
	std::string line;
	std::ifstream infile;
	
	infile.open(eventsFilePath,std::ios::in);

	if(!(infile.is_open())) {
        return false;
    }
    
	while(!infile.eof()){
		std::getline(infile, line);
		if (strlen(line.c_str())>0)
			allEvents.push_back(line);
	}
	
	infile.close();
	return true;
}

void KBasicNotificationGenerator::finish()
{
	
}

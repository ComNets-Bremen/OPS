/******************************************************************************
 * ExtendedSWIMMobility - An Extended SWIM implementation for the INET Framework of the OMNeT++ 
 * Simulator.
 *
 * Copyright (C) 2016, Sustainable Communication Networks, University of Bremen, Germany
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 *
 *
 ******************************************************************************/

/**
 * The C++ implementation file of the SWIM mobility model for the INET Framework
 * in OMNeT++.
 *
 * @author : Anas bin Muslim (anas1@uni-bremen.de)
 *
 * Change History:
 * Asanga Udugama (adu@comnets.uni-bremen.de)
 * - Corrected the jumble of some files in OPS and others in INET
 * - All files now are copied into INET (like SWIMMobility files) before building INET
 * - file exists check for locations.txt
 */

#include <algorithm>

#include "ExtendedSWIMMobility.h"

namespace inet{

Define_Module(ExtendedSWIMMobility);

bool sortViaWeight(const nodeProp &a, const nodeProp &b) { return a.weight > b.weight; }

ExtendedSWIMMobility::ExtendedSWIMMobility()
{
    nextMoveIsWait = false;
    firstStep = true;
    homeCoordFound = false;
    stoppedForEvent = false;
}

void ExtendedSWIMMobility::initialize(int stage){

    LineSegmentsMobilityBase::initialize(stage);

    if(stage == 0){

        speed = par("speed");
        alpha = par("alpha");
        noOfLocs = par("noOfLocations");
        radius = par("radius");
        popularityDecisionThreshold = par("popularityDecisionThreshold");
        neighbourLocationLimit = par("neighbourLocationLimit");
        returnHomePercentage = par("returnHomePercentage");
        usedRNG = par("usedRNG");
        nodes = getParentModule()->getParentModule()->par("numNodes");
       
        maxAreaX = constraintAreaMax.x;
        maxAreaY = constraintAreaMax.y;
        maxAreaZ = constraintAreaMax.z;
       
        if(radius == 0) radius = 1;
        
       
        
        locations.resize(noOfLocs);

        int success = readLocations();
        if (!success) {
            EV << EXTENDEDSWIMMOBILITY_SIMMODULEINFO << " ending simulation - locations.txt not found, create manually " << endl;            
            endSimulation();
        }
                
        outfile.open("record.txt",std::ios::out|std::ios::app);
    }
    
}

void ExtendedSWIMMobility::setTargetPosition(){

    // initial position of a node is considered as the home
    // location
    
    //std::cout<<"inside settargetposition"<<std::endl;
    if (!homeCoordFound) {
        homeCoordFound = true;
        homeCoord = this->getCurrentPosition();
        neew = homeCoord;
        lastPosition = homeCoord;
        EV << EXTENDEDSWIMMOBILITY_SIMMODULEINFO << "Home :: " << homeCoord << endl;
    }

    // a nodes switches between moving and waiting
    // if the next  action is to wait, give the waiting time
    if(nextMoveIsWait){
		/*if(simTime() == nextChange){
			std::cout<< "is equal" <<std::endl;
		} else {
			std::cout<< "not equal" <<std::endl;
		}*/
		//std::cout << EXTENDEDSWIMMOBILITY_SIMMODULEINFO <<" At :: " << targetPosition << " :: Last Position :: " << lastPosition << std::endl;
		lastPosition = targetPosition;
        simtime_t waitTime = par("waitTime");
        
        nextChange = simTime() + waitTime.dbl();
        
        for(int i=0; i<otherEvents.size(); i++){
			if((this->getCurrentPosition() - otherEvents[i].eventLoc).length() < radius && \
			simTime().dbl() >= otherEvents[i].start && simTime().dbl() < otherEvents[i].end){
				waitTime = otherEvents[i].end - simTime().dbl();
				nextChange = otherEvents[i].end;
			}
			
		}
		
		/*std::cout << EXTENDEDSWIMMOBILITY_SIMMODULEINFO << "At :: " << lastPosition << " wait time :: " << waitTime << std::endl;*/
        outfile<<EXTENDEDSWIMMOBILITY_SIMMODULEINFO<<" "<<targetPosition<<" Wait : "<<waitTime.dbl() <<" Change at : " << simTime().dbl()+waitTime.dbl() <<endl;
        
        //std::cout << "Next change at :: " << nextChange << " :: Next move = wait ? :: " << !nextMoveIsWait <<std::endl;
        
        // if the next action is to start moving, compute the next location to move
    } else {
		if(!stoppedForEvent){
        double randomNum = uniform(0.0, 1.0, usedRNG);
        double returnHomeDecimalFraction = returnHomePercentage / 100.0;

        // randomly base to where the next move will be; home or
        // other location (neighboring or visiting), provided that
        // node is not already at home location
        if(randomNum < returnHomeDecimalFraction && ((lastPosition - homeCoord).length()>radius) ) {
            //std::cout<<"Next Destination is Home"<<endl;

            // select home location to move to
            targetPosition = homeCoord;
            neew = homeCoord;
            if(!firstStep) {
                updateAllNodes(false);
            }

            // compute next change time based on distance to the next
            // loation to move to and speed
            Coord positionDelta = targetPosition - lastPosition;
            double distance = positionDelta.length();
            nextChange = simTime().dbl() + distance/speed;
            
            //std::cout << EXTENDEDSWIMMOBILITY_SIMMODULEINFO <<"Going to home :: " << targetPosition << " :: Last Position :: " << lastPosition << std::endl;
            //std::cout << "Distance :: " << distance << " :: Next change at :: " << nextChange << " :: Next move = wait ? :: " << !nextMoveIsWait <<std::endl;

        } else {
            // select the neighbouring or visiting location to move to
            //std::cout<<"selecting new dest"<<std::endl;
            // compute the weights assignd to each node
            seperateAndUpdateWeights();

            // update the seen count (i.e., decrement nodes), but not at the begining
            // as locations have not seen any nodes yet
            if(!firstStep) {
                updateAllNodes(false);
            }

            // find the next location (position) to move to
            targetPosition = decision();
            
            EV << EXTENDEDSWIMMOBILITY_SIMMODULEINFO << "Target Position :: " << targetPosition << endl;
            //std::cout << EXTENDEDSWIMMOBILITY_SIMMODULEINFO << "Target Position :: " << targetPosition << endl;
            
            // compute next change time based on distance to the next
            // loation to move to and speed
            Coord positionDelta = targetPosition - lastPosition;

            double distance = positionDelta.length();
            nextChange = simTime() + distance/speed;

            // begin temp code
            // EV << simTime() << " :: SWIM :: not nextMoveIsWait other :: node id :: " << getId() << " :: target pos :: x pos :: " << targetPosition.x
            //     << " :: y pos :: " << targetPosition.y << " :: next change " << nextChange << "\n";
            // end temp code

            // update the seen count (i.e., increment nodes)
            updateAllNodes(true);
            
            //std::cout << EXTENDEDSWIMMOBILITY_SIMMODULEINFO <<"Going to :: " << targetPosition << " :: Last Position :: " << lastPosition << std::endl;
            //std::cout << "Distance :: " << distance << " :: Next change at :: " << nextChange << " :: Next move = wait ? :: " << !nextMoveIsWait <<std::endl;
            
        }
        
    outfile<<EXTENDEDSWIMMOBILITY_SIMMODULEINFO<<" "<<targetPosition<< " :: Arrival time :: " << nextChange <<endl;
    /*std::cout << EXTENDEDSWIMMOBILITY_SIMMODULEINFO << "destination :: " << targetPosition << std::endl;*/
    }
    else{
		int index = -1;
		for (int i=0; i<otherEvents.size(); i++){
			if(stoppedForEventName==otherEvents[i].eventname){
				index = i;
				break;
			}
		}
		if(index >= 0){
			
			neew = otherEvents[index].eventLoc;
			targetPosition = spreadInsideRadius(otherEvents[index].eventLoc);
			
			nextChange = simTime() + ((targetPosition - lastPosition).length())/speed;
			stoppedForEvent = false;
			updateAllNodes(true);
			
		}
		
		//std::cout << EXTENDEDSWIMMOBILITY_SIMMODULEINFO <<"Going to :: " << targetPosition << " :: Last Position :: " << lastPosition << std::endl;
        //std::cout << "Distance :: " << (targetPosition - lastPosition).length() << " :: Next change at :: " << nextChange << " :: Next move = wait ? :: " << !nextMoveIsWait <<std::endl;
	}
    }
    // indicate first time actions are all done
    firstStep = false;

    // next action is the opposite of current action
    nextMoveIsWait = !nextMoveIsWait;
}

void ExtendedSWIMMobility::move(){
	//std::cout<<EXTENDEDSWIMMOBILITY_SIMMODULEINFO<<"inside move :: " << simTime() <<" :: " <<nextChange <<std::endl;
	/*if(simTime()==nextChange){
		std::cout<<"inside move it is true at :: " << simTime() <<std::endl;
	}*/
    LineSegmentsMobilityBase::move();
    raiseErrorIfOutside();
}


void ExtendedSWIMMobility::handleSelfMessage(cMessage *message){
	
	// check otherEvents and emergencies vector to see if any event has scheduled flag as FALSE
	// then schedule that event; expiry time msg for emergency and movement for others
	
	if (emergencyReceived){
		//std::cout<<"handle msg: emergency rcvd flag true"<<std::endl;
	for(int i=0; i<emergencies.size(); i++){
		if(!emergencies[i].scheduled && simTime().dbl() < emergencies[i].end){
			if(simTime().dbl() < emergencies[i].start){
				
				//std::cout<<simTime().dbl() << " :: " << emergencies[i].start <<std::endl;
				
				std::string emergencyLocation = std::to_string(emergencies[i].eventLoc.x) + " " + std::to_string(emergencies[i].eventLoc.y) + " " + std::to_string(emergencies[i].eventLoc.z) + " " + "emergencyStarted";
				emergencyEventInterrupt = new cMessage(emergencyLocation.c_str());
				emergencyEventInterrupt->setKind(302);
				scheduleAt(emergencies[i].start, emergencyEventInterrupt);
			}
			eventExpiry = new cMessage("emergencyExpired");
			eventExpiry->setKind(304);
			scheduleAt(emergencies[i].end, eventExpiry);
			emergencies[i].scheduled = true;
		}
	}
	emergencyReceived = false;
	}
	
	
	if (eventReceived){
		//std::cout<<"handle msg: event rcvd flag true"<<std::endl;
	for(int i=0; i<otherEvents.size(); i++){
		if(!otherEvents[i].scheduled){
			
			Coord middlePointOfCanvas, startingPointOfCanvas;
			startingPointOfCanvas.x = 0;
			startingPointOfCanvas.y = 0;
			startingPointOfCanvas.z = 0;
			middlePointOfCanvas.x = maxAreaX/2;
			middlePointOfCanvas.y = maxAreaY/2;
			middlePointOfCanvas.z = maxAreaZ/2;
			double length = (middlePointOfCanvas - startingPointOfCanvas).length();
			simtime_t startMovingBefore = length/speed;
			moveToLoc = new cMessage(otherEvents[i].eventname.c_str());
			moveToLoc->setKind(301);
						
			if(otherEvents[i].end > simTime().dbl()){
				if( (otherEvents[i].start - startMovingBefore) > simTime() ){
					//Check if it works like this
					scheduleAt(otherEvents[i].start - startMovingBefore, moveToLoc);
					//scheduleAt(otherEvents[i].start, moveToLoc);
					otherEvents[i].scheduled = true;
				}
				else{
					scheduleAt(simTime(), moveToLoc);
					otherEvents[i].scheduled = true;
				}
			}
		}
	}
	eventReceived = false;
	}
	
	
	if(message->getKind() == 304){
		//std::cout<<"handle msg: delete emergency"<<std::endl;
		for (int i=0; i<emergencies.size(); i++){
			if((emergencies[i].end - simTime().dbl()) < 05){
				emergencies.erase(emergencies.begin()+i);
				i--;
			}
		}
		delete message; 
	}
	else if(message->getKind() == 301){
		//std::cout<<"handle msg: move to event location"<<std::endl;
		
		// Move to an event
		for(int i=0; i<otherEvents.size(); i++){
			if(message->getName()==otherEvents[i].eventname){
				//cancelEvent(moveTimer);
				lastPosition = this->getCurrentPosition();
				
				//targetPosition = otherEvents[i].eventLoc;
				targetPosition = lastPosition;
				stoppedForEvent = true;
				stoppedForEventName = otherEvents[i].eventname;
				updateAllNodes(false);
				
				Coord positionDelta = targetPosition - lastPosition;
            	double distance = positionDelta.length();
            	nextChange = simTime() + distance/speed;
								
				outfile<<EXTENDEDSWIMMOBILITY_SIMMODULEINFO<<targetPosition <<" :: Arrival Time (from 301) :: " << nextChange <<endl;
        		nextMoveIsWait = !nextMoveIsWait;
				
				eventExpiry = new cMessage(otherEvents[i].eventname.c_str());
				eventExpiry->setKind(303);
				scheduleAt(otherEvents[i].end, eventExpiry);
			}
		}
		delete message;
	}
	
	else if(message->getKind() == 302){
		//std::cout<<"handle msg: avoid emergency location"<<std::endl;
		std::string temp;
		std::stringstream location(message->getName());
		
		Coord emergencyCoord;
		int rad;
		double distance;
		
		location>>temp;
		emergencyCoord.x = std::stoi(temp);
		location>>temp;
		emergencyCoord.y = std::stoi(temp);
		location>>temp;
		emergencyCoord.z = std::stoi(temp);
		
		for(int i=0; i<emergencies.size(); i++){
			if(emergencies[i].eventLoc == emergencyCoord){
				rad = emergencies[i].radius;
			}
		}
		//if(((this->getCurrentPosition()) - emergencyCoord).length() < rad){
		if((targetPosition - emergencyCoord).length() < rad){
			updateAllNodes(false);
            seperateAndUpdateWeights();
            lastPosition = this->getCurrentPosition();
            targetPosition = decision();
            EV<<"Emergency Decision is to go to (from 302): "<<targetPosition<<endl;
            //std::cout<<EXTENDEDSWIMMOBILITY_SIMMODULEINFO<<"Emergency Decision is to go to : "<<targetPosition<<endl;
    	    distance = (targetPosition - lastPosition).length();
	        nextChange = simTime() + distance/speed;
	        
	        updateAllNodes(true);
        	
        	outfile<<EXTENDEDSWIMMOBILITY_SIMMODULEINFO<<targetPosition <<" :: Arrival Time :: " << nextChange <<endl;
        	
        	nextMoveIsWait = !nextMoveIsWait;

		}
		delete message;
	}
	
	else if(message->getKind() == 303){
		//std::cout<<"handle msg: delete normal event"<<std::endl;
		for(int i=0; i<otherEvents.size(); i++){
			if(message->getName() == otherEvents[i].eventname){
				otherEvents.erase(otherEvents.begin()+i);
				break;
			}
		}
		delete message;
	}
	
	if(nextChange<simTime()){nextChange=simTime();}
	
	moveAndUpdate();
    scheduleUpdate();
}


bool ExtendedSWIMMobility::createLocations(){

    bool opn = false;

    // open the file in output mode
    std::ofstream outfile;
    outfile.open("locations.txt",std::ios::out|std::ios::trunc);

    if(outfile.is_open())
        opn = 1;

    // create a set of random locations in the mobility area
    for(int i = 0; i < noOfLocs; i++) {

        // creation of locations assume the following based
        // on the mobility area
        // 1) constraint area must be > 0
        // 2) constraint area must be at least > 2 times the value
        //    defined in radius because locations are not created
        //    at the borders

        if (!(maxAreaX > (radius * 4.0) && maxAreaY > (radius * 4.0))) {
            EV << "SWIM :: Constraint area (maxAreaX or maxAreaY) is below 10 meters" << "\n";
            opn = 0;
            break;
        }

        // compute random x coord
        double coordElem = uniform((radius * 2.0), (maxAreaX - (radius * 2.0)), usedRNG);
        locations[i].myCoordX = (int) coordElem;

        // compute random y coord
        coordElem = uniform((radius * 2.0), (maxAreaY - (radius * 2.0)), usedRNG);
        locations[i].myCoordY = (int) coordElem;

        // z coord is always 0
        locations[i].myCoordZ = 0.0;

        locations[i].noOfNodesPresent = 0;

        // write to file
        outfile << locations[i].myCoordX << " " << locations[i].myCoordY << " " << locations[i].myCoordZ
                << " " << locations[i].noOfNodesPresent << endl;
    }

    outfile.close();
    return opn;
}

bool ExtendedSWIMMobility::readLocations(){

    // open location file to read
    std::ifstream infile;
    infile.open("locations.txt", std::ios::in);

    // if coudn't open, then problem
    if(!(infile.is_open())) {
        return 0;
    }

    // read all the values into locations array
    for(int i=0;i<noOfLocs;i++){
        infile >> locations[i].myCoordX;
        infile >> locations[i].myCoordY;
        infile >> locations[i].myCoordZ;
        infile >> locations[i].noOfNodesPresent;
    }

    // close file
    infile.close();

    return 1;
}

void ExtendedSWIMMobility::seperateAndUpdateWeights(){

    Coord temp;
    int noOfNeighbors = 0, n = 0, v = 0;

    // compute the maximum possible weight (to normalize)
    double maxWeight = alpha * ( sqrt( pow(maxAreaX, 2.0) + pow(maxAreaY, 2.0) + pow(maxAreaZ, 2.0)) ) + (1.0 - alpha) * nodes;

    // compute how many neighboring locations exist
    for(int i = 0; i < noOfLocs; i++) {
        temp.x = locations[i].myCoordX;
        temp.y = locations[i].myCoordY;
        temp.z = locations[i].myCoordZ;

        if(temp.distance(homeCoord) <= neighbourLocationLimit){
            noOfNeighbors++;
        }
    }

    // adjust arrays to hold the neighboring and visiting locations
    neighborLocs.resize(noOfNeighbors);
    visitingLocs.resize(noOfLocs-noOfNeighbors);

    // separate the locations into neighboring and visiting locations
    // and compute the weight assigned to each location
    for(int i=0;i<noOfLocs;i++){
        temp.x = locations[i].myCoordX;
        temp.y = locations[i].myCoordY;
        temp.z = locations[i].myCoordZ;

        if(temp.distance(homeCoord) <= neighbourLocationLimit) {
            neighborLocs[n].locCoordX = locations[i].myCoordX;
            neighborLocs[n].locCoordY = locations[i].myCoordY;
            neighborLocs[n].locCoordZ = locations[i].myCoordZ;
            neighborLocs[n].seen = locations[i].noOfNodesPresent;
            neighborLocs[n].weight = (alpha * (temp.distance(homeCoord)) + (1.0 - alpha) * neighborLocs[n].seen);
            neighborLocs[n].weight = neighborLocs[n].weight / maxWeight;
            n++;
        } else {
            visitingLocs[v].locCoordX = locations[i].myCoordX;
            visitingLocs[v].locCoordY = locations[i].myCoordY;
            visitingLocs[v].locCoordZ = locations[i].myCoordZ;
            visitingLocs[v].seen = locations[i].noOfNodesPresent;
            visitingLocs[v].weight = (alpha * (temp.distance(homeCoord)) + (1.0 - alpha) * visitingLocs[v].seen);
            visitingLocs[v].weight = visitingLocs[v].weight / maxWeight;
            v++;
        }
    }
}

// make the decision of which location to go to next
Coord ExtendedSWIMMobility::decision(){
	
	//std::cout<<"inside decision"<<endl;
    Coord dest;
    nodeProp temp;

    // order the neighbor & visiting locations lists by the weight
    sort(neighborLocs.begin(), neighborLocs.end(), sortViaWeight);
    sort(visitingLocs.begin(), visitingLocs.end(), sortViaWeight);

    // the procedure of identifying the next location is as follows
    // 1) decide randomly what type of location to go to next (neighboring or visiting)
    //    using the alpha parameter
    //    if alpha is lower, more likely to choose a visisting location
    //    if alpha is larger, more likely to choose a neighbouring location
    // 2) call chooseDestination() to select the destination to go to
    // 3) if there are no locations selected by chooseDestination() based on
    //    the given list (i.e., neighboring or visiting), check in the other
    //    list (i.e., visiting or neighbouring)

    // get random number between 0 & 1 (included)
    double randomNumber = uniform(0.0, 1.0, usedRNG);
    if (randomNumber <= alpha) {

        // if random number is lower than alpha, choose a neighbor location as
        // next destination
        dest = chooseDestination(neighborLocs);
        
        // must be a valid destination
        if(dest.x > 0 && dest.y > 0 && dest.z >= 0) {
            return dest;

            // if there was a invalid destination, choose visiting location
            // as next destination (NL 0 0 0)
        } else {
            return chooseDestination(visitingLocs);
        }

    } else {

        // if random number is lower than alpha, choose a visiting location as
        // next destination
        dest  = chooseDestination(visitingLocs);
        if (dest.x > 0.0 && dest.y > 0.0 && dest.z >= 0.0) {
            return dest;

            // if there was a invalid destination, choose neighbor location
            // as next destination (VL 0 0 0)
        } else {
            return chooseDestination(neighborLocs);
        }
    }
}

// select a destination randomly from the given aray (i.e., neighboring
// locations array or visiting locations array)
Coord ExtendedSWIMMobility::chooseDestination(std::vector<nodeProp> &array){
	
	//std::cout<<"inside choose dest"<<endl;
    int size = array.size();
    int randomNum = 0;
    int popular = 0;
    int notPopular = 0;
    int count, iterate = 0;

    Coord temp;
    Coord target;
    double weight = 0;

    // if array has no elements, then return
    if (size == 0) {
        temp.x = -1;
        temp.y = -1;
        temp.z = -1;
        return temp;
    }

    // count the total number of popular locations based on
    // previously computed weight values
    for(int i = 0; i < size; i++){
        if(array[i].weight > 0.75) {
            popular++;
        }
    }

    // compute the not-popular locations
    notPopular = size - popular;
    
    EV<<"Popular locs :: "<<popular<<endl;
    EV<<"Not Popular locs :: "<<notPopular<<endl;
    
    // choose a destination from the given array in the following manner
    // 1) obtain a random number and check if a popular or not popular
    //    item is selected (i.e., popularityDecisionThreshold)
    // 2) if popular item to be selected and the popular range has items,
    //    select an item in the popular range, randomly
    // 3) if not popular to be selected and the not popular range has items,
    //    select an item in the not popular range, randomly
    // 4) if none of the above, select an item from the whole array,
    //    randomly
    
    do{
        iterate ++ ;
        count = 0;

        randomNum = intuniform(0, 10, usedRNG);
        if(popular > 0 && randomNum > (10 - popularityDecisionThreshold)) {
            EV<<"Selecting popular location"<<endl;
            randomNum = intuniform(0, (popular - 1), usedRNG);
            temp.x = array[randomNum].locCoordX;
            temp.y = array[randomNum].locCoordY;
            temp.z = array[randomNum].locCoordZ;
        } else if (notPopular > 0) {
			EV<<"Selecting not popular location"<<endl;
            randomNum = intuniform(0, (notPopular - 1), usedRNG);
            temp.x = array[popular + randomNum].locCoordX;
            temp.y = array[popular + randomNum].locCoordY;
            temp.z = array[popular + randomNum].locCoordZ;
        } else {
			EV<<"Selecting any random location :: "<<endl;
            randomNum = intuniform(0, (size - 1), usedRNG);
            temp.x = array[randomNum].locCoordX;
            temp.y = array[randomNum].locCoordY;
            temp.z = array[randomNum].locCoordZ;
        }
        
        double arrivalTime = simTime().dbl() + ((temp - lastPosition).length())/speed;
        for(int i=0;i<emergencies.size();i++){
            if((temp-emergencies[i].eventLoc).length() < emergencies[i].radius && (arrivalTime>emergencies[i].start && arrivalTime<emergencies[i].end)){
                count++;
            }
        }
        if(iterate > 10){
            temp = homeCoord;
            break;
        }
    }while(count > 0);
    
    
    EV << EXTENDEDSWIMMOBILITY_SIMMODULEINFO << "Selected Locations :: " << temp <<endl;

    /*
    double u, v, w, t, x, y;

    // find a position within the radius given from the selected location
    // to move to
    // REASON: don't want all the nodes to pile up at the center of the
    // location
    u = uniform(0, 1, usedRNG);
    v = uniform(0, 1, usedRNG);
    w = radius * sqrt(u);
    t = 2 * PI * v;
    x = w * cos(t);
    y = w * sin(t);

    target.x = temp.x + x;
    target.y = temp.y + y;
    target.z = temp.z;
    */
    
    target = spreadInsideRadius(temp);
    
    // temporary code to place node at the center of the location
    // and not to spread inside the radius.
    //target = temp;

    // save position for node count computation
    neew = temp;

    return target;
}

int ExtendedSWIMMobility::updateNodesCount(Coord update,bool inc){

    bool inc_success = false;
    bool dec_success = false;

    Coord temp3;

    // increment or decrement node counts based on the flag
    for(int i = 0; i < noOfLocs; i++){
        temp3.x = locations[i].myCoordX;
        temp3.y = locations[i].myCoordY;
        temp3.z = locations[i].myCoordZ;
        if(inc == false && update == temp3) {
            if(locations[i].noOfNodesPresent > 0) {
                locations[i].noOfNodesPresent--;
                dec_success = true;
            }
        }
        if(inc == true && update == temp3){
            locations[i].noOfNodesPresent++;
            inc_success = true;
        }
    }

    if(inc_success) {
        return 1;
    } else if(dec_success) {
        return 2;
    } else {
        return 0;
    }
}

void ExtendedSWIMMobility::updateAllNodes(bool increase){

    int count = 0;

    // get mobility modules of all nodes to compute node count
    cSimulation *currentSimulation = getSimulation();
    int maxID = currentSimulation->getLastComponentId();
    for (int currentID = 0; currentID <= maxID; currentID++) {
        cModule *currentModule = currentSimulation->getModule(currentID);
        ExtendedSWIMMobility *mobile = dynamic_cast<ExtendedSWIMMobility*>(currentModule);
        if (currentModule != NULL && mobile != NULL) {
            mobile->updateNodesCount(neew, increase);
            count++;
        }
    }
}

Coord ExtendedSWIMMobility::spreadInsideRadius(Coord location){
	double u, v, w, t, x, y;
	Coord newLocation;
    // find a position within the radius given from the selected location
    // to move to
    // REASON: don't want all the nodes to pile up at the center of the
    // location
    u = uniform(0, 1, usedRNG);
    v = uniform(0, 1, usedRNG);
    w = radius * sqrt(u);
    t = 2 * PI * v;
    x = w * cos(t);
    y = w * sin(t);

    newLocation.x = location.x + x;
    newLocation.y = location.y + y;
    newLocation.z = location.z;
    
    return newLocation;
}

void ExtendedSWIMMobility::setNewTargetPosition(std::string dataName, int msgType, double validUntilTime, 
                                                std::string eventName) {
		
    // KDataMsg * dataMsg;
    // dataMsg = check_and_cast<KDataMsg *>(msg);

	
	bool found = false;
	double distance;
	
	event tempEvent;
	
    // std::string dataName = dataMsg->getDummyPayloadContent();
	std::string temp;
	
	std::stringstream loccoord(dataName);
	
	loccoord>>temp;
	tempEvent.eventLoc.x = std::stoi(temp);
	loccoord>>temp;
	tempEvent.eventLoc.y = std::stoi(temp);	
	loccoord>>temp;
	tempEvent.eventLoc.z = std::stoi(temp);
	loccoord>>temp;
	tempEvent.start = std::stof(temp);
	//if(dataMsg->getMsgType() == 1){
	if(msgType == 1){
		loccoord>>temp;
		tempEvent.radius = std::stoi(temp);
	}	
    // tempEvent.end = dataMsg->getValidUntilTime();
    // tempEvent.eventname = dataMsg->getDataName();
	tempEvent.end = validUntilTime;
	tempEvent.eventname = eventName;
	
	if(tempEvent.radius == -1){
		// normal event. Schedule a move from start till end time.
		for (int i=0; i<emergencies.size(); i++){
			if((tempEvent.eventLoc-emergencies[i].eventLoc).length() <= emergencies[i].radius){
				if( (tempEvent.start >= emergencies[i].start && tempEvent.start <= emergencies[i].end) || \
				(tempEvent.end >= emergencies[i].start && tempEvent.end <= emergencies[i].end) ){
					found = true;
					break;
				}
			}
		}
		
		/*if no emergency event is taking place at same location at same time
		  then push event in list and schedule the mobility event  */
		if(!found){
			eventReceived = true;
			otherEvents.push_back(tempEvent);
		}
	}
	else if(tempEvent.radius > 0){
		
		// emergency event. Save the event details. Move away.
		emergencies.push_back(tempEvent);
		emergencyReceived = true;
		
		if(neew == tempEvent.eventLoc || (tempEvent.eventLoc-neew).length() < radius){
			if( ((simTime().dbl() >= tempEvent.start) && (simTime().dbl() <= tempEvent.end)) || \
			(nextChange >= tempEvent.start && nextChange < tempEvent.end) ){
        	    //std::cout<<"Going to Emergency event Coordinates: Changing direction now"<<simTime().dbl()<<tempEvent.start<<tempEvent.end<<endl;
            	//lastPosition = neew;
            	
            	updateAllNodes(false);
            	seperateAndUpdateWeights();
            	lastPosition = this->getCurrentPosition();
            	targetPosition = lastPosition;
            	
            	//targetPosition = decision();
            	EV<<"Emergency Decision is to go to (event rcvd emergency) : Stopping mobility "<<targetPosition<<endl;
        	    
        	    Coord positionDelta = targetPosition - lastPosition;
    	        distance = positionDelta.length();
	            nextChange = simTime() + distance/speed;
            	//updateAllNodes(true);
            	nextMoveIsWait = !nextMoveIsWait;
            	
        	    //std::cout<<EXTENDEDSWIMMOBILITY_SIMMODULEINFO<<"New Target position is :: "<<neew<<" :: "<<targetPosition<<" :: & event is at : "<<tempEvent.eventLoc<<endl;

			}
        }
	}
}

ExtendedSWIMMobility::~ExtendedSWIMMobility(){
	outfile.close();
}

} // namespace inet

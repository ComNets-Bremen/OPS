/******************************************************************************
 * SWIMMobility - A SWIM implementation for the INET Framework of the OMNeT++
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
 * - All random numbers are obtained from OMNeT++ RNG functions instead of
 *   the system RNG.
 * - Code commented
 * - Home coordinates found in setTargetPosition()
 * - Bug fixes
 * Anna Förster (anna.foerster@comnets.uni-bremen.de)
 * - handling for automated simulation runs for multiple seeds
 * Matthias Lehmann (matthias.lehmann@tuhh.de)
 * - regeneration of locations when location count changes
 * Asanga Udugama (adu@comnets.uni-bremen.de)
 * - Introduced a parameter to decide the creation of the locations file
 */

#include <algorithm>

#include "inet/mobility/single/SWIMMobility.h"

namespace inet{

Define_Module(SWIMMobility);

bool sortByWeight(const nodeProp &a, const nodeProp &b) { return (a.weight > b.weight); }
bool locationsCreated = 0;

SWIMMobility::SWIMMobility()
{
    nextMoveIsWait = false;
    created = false;
    firstStep = true;
    count = 0;
    homeCoordFound = false;
}

void SWIMMobility::finish()
{
    locationsCreated = 0;
}

void SWIMMobility::initialize(int stage)
{

    // call initialize of base class to perform all the required
    // actions related to line segments mobility pattern
    LineSegmentsMobilityBase::initialize(stage);

    if(stage == 0){

        // read all the parameters
        speed = par("speed");
        alpha = par("alpha");
        noOfLocs = par("noOfLocations");
        radius = par("radius");
        popularityDecisionThreshold = par("popularityDecisionThreshold");
        neighbourLocationLimit = par("neighbourLocationLimit");
        returnHomePercentage = par("returnHomePercentage");
        usedRNG = par("usedRNG");
        nodes = par("Hosts");
        dimensions = par("dimensions");
        recreateLocationsFile = par("recreateLocationsFile");

        maxAreaX = constraintAreaMax.x;
        maxAreaY = constraintAreaMax.y;
        maxAreaZ = constraintAreaMax.z;
        nextMoveIsWait = false;
        created = false;
        firstStep = true;
        count = 0;
        homeCoordFound = false;

        // radius should never be zero
        if(radius == 0) {
            radius = 1;
        }

        // extend array to hold all the locations
        locations.resize((noOfLocs));
        
        // if recreateLocationsFile true and file exists, remove it
        if (!locationsCreated && recreateLocationsFile) {
            std::ifstream infile(LOCATIONS_FILE);
            if(infile) {
                infile.close();
                remove(LOCATIONS_FILE);
            }
            
        } else {

            // check if file already exists and if so, the location count is right
            std::string line;
            int i = 0;
            std::ifstream infile(LOCATIONS_FILE);
            if(infile) {
                // read number of lines in file
                while (getline(infile, line)) {
                    i++;
                }
                // if number of line in file equals or is greater than noOfLocs
                // use current file otherwise create new
                if(noOfLocs <= i) {
                    locationsCreated = 1;
                }
                infile.close();
            }
        }
    
        // one of the nodes creates the locations.txt file which
        // will be used by all nodes.
        if(!locationsCreated) {
            createLocations();
            locationsCreated = 1;
        }
    }
}

void SWIMMobility::setTargetPosition()
{

    // initial position of a node is considered as the home
    // location
    if (!homeCoordFound) {
        homeCoordFound = true;
        homeCoord = this->getCurrentPosition();
        neew = homeCoord;
        lastPosition = homeCoord;
    }

    // a nodes switches between moving and waiting
    // if the next  action is to wait, give the waiting time
    if(nextMoveIsWait){
        simtime_t waitTime = par("waitTime");
        nextChange = simTime() + waitTime.dbl();

        // begin temp code
        // EV << simTime() << " :: SWIM :: nextMoveIsWait wait :: node id :: " << getId()
        //     << " :: target pos :: x pos :: 0 :: y pos :: 0 :: next change " << nextChange << "\n";
        // end temp code

        // if the next action is to start moving, compute the next location to move
    } else {
        double randomNum = uniform(0.0, 1.0, usedRNG);
        double returnHomeDecimalFraction = returnHomePercentage / 100.0;

        // randomly base to where the next move will be; home or
        // other location (neighboring or visiting), provided that
        // node is not already at home location
        if(randomNum < returnHomeDecimalFraction && lastPosition != homeCoord) {

            if(!firstStep) {
                updateAllNodes(false);
            }

            // select home location to move to
            targetPosition = homeCoord;

            // compute next change time based on distance to the next
            // loation to move to and speed
            Coord positionDelta = targetPosition - lastPosition;
            double distance = positionDelta.length();
            nextChange = simTime() + distance/speed;

            // begin temp code
            // EV << simTime() << " :: SWIM :: not nextMoveIsWait home :: node id :: " << getId() << " :: target pos :: x pos :: " << targetPosition.x
            //     << " :: y pos :: " << targetPosition.y << " :: next change " << nextChange << "\n";
            // end temp code


        } else {

            // select the neighbouring or visiting location to move to

            // read the locations from the locations.txt, but only
            // once (firstStep)
            if(firstStep){
                readLocations();
            }

            // compute the weights assignd to each node
            seperateAndUpdateWeights();

            // update the seen count (i.e., decrement nodes), but not at the begining
            // as locations have not seen any nodes yet
            if(!firstStep) {
                updateAllNodes(false);
            }

            // find the next location (position) to move to
            targetPosition = decision();

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
        }
    }

    // indicate first time actions are all done
    firstStep = false;

    // next action is the opposit of current action
    nextMoveIsWait = !nextMoveIsWait;
}

void SWIMMobility::move()
{
    // this method is called every time the node moves
    LineSegmentsMobilityBase::move();
    raiseErrorIfOutside();
}

bool SWIMMobility::createLocations(){
    bool opn = false;

    // open the file in output mode
    std::ofstream outfile;
    outfile.open(LOCATIONS_FILE, std::ios::out|std::ios::trunc);

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
        // locations[i].myCoordX = (double) intuniform(0, ((int)round(maxAreaX) - 10), usedRNG);
        double coordElem = uniform((radius * 2.0), (maxAreaX - (radius * 2.0)), usedRNG);
        locations[i].myCoordX = (int) coordElem;

        // compute random y coord
        // locations[i].myCoordY = (double) intuniform(0, ((int)round(maxAreaY) - 10), usedRNG);
        coordElem = uniform((radius * 2.0), (maxAreaY - (radius * 2.0)), usedRNG);
        locations[i].myCoordY = (int) coordElem;

        // z coord is always 0
        if(dimensions == 3){
            coordElem = uniform((radius * 2.0), (maxAreaZ - (radius * 2.0)), usedRNG);
            locations[i].myCoordZ = (int) coordElem;
        } else {
            locations[i].myCoordZ = 0.0;
        }

        locations[i].noOfNodesPresent = 0;

        // write to file
        outfile << locations[i].myCoordX << " " << locations[i].myCoordY << " " << locations[i].myCoordZ
                << " " << locations[i].noOfNodesPresent << endl;
    }

    outfile.close();
    return opn;
}

bool SWIMMobility::readLocations()
{

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

    return true;
}

void SWIMMobility::seperateAndUpdateWeights()
{
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
    neighborLocs.resize((noOfNeighbors));
    visitingLocs.resize((noOfLocs - noOfNeighbors));


    // separate the locations into neighboring and visiting locations
    // and compute the weight assigned to each location
    for(int i = 0; i < noOfLocs; i++) {
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
Coord SWIMMobility::decision()
{
    Coord dest;
    nodeProp temp;

    // order the neighbor & visiting locations lists by the weight
    sort(neighborLocs.begin(), neighborLocs.end(), sortByWeight);
    sort(visitingLocs.begin(), visitingLocs.end(), sortByWeight);

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

        // if random number is lowr than alpha, choose a neighbor location as
        // next destination
        dest = chooseDestination(neighborLocs);

        // must be a valid destination
        if(!(dest.x <= 0 && dest.y <= 0 && dest.z <= 0)) {
            return dest;

            // if there was a invalid destination, choose visiting location
            // as next destination (NL 0 0 0)
        } else {
            return chooseDestination(visitingLocs);
        }

    } else {

        // if random number is lowr than alpha, choose a visiting location as
        // next destination
        dest  = chooseDestination(visitingLocs);
        if (!(dest.x <= 0.0 && dest.y <= 0.0 && dest.z <= 0.0)) {
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
Coord SWIMMobility::chooseDestination(std::vector<nodeProp> &array)
{
    int size = array.size();
    int randomNum = 0;
    int popular = 0;
    int notPopular = 0;

    Coord temp;
    Coord target;

    // if array has no elements, then return
    if (size == 0) {
        temp.x = 0.0;
        temp.y = 0.0;
        temp.z = 0.0;
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

    // choose a destination from the given array in the following manner
    // 1) obtain a random number and check if a popular or not popular
    //    item is selected (i.e., popularityDecisionThreshold)
    // 2) if popular item to be selected and the popular range has items,
    //    select an item in the popular range, randomly
    // 3) if not popular to be selected and the not popular range has items,
    //    select an item in the not popular range, randomly
    // 4) if none of the above, select an item from the whole array,
    //    randomly

    randomNum = intuniform(0, 10, usedRNG);
    if(popular > 0 && randomNum > (10 - popularityDecisionThreshold)) {
        randomNum = intuniform(0, (popular - 1), usedRNG);
        temp.x = array[randomNum].locCoordX;
        temp.y = array[randomNum].locCoordY;
        temp.z = array[randomNum].locCoordZ;

    } else if (notPopular > 0) {
        randomNum = intuniform(0, (notPopular - 1), usedRNG);
        temp.x = array[popular + randomNum].locCoordX;
        temp.y = array[popular + randomNum].locCoordY;
        temp.z = array[popular + randomNum].locCoordZ;

    } else {
        randomNum = intuniform(0, (size - 1), usedRNG);
        temp.x = array[randomNum].locCoordX;
        temp.y = array[randomNum].locCoordY;
        temp.z = array[randomNum].locCoordZ;

    }

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

    // temporary code to place node at the center of the location
    // and not to spread inside the radius.
    //target = temp;

    // save position for node count computation
    neew = temp;

    return target;
}

int SWIMMobility::updateNodesCount(Coord update, bool inc)
{
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

void SWIMMobility::updateAllNodes(bool increase) {
    int count = 0;

    // get mobility modules of all nodes to compute node count
    cSimulation *currentSimulation = getSimulation();
    int maxID = currentSimulation->getLastComponentId();
    for (int currentID = 0; currentID <= maxID; currentID++) {
        cModule *currentModule = currentSimulation->getModule(currentID);
        SWIMMobility *mobile = dynamic_cast<SWIMMobility*>(currentModule);
        if (currentModule != NULL && mobile != NULL) {
            mobile->updateNodesCount(neew, increase);
            count++;
        }
    }
}

SWIMMobility::~SWIMMobility() {
}

} // namespace inet


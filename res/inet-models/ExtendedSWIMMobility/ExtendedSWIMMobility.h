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
* The C++ include file of the EXTENDED SWIM mobility model for the INET Framework
* in OMNeT++.
*
* @author : Anas bin Muslim (anas1@uni-bremen.de)
*
* Change History:
* Asanga Udugama (adu@comnets.uni-bremen.de)
* - Corrected the jumble of some files in OPS and others in INET
* - All files now are copied into INET (like SWIMMobility files) before building INET
* - use of simtime_t variables instead of double for time variables
*/

#ifndef __INET_EXTENDEDSWIMMOBILITY_H
#define __INET_EXTENDEDSWIMMOBILITY_H

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <omnetpp.h>

#include "inet/common/INETMath.h"
#include "inet/common/INETDefs.h"
#include "inet/mobility/contract/IReactiveMobility.h"
#include "inet/mobility/base/LineSegmentsMobilityBase.h"

struct loc{
    int noOfNodesPresent;

    double myCoordX;
    double myCoordY;
    double myCoordZ;
};

struct nodeProp{
    int seen = 0;

    double locCoordX;
    double locCoordY;
    double locCoordZ;
    double weight = 0;
};

#ifndef PI
#define PI 3.14159265
#endif
#define EXTENDEDSWIMMOBILITY_SIMMODULEINFO       " ExtendedSWIMMobility>!<" << simTime().dbl() << ">!<" << this->getParentModule()->getFullName()

namespace inet {

struct event{
    bool scheduled = false;
    Coord eventLoc;
    simtime_t start;
    simtime_t end;
    int radius = -1;
    std::string eventname;
};

class INET_API ExtendedSWIMMobility : public LineSegmentsMobilityBase, public IReactiveMobility
{
protected:
    bool firstStep;
    bool nextMoveIsWait;
    bool stoppedForEvent;

    int popularityDecisionThreshold;
    int neighbourLocationLimit;
    int returnHomePercentage;
    int usedRNG;
    std::string locationsFilePath;

    double speed;
    double alpha = 0;
    double radius;
    double avgMovementTime;

    std::vector<loc> locations;
    std::vector<nodeProp> neighborLocs;
    std::vector<nodeProp> visitingLocs;
    std::vector<event> emergencies;
    std::vector<event> otherEvents;

    Coord neew;
    Coord homeCoord;

    cMessage *moveToLoc;
    cMessage *eventExpiry;
    cMessage *emergencyEventInterrupt;

    std::string stoppedForEventName;

private:
    bool homeCoordFound;

public:

    bool emergencyReceived;
    bool eventReceived;

    int noOfLocs;
    int nodes;

    double maxAreaX;
    double maxAreaY;
    double maxAreaZ;

protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    virtual void initialize(int stage) override;

    virtual void setTargetPosition() override;

    virtual void move() override;

    virtual void handleSelfMessage(cMessage *message) override;

    /** Reads the locations from file **/
    virtual bool readLocations();

    /** Separates the visiting and neighboring locations and updates their weights **/
    virtual void seperateAndUpdateWeights();

    /** Decides whether neighbor or visiting locations is to be visited next **/
    virtual Coord decision();

    /** Chooses a random set of Coordinates from given array **/
    virtual Coord chooseDestination(std::vector<nodeProp> &array);

    /** Updates the number of nodes for given coordinates **/
    virtual int updateNodesCount(Coord update, bool inc);

    /** Used to update a common locations table in all of the nodes **/
    virtual void updateAllNodes(bool increase);

    virtual Coord spreadInsideRadius(Coord location);

public:
    /** Constructor **/
    ExtendedSWIMMobility();

    /** Destructor **/
    ~ExtendedSWIMMobility();

    /** Move away from emergency or move to an event **/
    // virtual void setNewTargetPosition(cMessage*) override;
    virtual void setNewTargetPosition(std::string dataName, int msgType, double validUntilTime,
                                        std::string eventName) override;

};

}//namespace inet

#endif

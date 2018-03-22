//
// ExtendedSWIMMobility - An Extended SWIM implementation for the INET Framework of the OMNeT++
// Simulator.
//
// Copyright (C) 2016, Sustainable Communication Networks, University of Bremen, Germany
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; version 3 of the License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, see <http://www.gnu.org/licenses/>
//
//
//
//
//
// The main model definition file (NED) of the SWIM mobility model for the
// INET Framework in OMNeT++.
//
// author : Anas bin Muslim (anas1@uni-bremen.de)
//
// Change History:
// Asanga Udugama (adu@comnets.uni-bremen.de)
// - Corrected the jumble of some files in OPS and others in INET
// - All files now are copied into INET (like SWIMMobility files) before building INET
// - Locations file as a parameter
//

package inet.mobility.single;
import inet.mobility.base.MovingMobilityBase;

simple ExtendedSWIMMobility extends MovingMobilityBase{
    parameters:
        bool initFromDisplayString = default(true);
        bool reactiveMobilityModel = default(true);

        int noOfLocations = default(20);
        //int Hosts = default(Hosts);
        int popularityDecisionThreshold = default(7); //0-10
        int returnHomePercentage = default(15); //0-100
        int neighbourLocationLimit @unit(m) = default(300m);

        double speed @unit(mps) = default(20mps);
        double initialX @unit(m) = default(uniform(constraintAreaMinX, constraintAreaMaxX));
        double initialY @unit(m) = default(uniform(constraintAreaMinY, constraintAreaMaxY));
        double initialZ @unit(m) = default(0m);//default(uniform(constraintAreaMinZ, constraintAreaMaxZ));
        double radius @unit(m)= default(50m);
        double alpha = default(0.5);

        //volatile double waitTime @unit(s) = default(0s);
        volatile double waitTime @unit(s) = default(uniform(0s,10s));
        int usedRNG = default(0);
        string locationsFilePath = default("locations.txt");

        @class (ExtendedSWIMMobility);

}

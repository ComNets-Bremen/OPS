//
// SWIMMobility - A SWIM implementation for the INET Framework of the OMNeT++
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
// - RNG numer added to use to generate random numbers
// Anna Förster (anna.foerster@comnets.uni-bremen.de)
// - handling for automated simulation runs for multiple seeds
// Matthias Lehmann (matthias.lehmann@tuhh.de)
// - regeneration of locations when location count changes
// Asanga Udugama (adu@comnets.uni-bremen.de)
// - Introduced a parameter to decide the creation of the locations file
//

package inet.mobility.single;
import inet.mobility.base.MovingMobilityBase;

simple SWIMMobility extends MovingMobilityBase{
    parameters:
        bool initFromDisplayString = default(true);

        int noOfLocations = default(20);
        int Hosts = default(Hosts);
        int popularityDecisionThreshold = default(7); //0-10
        int returnHomePercentage = default(15); //0-100
        int dimensions @unit(D) = default(2D); //2D or 3D

        double neighbourLocationLimit @unit(m) = default(300m);
        double radius @unit(m)= default(5m);
        double alpha = default(0.5);

        double speed @unit(mps) = default(20mps);
        double initialX @unit(m) = default(uniform(constraintAreaMinX, constraintAreaMaxX));
        double initialY @unit(m) = default(uniform(constraintAreaMinY, constraintAreaMaxY));
        double initialZ @unit(m) = default(0m);//default(uniform(constraintAreaMinZ, constraintAreaMaxZ));

        volatile double waitTime @unit(s) = default(0s);

        int usedRNG = default(0);
        
        bool recreateLocationsFile = default(true);

        @class (SWIMMobility);
}


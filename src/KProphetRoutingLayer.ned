//The model for the Prophet Routing Layer

package ops;

simple KProphetRoutingLayer like IForwardingLayer
{
     parameters:


        // unique address of the node
        string ownMACAddress;

        // size of cache in bytes - zero means unlimited caching
        int maximumCacheSize @unit(byte) = default(5000000 byte);

        // maximum amount of hops to forward for a data
        int maximumHopCount = default(25);

        // maximum duration to use when obtaining a random backoff duration
        double maximumRandomBackoffDuration @unit(s) = default(5s);

        // duration after which the next anti-entropy session must be started for a given host
        double antiEntropyInterval @unit(s) = default(300s);

        // logging at layer - 0 = disabled, 1 = enabled
        int logging = default(1);

        // switch on and off the TTL handling (expensive procedure)
        bool useTTL = default(false);

        //Parameters specific to Prophet Routing

        //Max limit
        double pEncounterMax = default(0.7);

        //Initial Encounter value

        double pEncounterFirst = default(0.5);

        //Initial Threshold value

        double pFirstThreshold = default (0.1);

        double alpha = default(0.5);

        //Scaling Constant
        double beta = default(0.9);

        //Aging Constant
        double gamma = default(0.998);

        double delta = default(0.1);

        //Standard time between encounter
        double standardTimeInterval @unit(s) = default(3600s);

	    // OMNeT RNG which is used for generation of random numbers
	    int usedRNG = default(0);

        // icon used
        @display("i=block/transport");

    gates:

        // interface to the upper layer (which may be the application)
        input upperLayerIn;
        output upperLayerOut;

        // interface to the lower layer (which may be the link adaptation layer)
        input lowerLayerIn;
        output lowerLayerOut;

}


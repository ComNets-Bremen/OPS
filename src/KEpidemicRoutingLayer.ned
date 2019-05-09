//
// The model for the Epidemic Routing layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 02-may-2017
//
//
package ops;

simple KEpidemicRoutingLayer
{
    parameters:

        // unique address of the node
        string ownMACAddress;

        // size of cache in bytes - zero means unlimited caching
        int maximumCacheSize @unit(byte) = default(5000000byte);

        // duration after which the next anti-entropy session must be started for a given host
        double antiEntropyInterval @unit(s) = default(300s);

        // maximum amount of hops to forward for a data
        int maximumHopCount = default(25);

        // maximum duration to use when obtaining a random backoff duration
        double maximumRandomBackoffDuration @unit(s) = default(5s);
        
        // switch on and off the TTL handling (expensive procedure)
        bool useTTL = default(false);

	    // OMNeT RNG which is used for generation of random numbers
	    int usedRNG = default(0);

        // icon used
        @display("i=block/transport");

        // statistics and signals
        @signal[dataBytesReceivedFwd](type=long);
        @signal[sumVecBytesReceivedFwd](type=long);
        @signal[dataReqBytesReceivedFwd](type=long);
        @signal[totalBytesReceivedFwd](type=long);
        @signal[cacheBytesRemovedFwd](type=long);
        @signal[cacheBytesAddedFwd](type=long);
        @signal[cacheBytesUpdatedFwd](type=long);
        
    
    gates:

        // interface to the upper layer (which may be the application)
        input upperLayerIn;
        output upperLayerOut;

        // interface to the lower layer (which may be the link adaptation layer)
        input lowerLayerIn;
        output lowerLayerOut;

}

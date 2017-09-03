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
        int maximumCacheSize @unit(byte) = default(0byte);

        // duration after which the next anti-entropy session must be started for a given host
        double antiEntropyInterval @unit(s) = default(300s);

        // maximum amount of hops to forward for a data
        int maximumHopCount = default(10);

        // maximum duration to use when obtaining a random backoff duration
        double maximumRandomBackoffDuration @unit(s) = default(5s);

        // logging at layer - 0 = disabled, 1 = enabled
        int logging = default(1);

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


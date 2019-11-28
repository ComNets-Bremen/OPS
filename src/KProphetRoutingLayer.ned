//
//The model for the Prophet Routing Layer
//
// @author : Kirishanth Chethuraja
// @date   : 25-feb-2019
//
//

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

	    // current cache size stat reporting frequency
	    double cacheSizeReportingFrequency @unit(s) = default(100s);

        // icon used
        @display("i=block/transport");

        // statistics and signals
        @signal[fwdDataBytesReceived](type=long);
        @signal[fwdSumVecBytesReceived](type=long);
        @signal[fwdDataReqBytesReceived](type=long);
        @signal[fwdDPTableRequestBytesReceived](type=long);
        @signal[fwdDPTableDataBytesReceived](type=long);
        @signal[fwdTotalBytesReceived](type=long);
        @signal[fwdHopsTravelled](type=long);
        @signal[fwdHopsTravelledCount](type=long);
        @signal[fwdCacheBytesRemoved](type=long);
        @signal[fwdCacheBytesAdded](type=long);
        @signal[fwdCacheBytesUpdated](type=long);
        @signal[fwdCurrentCacheSizeBytes](type=long);
        @signal[fwdCurrentCacheSizeReportedCount](type=long);
        @signal[fwdCurrentCacheSizeBytesSimple](type=long);

        @signal[fwdDataBytesSent](type=long);
        @signal[fwdSumVecBytesSent](type=long);
        @signal[fwdDataReqBytesSent](type=long);
        @signal[fwdDPTableRequestBytesReceived](type=long);
        @signal[fwdDPTableDataBytesSent](type=long);


    gates:

        // interface to the upper layer (which may be the application)
        input upperLayerIn;
        output upperLayerOut;

        // interface to the lower layer (which may be the link adaptation layer)
        input lowerLayerIn;
        output lowerLayerOut;

}


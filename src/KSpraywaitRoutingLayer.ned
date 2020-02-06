//
// The model for the Epidemic Routing layer
//
// @author : Jibin John(adu@comnets.uni-bremen.de)
// @date   : 02-may-2017
//
//bool disableExcesslog=default(false);

package ops;

simple KSpraywaitRoutingLayer like IForwardingLayer
{
    parameters:

        // unique address of the node
        string ownMACAddress;
        string spraywaitFlavour = default("binary");

        // size of cache in bytes - zero means unlimited caching
        int maximumCacheSize @unit(byte) = default(0byte);

        // duration after which the next anti-entropy session must be started for a given host
        double antiEntropyInterval @unit(s) = default(300s);

        // maximum amount of hops to forward for a data
        int maximumHopCount = default(10);

        // maximum amount of hops to forward for a data (the initial K value)
        int noDuplicate = default(4);

        // maximum duration to use when obtaining a random backoff duration
        double maximumRandomBackoffDuration @unit(s) = default(5s);

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
        @signal[fwdTotalBytesReceived](type=long);
        @signal[fwdHopsTravelled](type=long);
        @signal[fwdHopsTravelledCount](type=long);        
        @signal[fwdCacheBytesRemoved](type=long);
        @signal[fwdCacheBytesAdded](type=long);
        @signal[fwdCacheBytesUpdated](type=long);
        @signal[fwdCurrentCacheSizeBytes](type=long);
        @signal[fwdCurrentCacheSizeReportedCount](type=long);
        @signal[fwdCurrentCacheSizeBytesPeriodic](type=long);

        @signal[fwdCurrentCacheSizeBytes2](type=long);

        @signal[fwdDataBytesSent](type=long);
        @signal[fwdSumVecBytesSent](type=long);
        @signal[fwdDataReqBytesSent](type=long);
        @signal[fwdTotalBytesSent](type=long);

    gates:

        // interface to the upper layer (which may be the application)
        input upperLayerIn;
        output upperLayerOut;

        // interface to the lower layer (which may be the link adaptation layer)
        input lowerLayerIn;
        output lowerLayerOut;
}

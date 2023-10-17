//
// The model implementation for the BubbleRAP Forwarding with Epidemic Routing layer foundation
//
// @author : Akhil Simha Neela (neela@uni-bremen.de)
// @date   : 17-10-2023
//

package ops;

simple KBubbleRAPForwarding like IOPSForwardingLayer
{
    parameters:
        // node index
        int nodeIndex;

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

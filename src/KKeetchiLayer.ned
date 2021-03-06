//
// The model for the OPS layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 01-June-2017
//
//

package ops;

simple KKeetchiLayer like IOPSForwardingLayer
{
    parameters:

        // node index
        int nodeIndex;

        // unique address of the node
        string ownMACAddress;

        // interval for reducing (by 1) the goodness value of data (i.e., age data)
        double agingInterval @unit(s) = default(600s);

        // size of cache in bytes - zero means unlimited caching
        int maximumCacheSize @unit(byte) = default(5000000 byte);

        // a value between 0.0 and 1.0 to indicate after which fraction a change
        // in neighbourhood is considered as a significant change
        // e.g. newly arrived nodes in neighbourhood = 6
        //      total number of neighbourhood nodes = 10
        //      ratio = 6/10 -> 0.6
        //      since 0.6 > 0.25 -> significant change
        double neighbourhoodChangeSignificanceThreshold = default(0.25);

        // cool off period when continuous insignificant changes are detected in the neighbourhood
        double coolOffDuration @unit(s) = default(999999s);

        // the constant that gives the weight assigned to the current goodness value in cache when
        // computing the new value together with the recently arrived goodness value (in a data or
        // feedback) - a value between 0.0 - 1.0
        double learningConst = default(0.5);

        // the factor used to compute wait duration before sending the next data packet from cache
        // when insignificant changes are present in the neighbourhood
        double backoffTimerIncrementFactor = default(1.5);

        // OMNeT RNG which is used for generation of random numbers -- FOR FUTURE USED
        int usedRNG = default(0);

	    // current cache size stat reporting frequency
	    double cacheSizeReportingFrequency @unit(s) = default(100s);

        // icon used
        @display("i=block/transport");

        // statistics and signals
        @signal[fwdDataBytesReceived](type=long);
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
        @signal[fwdTotalBytesSent](type=long);

    gates:

        // interface to the upper layer (which may be the application)
        input upperLayerIn;
        output upperLayerOut;

        // interface to the lower layer (which may be the link adaptation layer)
        input lowerLayerIn;
        output lowerLayerOut;

}


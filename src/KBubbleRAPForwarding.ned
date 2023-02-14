//
// The model for the BubbleRAP forwarder
// @author : Akhil Simha Neela (neela@uni-bremen.de)
// @date   : 26.10.2022
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
        int maximumCacheSize @unit(byte) = default(5000000 byte);
        
        // unicast (selects one neighbour) or broadcast RRS (broadcasts to all)
        bool broadcastRRS = default(true);

        // RNG used for random numbers
        int usedRNG = default(0);

        // enable/disable aging cache
        bool ageCache = default(false);

	    // current cache size stat reporting frequency
	    double cacheSizeReportingFrequency @unit(s) = default(100s);

        // is data sending frequency sync'ed with neighbour list reporting frequency?
        bool  sendOnNeighReportingFrequency = default(true);
        
        // if sendOnNeighReportingFrequency is false, then how many seconds after
        // data is sent, BUT this value has to be greater than and in multiples
        // of the neigh reporting frequency of the WirelessInterface module
        double sendFrequencyWhenNotOnNeighFrequency @unit(s) = default(1s);
        
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


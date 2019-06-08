//
// The model for the Randomized Rumor Spreading layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 25-aug-2016
//
//

package ops;

simple KRRSLayer like IForwardingLayer
{
     parameters:
         
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

        // icon used
      	@display("i=block/transport");

        // statistics and signals
        @signal[dataBytesReceivedFwd](type=long);
        @signal[totalBytesReceivedFwd](type=long);
        @signal[cacheBytesRemovedFwd](type=long);
        @signal[cacheBytesAddedFwd](type=long);
        @signal[cacheBytesUpdatedFwd](type=long);
        @signal[currentCacheSizeBytesFwd](type=long);
        @signal[fwdCurrentCacheSizeReportedCount](type=long);

	gates:
	    
	    // interface to the upper layer (which may be the application)
	    input upperLayerIn;
	    output upperLayerOut;
	    
	    // interface to the lower layer (which may be the link adaptation layer)
	    input lowerLayerIn;
	    output lowerLayerOut;
	    
}


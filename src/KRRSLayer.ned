//
// The model for the Randomized Rumor Spreading layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 25-aug-2016
//
//

package ops;

simple KRRSLayer
{
     parameters:
         
		// unique address of the node         
		string ownMACAddress;
        
        // size of cache in bytes - zero means unlimited caching
        int maximumCacheSize @unit(byte) = default(0byte);
        
        // consider all messages to be destination oriented 
        bool destinationOrientedData = default(false);
		
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


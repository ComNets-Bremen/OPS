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


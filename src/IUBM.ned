//
// The model interface for user behaviour model.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 16-apr-2018
//
//

package ops;

moduleinterface IUBM
{
    parameters:
    
        // node index
        int nodeIndex;
        
        // icon used
      	@display("i=block/app");
      	 
	gates:
	    
	    // interfaces to the lower layer (which is application layer)
	    input lowerLayerIn;
	    output lowerLayerOut;
	    
	    input notificationGeneratorIn;

	    output mobilityOut;
}

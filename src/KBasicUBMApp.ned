//
// The model is the implementation of the Basic UBM application
// for Keetchi Simulator
//
// @date   : 26-06-2016
// @author : Anas bin Muslim (anas1@uni-bremen.de)
//
// Change History:
// Asanga Udugama (adu@comnets.uni-bremen.de)
// - Unwanted code, comments cleanup
// - use of an interface

package ops;

simple KBasicUBMApp like IOPSApplicationLayer
{
    parameters:
        
        // node index
        int nodeIndex;

        // unique address of the node
        string ownMACAddress;

		// prefix assigned to the app
    	string appPrefix = default("/basicubm-");

		// prefix used to generate names appended with a number, e.g., /event-000328     	
    	string appNameGenPrefix = default("/event-");

        // logging at layer - 0 = disabled, 1 = enabled
        int logging = default(1);
		 
        // data size
        int dataSizeInBytes = default(10000);

        // icon used
      	@display("i=block/app");
        
	gates:
	    
	    // interfaces to the lower layer (which may be the keetchi layer)
	    input lowerLayerIn;
	    output lowerLayerOut;
	    
	    input upperLayerIn;
	    output upperLayerOut;
	    
}

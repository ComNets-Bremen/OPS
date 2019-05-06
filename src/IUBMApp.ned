//
// A module interface that provides a common interface to
// any UBM based application in OPS.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 16-apr-2018
//
//

package ops;

moduleinterface IUBMApp
{
    parameters:
        // node index
        int nodeIndex;

        // icon used
      	@display("i=block/app");
    gates:
	    // interfaces to the lower layer (which may be the keetchi layer)
	    input lowerLayerIn;
	    output lowerLayerOut;
	    
	    input upperLayerIn;
	    output upperLayerOut;
}

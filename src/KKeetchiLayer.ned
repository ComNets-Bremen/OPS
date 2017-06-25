//
// The model for the OPS layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 01-June-2017
//
//

package ops;

simple KKeetchiLayer
{
     parameters:

        // unique address of the node
        string ownMACAddress;

        double agingInterval @unit(s) = default(300s);

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


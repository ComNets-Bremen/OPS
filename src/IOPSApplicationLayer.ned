//
// A module interface that represents the application
// layer in the nodes.
// 
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 13-jul-2020
// 

package ops;

moduleinterface IOPSApplicationLayer
{
    parameters:

        // node index
        int nodeIndex;

        // address assigned to node
        string ownMACAddress;

        // icon used
        @display("i=block/transport");

    gates:
        // interface to the upper layer (which may be the user behaviour model)
        input upperLayerIn;
        output upperLayerOut;

        // interface to the lower layer (which may be the forwarding layer)
        input lowerLayerIn;
        output lowerLayerOut;    
}

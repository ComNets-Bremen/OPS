//
// A module interface that represents the link layer 
// in the nodes.
// 
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 13-jul-2020
// 

package ops;

moduleinterface IOPSLinkLayer
{
    parameters:

        // node index
        int nodeIndex;
        
        // address assigned to node
        string ownMACAddress;

        // icon used
        @display("i=block/transport");

    gates:
        // interface to the upper layer (which may be the forwarding layer)
        input upperLayerIn;
        output upperLayerOut;

        // interface to the lower layer (which may be the physical layer)
        input lowerLayerIn;
}

//
// A module interface that represents the forwarding layer in the nodes.
// 
//
// @author : Rudolf Honig
// @date   : 13-jul-2017
// 

package ops;

moduleinterface IOPSForwardingLayer
{
    parameters:

        // node index
        int nodeIndex;

        // address assigned to node
        string ownMACAddress;

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

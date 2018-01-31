//
// A module interface that represents the forwardin layer in the nodes.
// 

package ops;

moduleinterface IForwardingLayer
{
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

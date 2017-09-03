//
// The model for link adaptations between the used link
// technology (below) and the upper layer.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 12-oct-2015
//
//

package ops;

simple KLinkAdaptLayer
{
    parameters:

        // own address
        string ownMACAddress;

        // logging at layer - 0 = disabled, 1 = enabled
        int logging = default(1);

        @display("i=block/layer");

    gates:
        // interface to the upper layer (forwarding layer)
        input upperLayerIn;
        output upperLayerOut;

        // interface to the lower layer (which may be the link technology used, e.g., emulated, WiFi adhoc, WiFi direct, etc.)
        input lowerLayerIn;
        output lowerLayerOut;

}


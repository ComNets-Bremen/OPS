//
// Wireless interface used in Keetchi with functionality
// to handle sending of packets considering wireless range
// and broadcast of packets. It should have access to the
// mobility management components.
//
// @date: 08-11-2015
// @author: Asanga Udugama (adu@comnets.uni-bremen.de)

package ops;

simple KWirelessInterfaceWithAngle like IOPSLinkLayer
{
    parameters:
        // node index
        int nodeIndex;

        string ownMACAddress;
        int calculateSpeedInterval @unit(s) = default(5s); 
        string interfaceTypeName = default("wireless");
        double wirelessRange @unit(m) = default(30m);
        double neighbourScanInterval @unit(s) = default(1s);
        double bandwidthBitRate @unit(bps) = default(100000bps); // 100 Kbps
        int wirelessHeaderSize @unit(byte) = default(16byte); // 32byte = 802.11 a/b/g, 16byte = BT

        // OMNET RNG for random numbers
        // not used - for future use
        int usedRNG = default(0);        

        // icon
        @display("i=device/card");

        // statistics and signals
        @signal[linkNeighSize](type=long);
        @signal[linkNeighSizeCount](type=long);
        @signal[linkContactDuration](type=simtime_t);
        @signal[linkContactDurationCount](type=long);
        @signal[linkContactCount](type=long);

        @signal[linkSimpleNeighSize](type=long);

    gates:
        // interface to the upper layer (which may be the link adaptation layer)
        input upperLayerIn;
        output upperLayerOut;

        // interface to the lower layer (which may be the physical layer)
        // there is no lowerLayerOut as packets are sent directly to
        // emulate wireless communications (uses @directIn)
        input lowerLayerIn;
}

//
// Wireless interface used in Keetchi with functionality
// to handle sending of packets considering wireless range
// and broadcast of packets. It should have access to the
// mobility management components.
//
// @date: 08-11-2015
// @author: Asanga Udugama (adu@comnets.uni-bremen.de)

package ops;

moduleinterface IKNeighborDiscovery
{
    parameters:
        double wirelessRange @unit(m);
        string expectedNodeTypes;
        double neighbourScanInterval;
    gates:
        output upperLayerOut;
}

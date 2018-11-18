//
// Wireless interface used in Keetchi with functionality
// to handle sending of packets considering wireless range
// and broadcast of packets. It should have access to the
// mobility management components.
//
// @date: 08-11-2015
// @author: Asanga Udugama (adu@comnets.uni-bremen.de)

package ops;

simple KCacheBasedNeighborDiscovery like IKNeighborDiscovery
{
    parameters:
        double wirelessRange @unit(m) = default(30m);
        string expectedNodeTypes = default("KNode, KHeraldNode, KUBMNode, KPromoteNode");
        double neighbourScanInterval @unit(s) = default(1s);
        @display("i=block/table");

    gates:
        output upperLayerOut;
}

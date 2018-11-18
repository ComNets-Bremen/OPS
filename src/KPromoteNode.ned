//
// The node model consisting of the different protocol
// layers and deployed with the Promote application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 21-nov-2016
//
//

package ops;

import inet.mobility.contract.IMobility;
import inet.mobility.single.RandomWPMobility;

module KPromoteNode
{
    parameters:
        @networkNode();

        // unique node identifier
        int nodeID;

        // app deployed (or is node just a forwarder?)
        // 1 = app deplyed
        // 2 = app not deployed (just forwarding)
        int promoteAppDeployed = default(1);

        // type of forwarding layer used
        string forwardingLayer @enum(KRRSLayer,KKeetchiLayer,KEpidemicRoutingLayer) = default("KRRSLayer");

        // build own address from nodeID (in the form of a MAC address)
        int digit1 = int((nodeID - (nodeID % 10000)) / 10000);
        int digit2 = int(((nodeID - (this.digit1 * 10000)) - ((nodeID - (this.digit1 * 10000)) % 1000)) / 1000);
        int digit3 = int(((nodeID - (this.digit1 * 10000) - (this.digit2 * 1000)) - ((nodeID - (this.digit1 * 10000) - (this.digit2 * 1000)) % 100)) / 100);
        int digit4 = int(((nodeID - (this.digit1 * 10000) - (this.digit2 * 1000) - (this.digit3 * 100)) - ((nodeID - (this.digit1 * 10000) - (this.digit2 * 1000) - (this.digit3 * 100)) % 10)) / 10);
        int digit5 = int(nodeID - (this.digit1 * 10000) - (this.digit2 * 1000) - (this.digit3 * 100) - (this.digit4 * 10));
        string ownAddress = "02:0" + string(this.digit1) + ":0" + string(this.digit2) + ":0" + string(this.digit3) + ":0" + string(this.digit4) + ":0" + string(this.digit5);
        *.ownMACAddress = ownAddress;

        // mobility model used
        string mobilityType = default("StationaryMobility");

        // icon
        @display("i=device/palm2;bgb=250,340");

    gates:
        input radioIn @directIn;

    submodules:

        // application layer
        promote: KPromoteApp if promoteAppDeployed == 1 {
            nodeIndex = nodeID;
            @display("p=120,30");
        }

        // Forwarding layer
        forwarding: <forwardingLayer> like IForwardingLayer {
            @display("p=120,112");
        }

        // link adaptation layer
        linkAdapt: KLinkAdaptLayer {
            @display("p=120,189");
        }

        neigh: <default("")> like IKNeighborDiscovery if typename != "" {
            @display("p=179.34,249.90001");
        }

        // wifi adhoc interface
        link: <default("KWirelessInterface")> like IKWirelessInterface {
            parameters:
                ownMACAddress = ownAddress;
                @display("p=90,350;t=WirelessIfc,l,blue");
        }

        // mobility
        mobility: <mobilityType> like IMobility;

    connections allowunconnected:

        // gate connections between application and forwarding layer
        promote.lowerLayerIn <-- forwarding.upperLayerOut if promoteAppDeployed == 1;
        promote.lowerLayerOut --> forwarding.upperLayerIn if promoteAppDeployed == 1;

        // gate connections forwarding layer and link adaptation layer
        forwarding.lowerLayerIn <-- linkAdapt.upperLayerOut;
        forwarding.lowerLayerOut --> linkAdapt.upperLayerIn;

        // gate connections between link adaptation layer and the wireless interface
        linkAdapt.lowerLayerIn <-- wirelessIfc.upperLayerOut;
        linkAdapt.lowerLayerOut --> wirelessIfc.upperLayerIn;

        // gate connections between wireless interface and physical layer
        // only considered receipts as @directIn is specified and nodes send
        // directly to other nodes
        wirelessIfc.lowerLayerIn <-- radioIn;

        neigh.upperLayerOut --> linkAdapt.neighborListIn if exists(neigh);
}



//
// The OPS messenger type node model consisting of the different OPS
// layers.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 19-feb-2019
//

package ops;

import inet.mobility.contract.IMobility;

module KMessengerNode
{
    parameters:
        @networkNode();

        // unique node identifier
        int nodeID;

        // app deployed (or is node just a forwarder?)
        bool appDeployed = default(true);

        // type of forwarding layer used
        string forwardingLayer @enum(KRRSLayer,KKeetchiLayer,KEpidemicRoutingLayer,KSpraywaitRoutingLayer,KProphetRoutingLayer) = default("KEpidemicRoutingLayer");

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
        app: KMessengerApp if appDeployed {
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

        // wifi adhoc interface
        link: KWirelessInterface {
            @display("p=120,269");
        }

        // mobility
        mobility: <mobilityType> like IMobility;

    connections allowunconnected:

        // gate connections between application and forwarding layer
        app.lowerLayerIn <-- forwarding.upperLayerOut if appDeployed;
        app.lowerLayerOut --> forwarding.upperLayerIn if appDeployed;

        // gate connections forwarding layer and link adaptation layer
        forwarding.lowerLayerIn <-- linkAdapt.upperLayerOut;
        forwarding.lowerLayerOut --> linkAdapt.upperLayerIn;

        // gate connections between link adaptation layer and the wireless interface
        linkAdapt.lowerLayerIn <-- link.upperLayerOut;
        linkAdapt.lowerLayerOut --> link.upperLayerIn;

        // gate connections between wireless interface and physical layer
        // only considered receipts as @directIn is specified and nodes send
        // directly to other nodes
        link.lowerLayerIn <-- radioIn;

}

//
// The OPS node model consisting of the different OPS
// layers and deployed with the Herald application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 16-aug-2016
//
//

package ops;

import inet.mobility.contract.IMobility;

module KHeraldNode
{
    parameters:
        @networkNode();

        // unique node identifier
        int nodeID;
		
        // app deployed (or is node just a forwarder?)
        // 1 = app deplyed
        // 2 = app not deployed (just forwarding)
		int heraldAppDeployed = default(1);

        // type of forwarding layer used
        // 1 = RRS
        // 2 = Keetchi
        // 3 = Epidemic
        int forwardingLayer = default(2);

        // build own address from nodeID (in the form of a MAC address)
        int digit1 = (nodeID - (nodeID % 10000)) / 10000;
        int digit2 = ((nodeID - (this.digit1 * 10000)) - ((nodeID - (this.digit1 * 10000)) % 1000)) / 1000;
        int digit3 = ((nodeID - (this.digit1 * 10000) - (this.digit2 * 1000)) - ((nodeID - (this.digit1 * 10000) - (this.digit2 * 1000)) % 100)) / 100;
        int digit4 = ((nodeID - (this.digit1 * 10000) - (this.digit2 * 1000) - (this.digit3 * 100)) - ((nodeID - (this.digit1 * 10000) - (this.digit2 * 1000) - (this.digit3 * 100)) % 10)) / 10;
        int digit5 = nodeID - (this.digit1 * 10000) - (this.digit2 * 1000) - (this.digit3 * 100) - (this.digit4 * 10);
        string ownAddress = "02:0" + string(this.digit1) + ":0" + string(this.digit2) + ":0" + string(this.digit3) + ":0" + string(this.digit4) + ":0" + string(this.digit5);

        // mobility model used
        string mobilityType = default("StationaryMobility");

        // icon
        @display("i=device/palm2;bgb=250,340");

    gates:
        input radioIn @directIn;

    submodules:

        // application layer
        herald: KHeraldApp if heraldAppDeployed == 1 {
            parameters:
                nodeIndex = nodeID;
                @display("p=120,30");
        }

        // Forwarding layer
        forwarding: KRRSLayer if forwardingLayer == 1 {
            parameters:
                ownMACAddress = ownAddress;
                @display("p=120,112");
        }

	forwarding: KKeetchiLayer if forwardingLayer == 2 {
            parameters:
                ownMACAddress = ownAddress;
                @display("p=120,112");
        }
        
	forwarding: KEpidemicRoutingLayer if forwardingLayer == 3 {
            parameters:
                ownMACAddress = ownAddress;
                @display("p=120,112");
        }

        // link adaptation layer
        linkAdapt: KLinkAdaptLayer {
            parameters:
                ownMACAddress = ownAddress;
                @display("p=120,189");
        }

        // wifi adhoc interface
        wirelessIfc: KWirelessInterface {
            parameters:
                ownMACAddress = ownAddress;
                @display("p=120,269");
        }

        // mobility
        mobility: <mobilityType> like IMobility;

    connections allowunconnected:

        // gate connections between application and forwarding layer
        herald.lowerLayerIn <-- forwarding.upperLayerOut if heraldAppDeployed == 1;
        herald.lowerLayerOut --> forwarding.upperLayerIn if heraldAppDeployed == 1;

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

}

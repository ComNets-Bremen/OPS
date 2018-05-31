//
// The OPS node model consisting of the different OPS
// layers.
//
// @author : Anas bin Muslim (anas1@uni-bremen.de)
// @date   : 12-may-17
//
// Change History:
// Asanga Udugama (adu@comnets.uni-bremen.de)
// - use of an interface

package ops;

import inet.mobility.contract.IMobility;
import inet.mobility.single.ExtendedSWIMMobility;

module KUBMNode
{
    parameters:
        @networkNode();

        // unique node identifier
        int nodeID;
        
        // user behaviour model used
        string userBahaviourModel = default("KBasicUBM");

        // user behaviour model application used
        string userBahaviourApp = default("KBasicUBMApp");

        // type of forwarding layer used
        string forwardingLayer @enum(KRRSLayer,KKeetchiLayer,KEpidemicRoutingLayer) = default("KKeetchiLayer");

		// Static node (currently only routers) or mobile node
		// Static swim = 0, swim = 1, Extended swim = 2

        // build own address from nodeID (in the form of a MAC address)
        int digit1 = (nodeID - (nodeID % 10000)) / 10000;
        int digit2 = ((nodeID - (this.digit1 * 10000)) - ((nodeID - (this.digit1 * 10000)) % 1000)) / 1000;
        int digit3 = ((nodeID - (this.digit1 * 10000) - (this.digit2 * 1000)) - ((nodeID - (this.digit1 * 10000) - (this.digit2 * 1000)) % 100)) / 100;
        int digit4 = ((nodeID - (this.digit1 * 10000) - (this.digit2 * 1000) - (this.digit3 * 100)) - ((nodeID - (this.digit1 * 10000) - (this.digit2 * 1000) - (this.digit3 * 100)) % 10)) / 10;
        int digit5 = nodeID - (this.digit1 * 10000) - (this.digit2 * 1000) - (this.digit3 * 100) - (this.digit4 * 10);
        string ownAddress = "02:0" + string(this.digit1) + ":0" + string(this.digit2) + ":0" + string(this.digit3) + ":0" + string(this.digit4) + ":0" + string(this.digit5);
        *.ownMACAddress = ownAddress;

		// mobility model used
		string mobilityType = default("SWIMMobility");

        // icon
        @display("i=device/palm2;bgb=290,400");

    gates:
        input radioIn @directIn;
		input notificationGenIn @directIn;

    submodules:

		// user behavior model
		ubm: <userBahaviourModel> like IUBM {
			parameters:
				@display("p=180,50;t=UserBehavior,r,blue");
		}

        // application layer
        app: <userBahaviourApp> like IUBMApp {
            parameters:
				nodeIndex = nodeID;
                @display("p=90,50;t=UBMApp,l,blue");
        }

        // Forwarding layer
        forwarding: <forwardingLayer> like IForwardingLayer {
            @display("p=90,150;t=Epidemic,l,blue");
        }

        // link adaptation layer
        linkAdapt: KLinkAdaptLayer {
            parameters:
                ownMACAddress = ownAddress;
                @display("p=90,250;t=PassThru,l,blue");
        }

        // wifi adhoc interface
        link: KWirelessInterface {
            parameters:
                ownMACAddress = ownAddress;
                @display("p=90,350;t=WirelessIfc,l,blue");
        }

		// mobility 
		mobility: <mobilityType> like IMobility {
                @display("p=180,350;t=ReactiveSWIM,t,blue");
        }

    connections allowunconnected:

		// gate connections between user behavior and application layer
		ubm.lowerLayerIn <-- app.upperLayerOut;
		ubm.lowerLayerOut --> app.upperLayerIn;
		
        // gate connections between application and forwarding layer
        app.lowerLayerIn <-- forwarding.upperLayerOut;
        app.lowerLayerOut --> forwarding.upperLayerIn;

        // gate connections forwarding layer and link adaptation layer
        forwarding.lowerLayerIn <-- linkAdapt.upperLayerOut;
        forwarding.lowerLayerOut --> linkAdapt.upperLayerIn;

		// gate connections between link adaptation layer and the wireless interface
        linkAdapt.lowerLayerIn <-- link.upperLayerOut;
        linkAdapt.lowerLayerOut --> link.upperLayerIn;

        // gate connections between wireless interface and physical layer
        // only considered receipts as @directIn is specified and nodes send
        // direcly to other nodes
        link.lowerLayerIn <-- radioIn;

		ubm.notificationGeneratorIn <-- notificationGenIn;
}

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

module KOPSNode
{
    parameters:
        @networkNode();

        // unique node identifier
        int nodeID;

        // user behaviour modelling
        bool ubmEnabled = default(false);
        string userBahaviourModel = default("KBasicUBM");

        // only a forwarder
        bool forwardingOnly = default(false);

        // configured layers
        string applicationLayer @enum(KHeraldApp,KMessengerApp,KBasicUBMApp) = default("KHeraldApp");
        string forwardingLayer @enum(KRRSLayer,KKeetchiLayer,KEpidemicRoutingLayer) = default("KEpidemicRoutingLayer");
        string adaptationLayer @enum(KLinkAdaptLayer) = default("KLinkAdaptLayer");
        string linkLayer @enum(KWirelessInterface) = default("KWirelessInterface");

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
        @display("i=device/palm2;bgb=271.78,317.5");

        // statistics
        // notification generator
        @statistic[genTotalReceivable](source=sum(genTotalReceivable); record=last,vector);
        
        // from application layer
        @statistic[appLikedDataBytesReceived](source=sum(appLikedDataBytesReceived); record=last,vector);
        @statistic[appNonLikedDataBytesReceived](source=sum(appNonLikedDataBytesReceived); record=last,vector);
        @statistic[appDuplicateDataBytesReceived](source=sum(appDuplicateDataBytesReceived); record=last,vector);
        @statistic[appTotalDataBytesReceived](source=sum(appTotalDataBytesReceived); record=last,vector);

        @statistic[appLikedDataCountReceived](source=sum(appLikedDataCountReceived); record=last,vector);
        @statistic[appNonLikedDataCountReceived](source=sum(appNonLikedDataCountReceived); record=last,vector);
        @statistic[appDuplicateDataCountReceived](source=sum(appDuplicateDataCountReceived); record=last,vector);
        @statistic[appTotalDataCountReceived](source=sum(appTotalDataCountReceived); record=last,vector);

        @statistic[appLikedDataBytesMaxReceivable](source=sum(appLikedDataBytesMaxReceivable); record=last,vector);
        @statistic[appNonLikedDataBytesMaxReceivable](source=sum(appNonLikedDataBytesMaxReceivable); record=last,vector);
        @statistic[appTotalDataBytesMaxReceivable](source=sum(appTotalDataBytesMaxReceivable); record=last,vector);

        @statistic[appLikedDataCountMaxReceivable](source=sum(appLikedDataCountMaxReceivable); record=last,vector);
        @statistic[appNonLikedDataCountMaxReceivable](source=sum(appNonLikedDataCountMaxReceivable); record=last,vector);
        @statistic[appTotalDataCountMaxReceivable](source=sum(appTotalDataCountMaxReceivable); record=last,vector);

        @statistic[appLikedDataReceivedAvgDelay](source=sum(appLikedDataReceivedDelay)/sum(appLikedDataCountReceivedForAvgDelayComp); record=last,vector);
        @statistic[appNonLikedDataReceivedAvgDelay](source=sum(appNonLikedDataReceivedDelay)/sum(appNonLikedDataCountReceivedForAvgDelayComp); record=last,vector);
        @statistic[appTotalDataReceivedAvgDelay](source=sum(appTotalDataReceivedDelay)/sum(appTotalDataCountReceivedForAvgDelayComp); record=last,vector);

        @statistic[appLikedDataDeliveryRatio](source=sum(appLikedDataCountReceivedForRatioComp)/sum(appLikedDataCountMaxReceivableForRatioComp); record=last,vector);
        @statistic[appNonLikedDataDeliveryRatio](source=sum(appNonLikedDataCountReceivedForRatioComp)/sum(appNonLikedDataCountMaxReceivableForRatioComp); record=last,vector);
        @statistic[appTotalDataDeliveryRatio](source=sum(appTotalDataCountReceivedForRatioComp)/sum(appTotalDataCountMaxReceivableForRatioComp); record=last,vector);

        @statistic[appLikedDataAvgHopCount](source=sum(appLikedDataHopsForAvgHopsComp)/sum(appLikedDataHopsCountForAvgHopsComp); record=last,vector);
        @statistic[appNonLikedDataAvgHopCount](source=sum(appNonLikedDataHopsForAvgHopsComp)/sum(appNonLikedDataHopsCountForAvgHopsComp); record=last,vector);
        @statistic[appTotalDataAvgHopCount](source=sum(appTotalDataHopsForAvgHopsComp)/sum(appTotalDataHopsCountForAvgHopsComp); record=last,vector);

        // some special application layer stats
        @statistic[appLikedDataReceivedDelay2](source=appLikedDataReceivedDelay2; record=mean,vector);
        @statistic[appNonLikedDataReceivedDelay2](source=appNonLikedDataReceivedDelay2; record=mean,vector);
        @statistic[appTotalDataReceivedDelay2](source=appTotalDataReceivedDelay2; record=mean,vector);

        @statistic[appTotalDataBytesReceived2](source=sum(appTotalDataBytesReceived2); record=last,vector);
        @statistic[appTotalDataCountReceived2](source=sum(appTotalDataCountReceived2); record=last,vector);

        // statistics
        // from forwarding layer
        @statistic[fwdDataBytesReceived](source=sum(fwdDataBytesReceived); record=last,vector; title="Data Received"; unit=byte);
        @statistic[fwdSumVecBytesReceived](source=sum(fwdSumVecBytesReceived); record=last,vector; title="Summary Vectors Received"; unit=byte);
        @statistic[fwdDataReqBytesReceived](source=sum(fwdDataReqBytesReceived); record=last,vector; title="Data Requests Received"; unit=byte);
        @statistic[fwdDPTableRequestBytesReceived](source=sum(fwdDPTableRequestBytesReceived); record=last,vector; title="DP Table Requests Received"; unit=byte);
        @statistic[fwdDPTableDataBytesReceived](source=sum(fwdDPTableDataBytesReceived); record=last,vector; title="DP Table Data Received"; unit=byte);
        @statistic[fwdTotalBytesReceived](source=sum(fwdTotalBytesReceived); record=last,vector; title="Total Received"; unit=byte);
        @statistic[fwdAvgHopsTravelled](source=sum(fwdHopsTravelled)/sum(fwdHopsTravelledCount); record=last,vector; title="Average Hops Travelled");
        @statistic[fwdCacheBytesRemoved](source=sum(fwdCacheBytesRemoved); record=last,vector; title="Cache Removals"; unit=byte);
        @statistic[fwdCacheBytesAdded](source=sum(fwdCacheBytesAdded); record=last,vector; title="Cache Additions"; unit=byte);
        @statistic[fwdCacheBytesUpdated](source=sum(fwdCacheBytesUpdated); record=last,vector; title="Cache Updates"; unit=byte);
        @statistic[fwdCurrentCacheSizeBytesAvg](source=sum(fwdCurrentCacheSizeBytes)/sum(fwdCurrentCacheSizeReportedCount); record=last,vector; title="Average Cache Size"; unit=byte);

        @statistic[fwdCurrentCacheSizeBytesPeriodic](source=fwdCurrentCacheSizeBytesPeriodic; record=last,mean,vector; title="Periodic Cache Size"; unit=byte);
        @statistic[fwdCurrentCacheSizeBytes2](source=fwdCurrentCacheSizeBytes2; record=last,mean,vector; title="Cache Size"; unit=byte);

        @statistic[fwdDataBytesSent](source=sum(fwdDataBytesSent); record=last,vector; title="Data Sent"; unit=byte);
        @statistic[fwdSumVecBytesSent](source=sum(fwdSumVecBytesSent); record=last,vector; title="Summary Vectors Sent"; unit=byte);
        @statistic[fwdDataReqBytesSent](source=sum(fwdDataReqBytesSent); record=last,vector; title="Data Requests Sent"; unit=byte);
        @statistic[fwdDPTableRequestBytesSent](source=sum(fwdDPTableRequestBytesSent); record=last,vector; title="DP Table Requests Sent"; unit=byte);
        @statistic[fwdDPTableDataBytesSent](source=sum(fwdDPTableDataBytesSent); record=last,vector; title="DP Table Data Sent"; unit=byte);
        @statistic[fwdTotalBytesSent](source=sum(fwdTotalBytesSent); record=last,vector; title="Total Sent"; unit=byte);

        // statistics
        // from link layer
        @statistic[linkAvgNeighbourSize](source=sum(linkNeighSize)/sum(linkNeighSizeCount); record=last,vector; title="Average Neighbour Size");
        @statistic[linkAvgContactDuration](source=sum(linkContactDuration)/sum(linkContactDurationCount); record=last,vector; title="Average Contact Duration"; unit=s);
        @statistic[linkContactCount](source=sum(linkContactCount); record=last,vector; title="Contact Count");

        @statistic[linkSimpleNeighbourSize](source=linkSimpleNeighSize; record=mean,vector; title="Simple Neighbour Size");

        @statistic[linkPacketsSent](source=sum(linkPacketsSent); record=last,vector; title="Packets Sent Count");
        @statistic[linkPacketsSentBytes](source=sum(linkPacketsSentBytes); record=last,vector; title="Total Sent Bytes"; unit=byte);
        @statistic[linkPacketsDelivered](source=sum(linkPacketsDelivered); record=last,vector; title="Packets Delivered Count");
        @statistic[linkPacketsDeliveredBytes](source=sum(linkPacketsDeliveredBytes); record=last,vector; title="Total Delivered Bytes"; unit=byte);
        @statistic[linkPacketsDropped](source=sum(linkPacketsDropped); record=last,vector; title="Packets Dropped Count");
        @statistic[linkPacketsDroppedBytes](source=sum(linkPacketsDroppedBytes); record=last,vector; title="Total Dropped Bytes"; unit=byte);
        @statistic[linkPacketsReceived](source=sum(linkPacketsReceived); record=last,vector; title="Packets Received Count");
        @statistic[linkPacketsReceivedBytes](source=sum(linkPacketsReceivedBytes); record=last,vector; title="Total Received Bytes"; unit=byte);


    gates:
        input radioIn @directIn;
        input notificationGenIn @directIn;

    submodules:

        // user behavior model
        ubm: <userBahaviourModel> like IUBM if ubmEnabled {
            nodeIndex = nodeID;
            @display("p=200.65999,29.633333;t=User\nBehaviour\nModel,r");
        }

        // application layer
        app: <applicationLayer> like IOPSApplicationLayer if !forwardingOnly {
            nodeIndex = nodeID;
            @display("p=120,30;t=Application\nLayer,l");
        }

        // Forwarding layer
        forwarding: <forwardingLayer> like IOPSForwardingLayer {
            nodeIndex = nodeID;
            @display("p=120,112;t=Forwarding\nLayer,r");
        }

        // link adaptation layer
        linkAdapt: <adaptationLayer> like IOPSAdaptationLayer {
            nodeIndex = nodeID;
            @display("p=120,189;t=Link\nAdaptation\nLayer,r");
        }

        // wifi adhoc interface
        link: <linkLayer> like IOPSLinkLayer {
            nodeIndex = nodeID;
            @display("p=120,269;t=Link\nLayer,r");
        }

        // mobility
        mobility: <mobilityType> like IMobility {
            @display("p=186.26666,268.39334;t=Mobility,r");
        }

    connections allowunconnected:

        // gate connections between user behavior and application layer
        ubm.lowerLayerIn <-- app.upperLayerOut if ubmEnabled;
        ubm.lowerLayerOut --> app.upperLayerIn if ubmEnabled;

        // gate connections between application and forwarding layer
        app.lowerLayerIn <-- forwarding.upperLayerOut if !forwardingOnly;
        app.lowerLayerOut --> forwarding.upperLayerIn if !forwardingOnly;

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

        // gate used by UBM to send direct messages
        ubm.notificationGeneratorIn <-- notificationGenIn if ubmEnabled;
}

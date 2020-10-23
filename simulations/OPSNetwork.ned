package ops.simulations;

import ops.*;

network OPSNetwork
{
    parameters:

        // number of nodes in the network
        int numNodes = default(20);

        // if UBM is used, enable
        bool ubmEnabled = default(false);
        string notificationGen = default("KBasicNotificationGenerator");

        // statistics
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

        // special app layer stats
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

        @statistic[fwdCurrentCacheSizeBytesPeriodicAvg](source=periodicAverageForAllNodes(fwdCurrentCacheSizeBytesPeriodic); record=mean,vector; title="Periodic Average Cache Size"; unit=byte);
        @statistic[fwdCurrentCacheSizeBytesAvg2](source=mean(fwdCurrentCacheSizeBytes2); record=last,mean,vector; title="Average Cache Size"; unit=byte);

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


        @display("bgb=190,171");

    submodules:

        // nodes of the network
        host[numNodes]: KOPSNode {
            nodeID = index;
            @display("p=85.36,80.833336");
        }

        // notification generator to use when UBM is enabled
        notificationGenerator: <notificationGen> like INotificationGenerator if ubmEnabled {
            @display("p=86,31");
        }
}

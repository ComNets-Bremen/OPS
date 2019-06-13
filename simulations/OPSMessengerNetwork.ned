package ops.simulations;

import ops.*;

network OPSMessengerNetwork
{
    parameters:

        // number of nodes in the network
        int numNodes = default(20);

        // statistics
        // from application layer
        @statistic[appLikedDataBytesReceived](source=sum(appLikedDataBytesReceived); record=stats,vector);
        @statistic[appNonLikedDataBytesReceived](source=sum(appNonLikedDataBytesReceived); record=stats,vector);
        @statistic[appDuplicateDataBytesReceived](source=sum(appDuplicateDataBytesReceived); record=stats,vector);
        @statistic[appTotalDataBytesReceived](source=sum(appTotalDataBytesReceived); record=stats,vector);

        @statistic[appLikedDataCountReceived](source=sum(appLikedDataCountReceived); record=stats,vector);
        @statistic[appNonLikedDataCountReceived](source=sum(appNonLikedDataCountReceived); record=stats,vector);
        @statistic[appDuplicateDataCountReceived](source=sum(appDuplicateDataCountReceived); record=stats,vector);
        @statistic[appTotalDataCountReceived](source=sum(appTotalDataCountReceived); record=stats,vector);

        @statistic[appLikedDataBytesMaxReceivable](source=sum(appLikedDataBytesMaxReceivable); record=stats,vector);
        @statistic[appNonLikedDataBytesMaxReceivable](source=sum(appNonLikedDataBytesMaxReceivable); record=stats,vector);
        @statistic[appTotalDataBytesMaxReceivable](source=sum(appTotalDataBytesMaxReceivable); record=stats,vector);

        @statistic[appLikedDataCountMaxReceivable](source=sum(appLikedDataCountMaxReceivable); record=stats,vector);
        @statistic[appNonLikedDataCountMaxReceivable](source=sum(appNonLikedDataCountMaxReceivable); record=stats,vector);
        @statistic[appTotalDataCountMaxReceivable](source=sum(appTotalDataCountMaxReceivable); record=stats,vector);

        @statistic[appLikedDataReceivedAvgDelay](source=sum(appLikedDataReceivedDelay)/sum(appLikedDataCountReceivedForAvgDelayComp); record=stats,vector);
        @statistic[appNonLikedDataReceivedAvgDelay](source=sum(appNonLikedDataReceivedDelay)/sum(appNonLikedDataCountReceivedForAvgDelayComp); record=stats,vector);
        @statistic[appTotalDataReceivedAvgDelay](source=sum(appTotalDataReceivedDelay)/sum(appTotalDataCountReceivedForAvgDelayComp); record=stats,vector);

        @statistic[appLikedDataDeliveryRatio](source=sum(appLikedDataCountReceivedForRatioComp)/sum(appLikedDataCountMaxReceivableForRatioComp); record=stats,vector);
        @statistic[appNonLikedDataDeliveryRatio](source=sum(appNonLikedDataCountReceivedForRatioComp)/sum(appNonLikedDataCountMaxReceivableForRatioComp); record=stats,vector);
        @statistic[appTotalDataDeliveryRatio](source=sum(appTotalDataCountReceivedForRatioComp)/sum(appTotalDataCountMaxReceivableForRatioComp); record=stats,vector);

        // statistics
        // from forwarding layer
        @statistic[fwdDataBytesReceived](source=sum(fwdDataBytesReceived); record=stats,vector; title="Data Received"; unit=byte);
        @statistic[fwdSumVecBytesReceived](source=sum(fwdSumVecBytesReceived); record=stats,vector; title="Summary Vectors Received"; unit=byte);
        @statistic[fwdDataReqBytesReceived](source=sum(fwdDataReqBytesReceived); record=stats,vector; title="Data Requests Received"; unit=byte);
        @statistic[fwdTotalBytesReceived](source=sum(fwdTotalBytesReceived); record=stats,vector; title="Total Received"; unit=byte);
        @statistic[fwdCacheBytesRemoved](source=sum(fwdCacheBytesRemoved); record=stats,vector; title="Cache Removals"; unit=byte);
        @statistic[fwdCacheBytesAdded](source=sum(fwdCacheBytesAdded); record=stats,vector; title="Cache Additions"; unit=byte);
        @statistic[fwdCacheBytesUpdated](source=sum(fwdCacheBytesUpdated); record=stats,vector; title="Cache Updates"; unit=byte);
        @statistic[fwdCurrentCacheSizeBytesAvg](source=sum(fwdCurrentCacheSizeBytes)/sum(fwdCurrentCacheSizeReportedCount); record=stats,vector; title="Average Cache Size"; unit=byte);

        // statistics
        // from link layer
        @statistic[linkAvgNeighbourSize](source=sum(linkNeighSize)/sum(linkNeighSizeCount);record=stats,vector;title="Average Neighbour Size";unit=byte);
        @statistic[linkAvgContactDuration](source=sum(linkContactDuration)/sum(linkContactDurationCount);record=stats,vector;title="Average Contact Duration";unit=byte);

        @display("bgb=190,171");
   
    submodules:

        // nodes of the network
        host[numNodes]: KMessengerNode {
            nodeID = index;
            @display("p=94,29");
        }
}

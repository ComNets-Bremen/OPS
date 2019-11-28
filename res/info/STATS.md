# Collected Results (Statistics)

There are different statistics collected by OPS during a simulation. These statistics
are available at the **Node level** and at the **Network level**. Statistics are initiated 
from different protocol layers of a node model. There are two types of statistics 
collected.

1. Vector statistics - These are the continuous statistics that are collected. These 
results can be plotted as graphs.

2. Scalar statistics - These are the measures of dispersion type statistics collected. In 
OPS, we only collect the **final value** of a scalar statistic.

Information about each of these statistics are given below.

## Statistics from the Application Layer

The table below lists the statistics generated from the application layer.


| Statistic Name                     |         Description                     | Collected Level |
| --------------                     |         -----------                     | --------------- |
| appLikedDataBytesReceived          | Liked data bytes received               | Network, Node   |
| appNonLikedDataBytesReceived       | Non-liked data bytes received           | Network, Node   |
| appDuplicateDataBytesReceived      | Duplicate data bytes received           | Network, Node   |
| appTotalDataBytesReceived          | Total data bytes received               | Network, Node   |
| appLikedDataCountReceived          | Liked data count received               | Network, Node   |
| appNonLikedDataCountReceived       | Non-liked data count received           | Network, Node   |
| appDuplicateDataCountReceived      | Duplicate data count received           | Network, Node   |
| appTotalDataCountReceived          | Total data count received               | Network, Node   |
| appLikedDataBytesMaxReceivable     | Liked data bytes maximum receivable     | Network, Node   |
| appNonLikedDataBytesMaxReceivable  | Non-liked data bytes maximum receivable | Network, Node   |
| appTotalDataBytesMaxReceivable     | Total data bytes maximum receivable     | Network, Node   |
| appLikedDataCountMaxReceivable     | Liked data bytes maximum receivable     | Network, Node   |
| appNonLikedDataCountMaxReceivable  | Non-liked data bytes maximum receivable | Network, Node   |
| appTotalDataCountMaxReceivable     | Total data bytes maximum receivable     | Network, Node   |
| appLikedDataReceivedAvgDelay       | Liked data received average delay       | Network, Node   |
| appNonLikedDataReceivedAvgDelay    | Non-liked data received average delay   | Network, Node   |
| appTotalDataReceivedAvgDelay       | Total data received average delay       | Network, Node   |
| appLikedDataDeliveryRatio          | Liked data delivery ratio               | Network, Node   |
| appNonLikedDataDeliveryRatio       | Non-liked data delivery ratio           | Network, Node   |
| appTotalDataDeliveryRatio          | Total data delivery ratio               | Network, Node   |
| appLikedDataAvgHopCount            | Liked data average hop count            | Network, Node   |
| appNonLikedDataAvgHopCount         | Non-liked data average hop count        | Network, Node   |
| appTotalDataAvgHopCount            | Total data average hop count            | Network, Node   |




## Statistics from the Forwarding Layer

The table below lists the statistics generated from the forwarding layer. Since different
forwarding layer protocols operation differently, they may not produce all the statistics
list below.

| Statistic Name                 |         Description          | Collected Level |
| --------------                 |         -----------          | --------------- |
| fwdDataBytesReceived           | Data Received                | Network, Node   |
| fwdSumVecBytesReceived         | Summary Vectors Received     | Network, Node   |
| fwdDataReqBytesReceived        | Data Requests Received       | Network, Node   |
| fwdDPTableRequestBytesReceived | DP Table Requests Received   | Network, Node   |
| fwdDPTableDataBytesReceived    | DP Table Data Received       | Network, Node   |
| fwdTotalBytesReceived          | Total Received               | Network, Node   |
| fwdAvgHopsTravelled            | Average Hops Travelled       | Network, Node   |
| fwdCacheBytesRemoved           | Cache Removals               | Network, Node   |
| fwdCacheBytesAdded             | Cache Additions              | Network, Node   |
| fwdCacheBytesUpdated           | Cache Updates                | Network, Node   |
| fwdCurrentCacheSizeBytesAvg    | Average Cache Size           | Network, Node   |
| fwdCurrentCacheSizeBytesSimple | Cache Size (Node level)      | Network, Node   |
|                                | Mean Cache Size (Network level) |              |
| fwdDataBytesSent               | Data Sent                    | Network, Node   |
| fwdSumVecBytesSent             | Summary Vectors Sent         | Network, Node   |
| fwdDataReqBytesSent            | Data Requests Sent           | Network, Node   |
| fwdDPTableRequestBytesSent     | DP Table Requests Sent       | Network, Node   |
| fwdDPTableDataBytesSent        | DP Table Data Sent           | Network, Node   |
| fwdTotalBytesSent              | Total Sent                   | Network, Node   |



## Statistics from the (WirelessInterface) Link Layer

The table below lists the statistics generated from the link layer.
 
| Statistic Name              |         Description          | Collected Level |
| --------------              |         -----------          | --------------- |
| linkAvgNeighbourSize        | Average Neighbour Size       | Network, Node   |
| linkAvgContactDuration      | Average Contact Duration     | Network, Node   |



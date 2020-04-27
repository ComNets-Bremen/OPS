//
// The model implementation of an Optimal Routing layer (delivery delay version)
//
// @author : Kirishanth Chethuraja
// @date   : 19-oct-2019
//
// Update for direct delivery of packets and stats.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 25-apr-2020

package ops;

simple KOptimumDelayRoutingLayer like IForwardingLayer
{
    parameters:

        // unique address of the node
        string ownMACAddress;

                
        // OMNeT RNG which is used for generation of random numbers
        int usedRNG = default(0);

        // current cache size stat reporting frequency
        double cacheSizeReportingFrequency @unit(s) = default(100s);

        // icon used
        @display("i=block/transport");

        // statistics and signals
        @signal[fwdDataBytesReceived](type=long);
        @signal[fwdTotalBytesReceived](type=long);
        @signal[fwdHopsTravelled](type=long);
        @signal[fwdHopsTravelledCount](type=long);        
        @signal[fwdCacheBytesRemoved](type=long);
        @signal[fwdCacheBytesAdded](type=long);
        @signal[fwdCacheBytesUpdated](type=long);
        @signal[fwdCurrentCacheSizeBytes](type=long);
        @signal[fwdCurrentCacheSizeReportedCount](type=long);

        @signal[fwdCurrentCacheSizeBytesPeriodic](type=long);
        
        @signal[fwdCurrentCacheSizeBytes2](type=long);

        @signal[fwdDataBytesSent](type=long);
        @signal[fwdTotalBytesSent](type=long);


    gates:

        // interface to the upper layer (which may be the application)
        input upperLayerIn;
        output upperLayerOut;

        // interface to the lower layer (which may be the link adaptation layer)
        input lowerLayerIn;
        output lowerLayerOut;
        
        // interface to receive data messages directly from other nodes (without delays)
        input lowerLayerDirectIn @directIn;

}

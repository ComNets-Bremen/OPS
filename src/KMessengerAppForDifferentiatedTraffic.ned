//
// The application model for destination oriented 
// data generations.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de), Anna Förster (afoerster@uni-bremen.de)
// @date   : 15-aug-2016, updated 6-febr-2018
//
//
// MessengerApp extended to handle differentiate traffic based on
// patterns, either based on random time frames or based
// the original traffic pattern in the traces.
//
// @author: Thenuka Karunatilake (thenukaramesh@gmail.com)
// @date: 01-03-2021
//


package ops;

simple KMessengerAppForDifferentiatedTraffic like IOPSApplicationLayer
{
     parameters:

        // assigned ID for the node
        int nodeIndex;

        // unique address of the node
        string ownMACAddress;

	// Data generation mode - how traffic is generated
        // random = random times based on groups
        // trace = times of the original trace
        string dataGenerationIntervalMode = default("trace");

        // Path to traffic info files (to generate differentiated traffic)
        string trafficInfoPath = default("./Scenario");

        // RNG to use
        int usedRNG = default(0);

        // how often data is generated
        double dataGenerationInterval @unit(s) = default(15min);

        // data size
        int dataSizeInBytes = default(10000);

        // TTL of messages
        double ttl = default(999999);

        // percentage of nodes from notificationCount to assign as being popularity
        double popularityAssignmentPercentage = default(25.0); // 25.0 = 25%

        // are messages destined to a single specific destination?
        bool specificDestination = default(false);
        
        // if singleSpecificDestination is true, then the node name of the specific destination, e.g., host[23]
        string specificDestinationNodeName = default("host[0]");

        // icon used
        @display("i=block/app");

        //statistic signals
        @signal[appLikedDataBytesReceived](type=long);
        @signal[appNonLikedDataBytesReceived](type=long);
        @signal[appDuplicateDataBytesReceived](type=long);
        @signal[appTotalDataBytesReceived](type=long);

        @signal[appLikedDataBytesMaxReceivable](type=long);
        @signal[appNonLikedDataBytesMaxReceivable](type=long);
        @signal[appTotalDataBytesMaxReceivable](type=long);

        @signal[appLikedDataCountReceived](type=long);
        @signal[appNonLikedDataCountReceived](type=long);
        @signal[appDuplicateDataCountReceived](type=long);
        @signal[appTotalDataCountReceived](type=long);

        @signal[appLikedDataCountMaxReceivable](type=long);
        @signal[appNonLikedDataCountMaxReceivable](type=long);
        @signal[appTotalDataCountMaxReceivable](type=long);


        @signal[appLikedDataReceivedDelay](type=simtime_t);
        @signal[appNonLikedDataReceivedDelay](type=simtime_t);
        @signal[appTotalDataReceivedDelay](type=simtime_t);

        @signal[appLikedDataCountReceivedForAvgDelayComp](type=long);
        @signal[appNonLikedDataCountReceivedForAvgDelayComp](type=long);
        @signal[appTotalDataCountReceivedForAvgDelayComp](type=long);

        @signal[appLikedDataCountReceivedForRatioComp](type=long);
        @signal[appNonLikedDataCountReceivedForRatioComp](type=long);
        @signal[appTotalDataCountReceivedForRatioComp](type=long);

        @signal[appLikedDataCountMaxReceivableForRatioComp](type=long);
        @signal[appNonLikedDataCountMaxReceivableForRatioComp](type=long);
        @signal[appTotalDataCountMaxReceivableForRatioComp](type=long);


        @signal[appLikedDataHopsForAvgHopsComp](type=long);
        @signal[appNonLikedDataHopsForAvgHopsComp](type=long);
        @signal[appTotalDataHopsForAvgHopsComp](type=long);

        @signal[appLikedDataHopsCountForAvgHopsComp](type=long);
        @signal[appNonLikedDataHopsCountForAvgHopsComp](type=long);
        @signal[appTotalDataHopsCountForAvgHopsComp](type=long);

        // signals for special stats
        @signal[appLikedDataReceivedDelay2](type=simtime_t);
        @signal[appNonLikedDataReceivedDelay2](type=simtime_t);
        @signal[appTotalDataReceivedDelay2](type=simtime_t);

        @signal[appTotalDataBytesReceived2](type=long);
        @signal[appTotalDataCountReceived2](type=long);

    gates:

        // interfaces to the lower layer
        input lowerLayerIn;
        output lowerLayerOut;
        
        // interface for statistics
        input statIn @directIn;

        // interface to the upper layer (never used, but included because of IOPSApplicationLayer interface)
        input upperLayerIn;
        output upperLayerOut;

}

//
// The model for the Event Notifier (Herald) application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de), Anna Förster (afoerster@uni-bremen.de)
// @date   : 15-aug-2016, updated 6-febr-2018
//
//

package ops;

simple KHeraldApp
{
     parameters:

        // assigned ID for the node
        int nodeIndex;


        // percentage of nodes from notificationCount to assign popularity values
        double popularityAssignmentPercentage = default(25.0); // 25.0 = 25%

        // RNG to use
        int usedRNG = default(0);

        // how often data is generated
        double dataGenerationInterval @unit(s) = default(15min);


        // data size
        int dataSizeInBytes = default(10000);

        // logging at layer - 0 = disabled, 1 = enabled
        int logging = default(1);
        
        // TTL of messages
        double ttl = default(999999999999);

        // icon used
        @display("i=block/app");

    gates:

        // interfaces to the lower layer
        input lowerLayerIn;
        output lowerLayerOut;

}

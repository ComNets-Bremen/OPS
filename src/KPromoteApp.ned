//
// The model for the Event Notifier (Promote) application
// which generates destination oriente notification.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 17-nov-2016
//
//

package ops;

simple KPromoteApp
{
    parameters:

        // assigned ID for the node
        int nodeIndex;

        // notification generation distribution
        // 1 = contant time intervals (i.e., no distribution)
        // 2 = exponential distribution (mean = notificationGenInterval)
        // 3 = uniform distribution (between 0.0 and notificationGenInterval)
        int notificationGenDistribution = default(1);

        // used to determine the start time of notification generation
        double notificationGenStartMaxTime @unit(s) = default(3600s);

        // notification generation interval
        double notificationGenInterval @unit(s) = default(7200s);

        // duration of the notification
        double notificationValidDuration @unit(s) = default(172800s);

        // data payload size of notification
        int dataPayloadSizeInBytes = default(1000);

        // total packet size of notification
        int dataPacketSizeInBytes = default(1000);

        // RNG to use
        int usedRNG = default(0);

        // node names to search for - used to get the destination address
        string expectedNodeTypes = default("KNode, KHeraldNode, KUBMNode, KPromoteNode");
        
        // are generated data destination oriented
        bool destinationOriented = default(false);
        
        // logging at layer - 0 = disabled, 1 = enabled
        int logging = default(1);

        // icon used
        @display("i=block/app");

    gates:

        // interfaces to the lower layer
        input lowerLayerIn;
        output lowerLayerOut;

}

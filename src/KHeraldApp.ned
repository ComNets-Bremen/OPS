//
// The model for the Event Notifier (Herald) application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 15-aug-2016
//
//

package ops;

simple KHeraldApp
{
     parameters:

        // assigned ID for the node
        int nodeIndex;

        // number of notifications to use (from the 100 defined)
        int notificationCount = default(20);

        // RNG to use
        int usedRNG = default(0);

        // how often data is generated
        double dataGenerationInterval @unit(s) = default(5s);

        // node that generates data in (pumps data in to) network
        int dataGeneratingNodeIndex = default(1);

        // icon used
        @display("i=block/app");

    gates:

        // interfaces to the lower layer
        input lowerLayerIn;
        output lowerLayerOut;

}

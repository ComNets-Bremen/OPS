
package ops.simulations;

import ops.*;

network OPSUBMNetwork
{
    parameters:

        // number of nodes in the network
        int numNodes = default(20);

        // type of forwarding layer used
        string notificationGen = default("KBasicNotificationGenerator");

        @display("bgb=190,171");
    submodules:

        // nodes of the network
        host[numNodes]: KUBMNode {
            nodeID = index;
            @display("p=86,104");
        }

        notificationGenerator: <notificationGen> like INotificationGenerator {
            @display("p=86,31");
        }
}

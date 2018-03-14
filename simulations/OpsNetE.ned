
package ops.simulations;

import ops.*;

network OpsNetE
{
   
    parameters:
        int numNodes = default(20);
       
    submodules:

        // nodes of the network
        host[numNodes]: KUBMNode {
            nodeID = index;
        }
       
       notificationGenerator: KNotificationGenerator{}

}


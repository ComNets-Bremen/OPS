
package ops.simulations;

import ops.*;

network OPSNetwork
{
   
    parameters:
    
        // number of nodes in the network
        int numNodes = default(20);
        
        // type of forwarding layer used
        string notificationGen = default("KBasicNotificationGenerator");
       
    submodules:

        // nodes of the network
        host[numNodes]: KUBMNode {
            nodeID = index;
        }
       
       notificationGenerator: <notificationGen> like INotificationGenerator;
}

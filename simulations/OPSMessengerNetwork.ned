package ops.simulations;

import ops.*;

network OPSMessengerNetwork
{
   
    parameters:
    
        // number of nodes in the network
        int numNodes = default(20);
       
    submodules:

        // nodes of the network
        host[numNodes]: KMessengerNode {
            nodeID = index;
        }
}

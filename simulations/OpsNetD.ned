
package ops.simulations;

import ops.*;

network OpsNetD
{
    parameters:
        int numNodes = default(20);

    submodules:

        // nodes of the network
        node[numNodes]: KPromoteNode {
            nodeID = index;
			promoteAppDeployed = 1;
        }

}


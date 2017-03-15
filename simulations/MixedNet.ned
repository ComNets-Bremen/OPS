
package ops.simulations;

import ops.*;

network MixedNet
{
    parameters:
        int numNodesM1 = default(20);
        int numNodesM2 = default(0);
        int numFixedNodes = default(20);

    submodules:

        // mobile nodes of the network with app deployed
        nodeM1[numNodesM1]: KPromoteNode {
            nodeID = index;
			promoteAppDeployed = 1;
        }

        // mobile nodes of the network without the app deployed
        nodeM2[numNodesM2]: KPromoteNode {
            nodeID = index;
			promoteAppDeployed = 0;
        }

        // fixed nodes of the network
        fixedNode[numFixedNodes]: KPromoteNode {
            nodeID = numNodesM1 + numNodesM2 + index;
			promoteAppDeployed = 0;
        }

}


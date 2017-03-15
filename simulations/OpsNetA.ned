
package ops.simulations;

import ops.*;

network OpsNetA
{
    parameters:
        int numNodes = default(20);

    submodules:

        // nodes of the network
        node[numNodes]: KNode {
            nodeID = index;
        }

}


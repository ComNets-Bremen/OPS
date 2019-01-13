
package ops.simulations;

import ops.*;

network OpsNetB
{
    parameters:
        int numNodes = default(20);

    submodules:

        // nodes of the network
        node[numNodes]: KHeraldNode {
            nodeID = index;
			heraldAppDeployed = 1;
        }

        neighborCache: <default("")> like INeighborCache if typename != "" {
            @display("p=200,50");
        }

}


//
// Copyright (C) 2014 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "NeighborCacheBase.h"
#include "KBaseNodeInfo.h"

void NeighborCacheBase::collectAllNodes(const std::string& expectedNodeTypes)
{
    // get module info of all other nodes in network
    for (int id = 0; id <= getSimulation()->getLastComponentId(); id++) {
        cModule *unknownModule = getSimulation()->getModule(id);
        if (unknownModule == nullptr)
            continue;

        // has to be a node type module given in expectedNodeTypes parameter
        if (strstr(expectedNodeTypes.c_str(), unknownModule->getModuleType()->getName()) == NULL)
            continue;

        KBaseNodeInfo *nodeInfo = new KBaseNodeInfo();
        nodeInfo->nodeModule = unknownModule;

        // find the wireless ifc module & mobility module
        nodeInfo->nodeWirelessIfcModule = unknownModule->getSubmodule("link");
        nodeInfo->nodeMobilityModule = dynamic_cast<inet::IMobility*>(unknownModule->getSubmodule("mobility"));

        // wireless ifc module & mobility module must be present, else something wrong
        if (nodeInfo->nodeMobilityModule == NULL || nodeInfo->nodeWirelessIfcModule == NULL) {
            delete nodeInfo;
            continue;
        }

        allNodes.push_back(nodeInfo);
    }
}

INeighborCache::Nodes NeighborCacheBase::getNeighbors(const KBaseNodeInfo *node, double range) const
{
    // filter potential neighbors list to those that are really no farther than range
    Coord pos = node->nodeMobilityModule->getCurrentPosition();
    Nodes neighbors;
    double rangeSquared = range * range;
    for (auto neighbor : getPotentialNeighbors(node, range)) {
        Coord neighborPos = neighbor->nodeMobilityModule->getCurrentPosition();
        if (pos.sqrdist(neighborPos) <= rangeSquared)
            neighbors.push_back(neighbor);
    }
    return neighbors;
}


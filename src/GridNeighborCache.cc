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

#include "GridNeighborCache.h"
#include "inet/common/ModuleAccess.h"


Define_Module(GridNeighborCache);

void GridNeighborCache::initialize(int stage)
{
    if (stage == 0) {
        cellSize.x = par("cellSizeX");
        cellSize.y = par("cellSizeY");
        cellSize.z = par("cellSizeZ");
        refillPeriod = par("refillPeriod");
        refillCellsTimer = new cMessage("refillCellsTimer");
    }
    else if (stage == 2) {
        collectAllNodes(par("expectedNodeTypes").stringValue());

        constraintAreaMin.x = par("minX").doubleValue(); //TODO get these parameters from somewhere else
        constraintAreaMin.y = par("minY").doubleValue();
        constraintAreaMin.z = par("minZ").doubleValue();
        constraintAreaMax.x = par("maxX").doubleValue();
        constraintAreaMax.y = par("maxY").doubleValue();
        constraintAreaMax.z = par("maxZ").doubleValue();
        maxSpeed = par("maxSpeed").doubleValue();

        const Coord constraintAreaSize = constraintAreaMax - constraintAreaMin;
        if (std::isnan(cellSize.x))
            cellSize.x = constraintAreaSize.x / par("cellCountX").intValue();
        if (std::isnan(cellSize.y))
            cellSize.y = constraintAreaSize.y / par("cellCountY").intValue();
        if (std::isnan(cellSize.z))
            cellSize.z = constraintAreaSize.z / par("cellCountZ").intValue();

        WATCH(constraintAreaMin);
        WATCH(constraintAreaMax);
        WATCH(cellSize);
        WATCH_VECTOR(allNodes);

        fillCubeVector();
        scheduleAt(simTime() + refillPeriod, refillCellsTimer);
    }
}

void GridNeighborCache::handleMessage(cMessage *msg)
{
    if (!msg->isSelfMessage())
        throw cRuntimeError("This module only handles self messages");
    EV_DETAIL << "Updating the grid cells" << endl;
    fillCubeVector();
    scheduleAt(simTime() + refillPeriod, msg);
}

void GridNeighborCache::fillCubeVector()
{
    delete grid;
    grid = new SpatialGrid(cellSize, constraintAreaMin, constraintAreaMax);
    for (const KBaseNodeInfo *node : allNodes) {
        Coord pos = node->nodeMobilityModule->getCurrentPosition();
        grid->insertPoint(node, pos);
    }
}

GridNeighborCache::Nodes GridNeighborCache::getPotentialNeighbors(const KBaseNodeInfo *node, double range) const
{
    double radius = range + (maxSpeed * refillPeriod);
    Coord pos = node->nodeMobilityModule->getCurrentPosition();
    Nodes result;
    CollectVisitor visitor(node, result);
    grid->rangeQuery(pos, radius, &visitor);
    return result;
}

void GridNeighborCache::CollectVisitor::visit(const cObject *neighborObj) const
{
    const KBaseNodeInfo *neighbor = check_and_cast<const KBaseNodeInfo *>(neighborObj);
    if (node->nodeModule != neighbor->nodeModule)
        result.push_back(neighbor);
}

GridNeighborCache::~GridNeighborCache()
{
    cancelAndDelete(refillCellsTimer);
    delete grid;
}

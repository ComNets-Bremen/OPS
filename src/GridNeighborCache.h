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

#ifndef __OPS_GRIDNEIGHBORCACHE_H
#define __OPS_GRIDNEIGHBORCACHE_H

#include "NeighborCacheBase.h"
#include "inet/common/geometry/container/SpatialGrid.h"

using namespace inet;

class GridNeighborCache : public NeighborCacheBase
{
  protected:
    class CollectVisitor : public IVisitor
    {
      protected:
        const KBaseNodeInfo *node;
        Nodes& result;

      public:
        void visit(const cObject *node) const override;
        CollectVisitor(const KBaseNodeInfo *node, Nodes& result) :
            node(node), result(result) {}
    };
  protected:
    SpatialGrid *grid = nullptr;
    Coord constraintAreaMin = Coord::NIL, constraintAreaMax = Coord::NIL;
    cMessage *refillCellsTimer = nullptr;
    double refillPeriod = NaN;
    double maxSpeed = NaN;
    Coord cellSize = Coord::NIL;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    void fillCubeVector();

  public:
    GridNeighborCache() {}
    virtual ~GridNeighborCache();
    virtual Nodes getPotentialNeighbors(const KBaseNodeInfo *node, double range) const override;
};

#endif


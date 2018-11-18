//
// Copyright (C) 2013 OpenSim Ltd.
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

#ifndef __OPS_INEIGHBORCACHE_H
#define __OPS_INEIGHBORCACHE_H

#include <omnetpp.h>
#include "KBaseNodeInfo.h"

/**
 * This interface keeps track of neighbor relationships between OPS nodes.
 */
class INeighborCache
{
  public:
    typedef std::vector<const KBaseNodeInfo *> Nodes;

    /**
     * Return the list of nodes that MAY BE within the given range.
     * (The result is a superset if the set of actual neighbors.)
     */
    virtual Nodes getPotentialNeighbors(const KBaseNodeInfo *node, double range) const = 0;

    /**
     * Return the list of nodes that are within the given range.
     */
    virtual Nodes getNeighbors(const KBaseNodeInfo *node, double range) const = 0;

};

#endif


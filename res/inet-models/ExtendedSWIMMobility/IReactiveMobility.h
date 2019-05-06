/* -*- mode:c++ -*- ********************************************************
 * file:        IMobility.h
 *
 * author:      Levente Meszaros
 *
 * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
 *              Technische Universitaet Berlin, Germany.
 *
 *              (C) 2005 Andras Varga
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 ***************************************************************************
 * part of:     framework implementation developed by tkn
 *************************************************************************/

#ifndef __INET_IREACTIVEMOBILITY_H
#define __INET_IREACTIVEMOBILITY_H

#include "inet/common/INETDefs.h"

#include "inet/common/geometry/common/Coord.h"
#include "inet/common/geometry/common/EulerAngles.h"

namespace inet {

/**
 * @brief Abstract base class defining the public interface that must be provided by all mobility modules.
 *
 * @ingroup mobility
 * @author Levente Meszaros
 */
class INET_API IReactiveMobility
{

  public:
    //virtual ~IMobility() {}

    /** @brief Returns the maximum possible speed at any future time. */
    // virtual void setNewTargetPosition(cMessage * message) = 0;
    virtual void setNewTargetPosition(std::string dataName, int msgType, double validUntilTime,
                                        std::string eventName) = 0;

};

} // namespace inet

#endif // ifndef __INET_IMOBILITY_H


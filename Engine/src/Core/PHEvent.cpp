/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHEvent.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>

#define INIT _owner(NULL), _sender(NULL), _handled(false), coord(NULL), lastCoord(NULL)

PHEvent::PHEvent() : INIT
{
}

PHEvent::PHEvent(PHEvent * o) : INIT
{
    setLastTime(o->time());
    setLastLocation(o->location());
    setOwner(o->owner());
}

PHDrawableCoordinates * PHEvent::drawableLocation()
{
    if (!coord)
        coord = new PHDrawableCoordinates(PHAABox(
            PHVector3(_location.x, _location.y, -1),
            PHVector3(_location.x, _location.y, 1))
       );
    return coord;
}

PHDrawableCoordinates * PHEvent::lastDrawableLocation()
{
    if (!lastCoord)
        lastCoord = new PHDrawableCoordinates(PHAABox(
            PHVector3(_lastLocation.x, _lastLocation.y, -1),
            PHVector3(_lastLocation.x, _lastLocation.y, 1))
       );
    return lastCoord;
}

PHEvent::~PHEvent()
{
    if (coord)
        coord->release();
    if (lastCoord)
        lastCoord->release();
}

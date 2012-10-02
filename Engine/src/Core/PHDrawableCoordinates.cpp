/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PHDrawableCoordinates.h>
#include <Porkholt/Core/PHDrawable.h>

PHDrawableCoordinates::PHDrawableCoordinates(const PHPositionalVector & screenSpace, PHDrawable * d)
{
    setPositionInDrawable(d, screenSpace);
    propagateUpstream(d);
}

void PHDrawableCoordinates::setPositionInDrawable(PHDrawable * d, const PHPositionalVector & v)
{
    coord.insert(make_pair(d, v));
}

void PHDrawableCoordinates::clearCache()
{
    PHPositionalVector ss = coord[NULL];
    coord.clear();
    setPositionInDrawable(NULL, ss);
}

PHPositionalVector PHDrawableCoordinates::positionInDrawable(PHDrawable * d)
{
    map<PHDrawable * , PHPositionalVector>::iterator i = coord.find(d);
    if (i!=coord.end())
        return i->second;
    PHPositionalVector v = d->positionInMyCoordinates(this);
    coord.insert(make_pair(d, v));
    return v;
}

PHPoint PHDrawableCoordinates::pointInView(PHDrawable * d)
{
    return positionInDrawable(d).start.xy();
}

void PHDrawableCoordinates::propagateUpstream(PHDrawable * d, PHDrawable * until)
{
    PHPositionalVector p = positionInDrawable(d);
    while (d!=until)
    {
        p = d->positionInParent(this, p);
        d = d->parent();
        setPositionInDrawable(d, p);
    }
}

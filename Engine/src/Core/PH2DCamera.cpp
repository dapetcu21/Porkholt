/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PH2DCamera.h>
#include <Porkholt/Core/PHView.h>

PH2DCamera::PH2DCamera() : cache(false), sz(0,0)
{
}

PHMatrix PH2DCamera::projection()
{
    if (cache)
        return mat;
    cache = true;
    PHSize s = sz;
    if (!s.x || !s.y)
    {
        PHSize ss = gm->screenBounds().size();
        if (s.x)
            s.y = s.x/ss.x*ss.y;
        else if (s.y)
            s.x = s.y/ss.y*ss.x;
        else
            s = ss;
    }

    return mat = PHMatrix::translation(PHPoint(-1, -1)) * PHMatrix::scaling(2/s.x, 2/s.y, 1);
}

void PH2DCamera::reshape()
{
    cache = false;
    PHRect b = gm->screenBounds();
    PHSize d = b.size() - gm->oldScreenBounds().size(); 
    for (list<PHDrawable*>::iterator i = _children.begin(); i!= _children.end(); i++)
        if ((*i)->isView())
            ((PHView*)(*i))->autoresizeMyself(b, d);
}

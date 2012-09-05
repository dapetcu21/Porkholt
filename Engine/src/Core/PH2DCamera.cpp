/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PH2DCamera.h>

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
        s = gm->screenBounds().size();
    return mat = PHMatrix::translation(PHPoint(-1, -1)) * PHMatrix::scaling(2/s.x, 2/s.y, 1);
}

void PH2DCamera::reshape()
{
    cache = false;
}

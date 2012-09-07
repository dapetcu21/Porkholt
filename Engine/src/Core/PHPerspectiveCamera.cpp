/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PHPerspectiveCamera.h>

PHPerspectiveCamera::PHPerspectiveCamera() : cache(false), ar(0), near(1), far(100), fov(45) {}

PHMatrix PHPerspectiveCamera::projection()
{
    if (cache)
        return mat;
    cache = true;
    ph_float a = ar;
    if (!a)
        a = gm->screenWidth() / gm->screenHeight();
    return mat = PHMatrix::perspective(fov, a, near, far);
}

void PHPerspectiveCamera::reshape()
{
    cache = false;
}

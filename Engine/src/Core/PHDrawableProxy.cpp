/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PHDrawableProxy.h>

PHDrawableProxy::PHDrawableProxy(PHDrawable * d) : _parentDraw(d)
{
}

PHDrawableProxy::~PHDrawableProxy()
{
}

void PHDrawableProxy::parentDestroyed()
{
    _parentDraw = NULL;
    cb.call(this);
}

void PHDrawableProxy::render()
{
    if (_parentDraw)
        _parentDraw->render();
}

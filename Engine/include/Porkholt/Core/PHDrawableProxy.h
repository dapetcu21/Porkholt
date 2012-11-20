/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHDRAWABLEPROXY_H
#define PHDRAWABLEPROXY_H

#include <Porkholt/Core/PHDrawable.h>

class PHDrawableProxy : public PHDrawable
{
    protected:
        PHDrawable * _parentDraw;
        PHDrawableProxy(PHDrawable * d);
        PHInvocation cb;

        void parentDestroyed(); 
        friend class PHDrawable;
    public:
        ~PHDrawableProxy();

        PHDrawable * parentDrawable() { return _parentDraw; }
        void setParentDestroyedCallback(const PHInvocation & inv) { cb = inv; }
        void render();
};

#endif

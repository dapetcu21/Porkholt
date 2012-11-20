/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGRIPPLES_H
#define IGRIPPLES_H

#include "IGProp.h"
#include <Porkholt/Core/PHGLUniformStates.h>

class PHGLTexture2D;
class PHPostProcess;
class PHTextureCanvas;

class IGRipples : public IGProp
{
    protected:
        PHDrawable * loadDrawable();
        PHGLTexture2D * disperse[2];
        PHGLTexture2D * displace[3];

        PHPostProcess * disv, * difv;
        PHTextureCanvas * distort;

        PHGLUniformStates::uniform * umask, * uoldmask, * umap, * umap2, * utex;

        void onFrame();
    public:
        IGRipples(IGWorld * w);
        ~IGRipples();

        void addToDistortion(IGObject * obj);

        const char * luaClass() { return "IGRipples"; } 
        static void loadLuaInterface(IGScripting * s);
};

#endif

/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGWALLCELL_H
#define IGWALLCELL_H

#include "IGDampingProp.h"

class b2Joint;
class PHImageView;

class IGWallCell : public IGDampingProp
{
    protected:
        PHImageView * iv;
        bool _flipped;
        void configureDrawable(PHDrawable * d);
    public:
        IGWallCell(IGWorld * w);
        ~IGWallCell();

        bool flipped() { return _flipped; }
        void setFlipped(bool b);

        void attachedToWorld();
        b2Joint * weldToObject(IGObject * obj, bool last = false);

        const char * luaClass() { return "IGWallCell"; } 
        static void loadLuaInterface(IGScripting * s);
};

#endif

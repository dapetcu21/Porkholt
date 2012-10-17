/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGIMAGEPROP_Hlobal
#define IGIMAGEPROP_H

#include "IGProp.h"

class PHImageView;
class IGImageProp : public IGProp
{
    protected:
        PHImageView * iv;
        void configureDrawable(PHDrawable * d);
    public:
        IGImageProp(IGWorld * world);
        virtual ~IGImageProp();
        const char * luaClass() { return "IGImageProp"; }

        PHImageView * imageView() { return iv; }

        static void loadLuaInterface(IGScripting * s); 
};

#endif

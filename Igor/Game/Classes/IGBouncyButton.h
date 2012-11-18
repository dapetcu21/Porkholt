/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGBOUNCYBUTTON_H
#define IGBOUNCYBUTTON_H

#include "IGDampingProp.h"
#include <Porkholt/Core/PHDrawable.h>

class PHImageView;

class IGBouncyButton : public IGDampingProp, PHDrawableInputDelegate
{
    protected:
        void configureDrawable(PHDrawable * d);
        string img;
        PHImageView * iv;
        ph_float time;
        PHSize _rotSize;
        PHSize _size;
        PHPoint _target;
    public:
        IGBouncyButton(IGWorld * w);
        ~IGBouncyButton();

        void attachedToWorld();

        PHSize size() { return _size; }
        void setSize(const PHSize & s) { _size = s; }
        PHSize rotationSize() { return _rotSize; }
        void setRotationSize(const PHSize & sz) { _rotSize = sz; }
        PHPoint target() { return _target; }
        void setTarget(const PHPoint & t);
        
        const string & image() { return img; }
        void setImage(const string & imageName);

        void dismiss();

        void animate(ph_float);
        void drawableRecievedEvent(PHDrawable * d, PHEvent * evt);

        const char * luaClass() { return "IGBouncyButton"; } 
        static void loadLuaInterface(IGScripting * s);
};

#endif

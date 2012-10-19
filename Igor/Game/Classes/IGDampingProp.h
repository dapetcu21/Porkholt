/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGDAMPINGPROP_H
#define IGDAMPINGPROP_H

#include "IGProp.h"

class b2Joint;
class IGDampingProp : public IGProp
{
    protected:
        b2MouseJoint * cui;
        PHVector2 dampImpulse;
        ph_float maxF, dampTorque, dampFreq;
    public:
        IGDampingProp(IGWorld * w);
        ~IGDampingProp();

        void setPosition(const PHPoint & p);
        void setRotation(ph_float r);
        void createDampingJoint();

        void setDampingForce(ph_float p);
        ph_float dampingForce() { return maxF; }
        void setDampingTorque(ph_float t) { dampTorque = t; }
        ph_float dampingTorque() { return dampTorque; }
        void setDampingFrequency(ph_float f);
        ph_float dampingFrequency() { return dampFreq; }

        virtual void adjustPhysics(ph_float elapsed);
        virtual void animate(ph_float elapsed);
        virtual const char * luaClass() { return "IGDampingProp"; }
        static void loadLuaInterface(IGScripting * s);
};

#endif

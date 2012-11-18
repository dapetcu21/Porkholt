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
        PHVector2 desiredPos;
        ph_float desiredRot;
        ph_float maxF, dampTorque, dampFreq;
        ph_float foresight;
        bool dampEnabled, rr;
    public:
        IGDampingProp(IGWorld * w);
        ~IGDampingProp();

        void setPosition(const PHPoint & p);
        void setPositionImmediately(const PHPoint & p);
        void setRotation(ph_float r);
        PHVector2 desiredPosition() { return desiredPos; }
        ph_float desiredRotation() { return desiredRot; }
        void createDampingJoint();

        void setRestrictRotation(bool r) { rr = r; }
        bool restrictRotation() { return rr; }

        void setDampingForce(ph_float p);
        ph_float dampingForce() { return maxF; }
        void setDampingTorque(ph_float t) { dampTorque = t; }
        ph_float dampingTorque() { return dampTorque; }
        void setDampingForesight(ph_float t) { foresight = t; }
        ph_float dampingForesight() { return foresight; }
        void setDampingFrequency(ph_float f);
        ph_float dampingFrequency() { return dampFreq; }
        void setDamping(bool damp) { dampEnabled = damp; }
        bool isDamping() { return dampEnabled; }

        virtual void adjustPhysics(ph_float elapsed);
        virtual void animate(ph_float elapsed);
        virtual const char * luaClass() { return "IGDampingProp"; }
        static void loadLuaInterface(IGScripting * s);
};

#endif

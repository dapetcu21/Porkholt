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
        float desiredRot;
        float maxF, dampTorque, dampFreq;
        float foresight;
        bool dampEnabled, rr;
    public:
        IGDampingProp(IGWorld * w);
        ~IGDampingProp();

        void setPosition(const PHPoint & p);
        void setPositionImmediately(const PHPoint & p);
        void setRotation(float r);
        PHVector2 desiredPosition() { return desiredPos; }
        float desiredRotation() { return desiredRot; }
        void createDampingJoint();

        bool restrictRotation() { return rr; }
        void setRestrictRotation(bool r) { rr = r; }
        bool restrictTranslation() { return dampEnabled; }
        void setRestrictTranslation(bool damp) { dampEnabled = damp; }

        void setDampingForce(float p);
        float dampingForce() { return maxF; }
        void setDampingTorque(float t) { dampTorque = t; }
        float dampingTorque() { return dampTorque; }
        void setDampingForesight(float t) { foresight = t; }
        float dampingForesight() { return foresight; }
        void setDampingFrequency(float f);
        float dampingFrequency() { return dampFreq; }

        virtual void adjustPhysics(float elapsed);
        virtual void animate(float elapsed);
        virtual const char * luaClass() { return "IGDampingProp"; }
        static void loadLuaInterface(IGScripting * s);
};

#endif

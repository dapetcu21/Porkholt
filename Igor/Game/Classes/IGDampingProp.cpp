/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Box2D/Box2D.h>
#include "IGDampingProp.h"
#include "IGScripting.h"
#include "IGWorld.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHGameManager.h>

IGSCRIPTING_REGISTERCLASS("IGDampingProp", IGDampingProp)

IGDampingProp::IGDampingProp(IGWorld * w) : IGProp(w), cui(NULL), dampImpulse(0, 0), maxF(20), dampTorque(30), dampFreq(3), desiredPos(0, 0), desiredRot(0), dampEnabled(true), rr(true), foresight(0.33)
{
}

IGDampingProp::~IGDampingProp()
{
}

void IGDampingProp::setRotation(ph_float r)
{
    if (!body || !rr)
        IGProp::setRotation(r);
    desiredRot = r;
}

void IGDampingProp::setPosition(const PHPoint & p)
{
    if (body && cui)
    {
        dampImpulse += (p-desiredPos);
        cui->SetTarget(p.b2d());
    } else 
        IGProp::setPosition(p);
    desiredPos = p;
}

void IGDampingProp::setPositionImmediately(const PHPoint & p)
{
    IGProp::setPosition(p);
}

void IGDampingProp::createDampingJoint()
{
    if (!dampEnabled) return;
    if (!body) return;
    b2MouseJointDef def;
    def.bodyB = body;
    def.bodyA = world->groundBody();
    def.maxForce = maxF;
    def.frequencyHz = dampFreq;
    def.target = desiredPos.b2d();
    cui = (b2MouseJoint*)world->physicsWorld()->CreateJoint(&def);
}

void IGDampingProp::adjustPhysics(ph_float elapsed)
{
    if (body)
    {
        if (rr)
        {
            ph_float totalRotation = PHNormalizeAngle(desiredRot - body->GetAngle()) - body->GetAngularVelocity() * foresight;
            ph_float desiredAngularVelocity = totalRotation / elapsed;
            ph_float change = dampTorque * elapsed;
            desiredAngularVelocity = min( change, max(-change, desiredAngularVelocity));
            ph_float impulse = body->GetInertia() * desiredAngularVelocity;
            body->ApplyAngularImpulse(impulse);
        }

        if (cui)
        {
            dampImpulse *= body->GetMass() / elapsed;
            body->ApplyLinearImpulse(dampImpulse.b2d(), body->GetWorldCenter());
        }
    }
}

void IGDampingProp::animate(ph_float elapsed)
{
    IGProp::animate(elapsed);
    if (body && cui)
    {
        body->ApplyLinearImpulse(-dampImpulse.b2d(), body->GetWorldCenter());
        dampImpulse = PHVector2(0, 0);
    }
}

void IGDampingProp::setDampingForce(ph_float f)
{
    if (cui)
        cui->SetMaxForce(f);
    maxF = f;
}

void IGDampingProp::setDampingFrequency(ph_float f)
{
    if (cui)
        cui->SetFrequency(f);
    dampFreq = f;
}

//--- Lua Scripting ---

PHLuaNumberGetter(IGDampingProp, dampingForce);
PHLuaNumberGetter(IGDampingProp, dampingTorque);
PHLuaNumberGetter(IGDampingProp, dampingForesight);
PHLuaNumberGetter(IGDampingProp, dampingFrequency);
PHLuaNumberSetter(IGDampingProp, setDampingForce);
PHLuaNumberSetter(IGDampingProp, setDampingTorque);
PHLuaNumberSetter(IGDampingProp, setDampingForesight);
PHLuaNumberSetter(IGDampingProp, setDampingFrequency);
PHLuaPointGetter(IGDampingProp, desiredPosition);
PHLuaNumberGetter(IGDampingProp, desiredRotation);
PHLuaBoolGetter(IGDampingProp, restrictTranslation);
PHLuaBoolSetter(IGDampingProp, setRestrictTranslation);
PHLuaBoolGetter(IGDampingProp, restrictRotation);
PHLuaBoolSetter(IGDampingProp, setRestrictRotation);
PHLuaPointSetter(IGDampingProp, setPositionImmediately);

void IGDampingProp::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGDampingProp");

    PHLuaAddMethod(IGDampingProp, dampingForce);
    PHLuaAddMethod(IGDampingProp, setDampingForce);
    PHLuaAddMethod(IGDampingProp, dampingForce);
    PHLuaAddMethod(IGDampingProp, setDampingTorque);
    PHLuaAddMethod(IGDampingProp, dampingFrequency);
    PHLuaAddMethod(IGDampingProp, dampingForesight);
    PHLuaAddMethod(IGDampingProp, setDampingForesight);
    PHLuaAddMethod(IGDampingProp, setDampingFrequency);
    PHLuaAddMethod(IGDampingProp, desiredPosition);
    PHLuaAddMethod(IGDampingProp, setPositionImmediately);
    PHLuaAddMethod(IGDampingProp, desiredRotation);
    PHLuaAddMethod(IGDampingProp, restrictTranslation);
    PHLuaAddMethod(IGDampingProp, setRestrictTranslation);
    PHLuaAddMethod(IGDampingProp, restrictRotation);
    PHLuaAddMethod(IGDampingProp, setRestrictRotation);

    lua_pop(L, 1);
}


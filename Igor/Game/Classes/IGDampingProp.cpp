/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Box2D/Box2D.h>
#include "IGDampingProp.h"
#include "IGScripting.h"
#include "IGWorld.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHGameManager.h>

IGSCRIPTING_REGISTERCLASS("IGDampingProp", IGDampingProp)

IGDampingProp::IGDampingProp(IGWorld * w) : IGProp(w), cui(NULL), dampImpulse(0, 0), maxF(10), dampTorque(10), dampFreq(3)
{
}

IGDampingProp::~IGDampingProp()
{
}

void IGDampingProp::setRotation(ph_float r)
{
    if (body && cui)
    {
        rot = r;
    } else
        IGProp::setRotation(r);
}

void IGDampingProp::setPosition(const PHPoint & p)
{
    if (body && cui)
    {
        dampImpulse += (p-pos);
        cui->SetTarget(p.b2d());
        pos = p;
    } else 
        IGProp::setPosition(p);
}

void IGDampingProp::createDampingJoint()
{
    if (!body) return;
    b2MouseJointDef def;
    def.bodyB = body;
    def.bodyA = world->groundBody();
    def.maxForce = maxF;
    def.frequencyHz = dampFreq;
    def.target = pos.b2d();
    cui = (b2MouseJoint*)world->physicsWorld()->CreateJoint(&def);
}

void IGDampingProp::adjustPhysics(ph_float elapsed)
{
    if (body && cui)
    {
        ph_float totalRotation = PHNormalizeAngle(rot - body->GetAngle()) - body->GetAngularVelocity() / dampFreq;
        ph_float desiredAngularVelocity = totalRotation / elapsed;
        ph_float change = dampTorque * elapsed;
        desiredAngularVelocity = min( change, max(-change, desiredAngularVelocity));
        ph_float impulse = body->GetInertia() * desiredAngularVelocity;
        body->ApplyAngularImpulse(impulse);

        dampImpulse *= body->GetMass() / elapsed;
        body->ApplyLinearImpulse(dampImpulse.b2d(), body->GetWorldCenter());
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

void IGDampingProp::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGDampingProp");

    lua_pop(L, 1);
}


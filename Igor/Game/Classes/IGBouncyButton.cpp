/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGBouncyButton.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Box2D/Box2D.h>

IGSCRIPTING_REGISTERCLASS("IGBouncyButton", IGBouncyButton)

IGBouncyButton::IGBouncyButton(IGWorld * w) : IGDampingProp(w), iv(NULL), img("placeholder"), time(0), _rotSize(0.1, 0.1), _size(1, 1), _target(0, 0)
{
}

IGBouncyButton::~IGBouncyButton()
{
    if (iv)
        iv->release();
}

void IGBouncyButton::attachedToWorld()
{
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(pos.x, pos.y);
    def.angle = rot;
    def.userData = this;
    body = world->physicsWorld()->CreateBody(&def);

    b2CircleShape shape;
    shape.m_radius = _size.width/2;

    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.density = 1.0f;
    fdef.filter.categoryBits = IGWorld::collisionEnv;
    fdef.filter.maskBits = IGWorld::collisionAllEnv;
    body->CreateFixture(&fdef);

    time = rand()/(float)RAND_MAX * M_PI * 2;

    createDampingJoint();
}

void IGBouncyButton::configureDrawable(PHDrawable * d)
{
    iv = new PHImageView(PHRect(-_size.x/2, -_size.y/2, _size.x, _size.y));
    iv->setImage(world->gameManager()->imageNamed(img));
    iv->setInputDelegate(this);
    iv->setUserInput(true);
    d->setUserInput(true);
    d->addChild(iv);
}

void IGBouncyButton::setImage(const string & imageName)
{
    img = imageName;
    if (iv)
        iv->setImage(world->gameManager()->imageNamed(img));
}

void IGBouncyButton::setTarget(const PHPoint & p)
{
    _target = p;
    setPosition(_target + PHVector2(cos(time) * _rotSize.x, sin(time) * _rotSize.y));
}

void IGBouncyButton::animate(ph_float e)
{
    time += e*1.5;
    if (time > M_PI*2)
        time -= M_PI*2;
    setPosition(_target + PHVector2(cos(time) * _rotSize.x, sin(time) * _rotSize.y));
    IGDampingProp::animate(e);
}

void IGBouncyButton::drawableRecievedEvent(PHDrawable * d, PHEvent * evt)
{
    if (evt->type() == PHEvent::touchDown)
    {
        evt->setHandled(true);
        return;
    }
    if (evt->type() == PHEvent::touchUp && PHPointInRect(evt->drawableLocation()->pointInView((PHView*)d), ((PHView*)d)->bounds()))
    {
        PHLuaGetWeakRef(L, this);
        if (lua_istable(L, -1))
        {
            lua_getfield(L, -1, "onTap");
            if (lua_isfunction(L, -1))
            {
                lua_pushvalue(L, -2);
                PHLuaCall(L, 1, 0);
            } else
                lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }
}

void IGBouncyButton::dismiss()
{
    if (iv)
    {
        iv->beginCinematicAnimation(rand()/(float)RAND_MAX * 0.5);
        iv->chainCinematicAnimation(0.5f, PHCinematicAnimator::FadeInFunction);
        iv->animateScale(PHSize(1.0/1024, 1.0/1024));
        iv->animationCallback(PHInvBind(this, IGObject::removeFromWorld, NULL));
        iv->commitCinematicAnimation();
    } else
        removeFromWorld();
}

//--- Lua Scripting ---

PHLuaPointGetter(IGBouncyButton, size);
PHLuaPointSetter(IGBouncyButton, setSize);
PHLuaPointGetter(IGBouncyButton, rotationSize);
PHLuaPointSetter(IGBouncyButton, setRotationSize);
PHLuaPointGetter(IGBouncyButton, target);
PHLuaPointSetter(IGBouncyButton, setTarget);
PHLuaStringGetter(IGBouncyButton, image);
PHLuaStringSetter(IGBouncyButton, setImage);
PHLuaDefineCall(IGBouncyButton, dismiss);

void IGBouncyButton::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGBouncyButton");

    PHLuaAddMethod(IGBouncyButton, size);
    PHLuaAddMethod(IGBouncyButton, setSize);
    PHLuaAddMethod(IGBouncyButton, rotationSize);
    PHLuaAddMethod(IGBouncyButton, setRotationSize);
    PHLuaAddMethod(IGBouncyButton, target);
    PHLuaAddMethod(IGBouncyButton, setTarget);
    PHLuaAddMethod(IGBouncyButton, image);
    PHLuaAddMethod(IGBouncyButton, setImage);
    PHLuaAddMethod(IGBouncyButton, dismiss);

    lua_pop(L, 1);
}

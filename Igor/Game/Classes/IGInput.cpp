/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGInput.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>

IGSCRIPTING_REGISTERCLASS("IGInput", IGInput)

IGInput::IGInput(IGWorld * w) : IGObject(w), firing(0), moveud(NULL)
{
}

IGInput::~IGInput()
{
}

PHDrawable * IGInput::loadDrawable()
{
    PHDrawable * d = new PHView(world->view()->bounds());
    d->setUserInput(true);
    d->setInputDelegate(this);
    return d;
}

void IGInput::drawableRecievedEvent(PHDrawable * d, PHEvent * evt)
{
    bool firsthalf;
    switch (evt->type())
    {
        case PHEvent::touchDown:
            firsthalf = evt->drawableLocation()->pointInView(d).x < ((PHView*)d)->boundsCenter().x;
            if (firsthalf)
            {
                if (moveud)
                    return;
                moveud = evt->userData();
                evt->setHandled(true);
            } else {
                firing++;
                evt->setHandled(true);
            }
            break;
        case PHEvent::touchUp:
        case PHEvent::touchCancelled:
            if (evt->userData() == moveud)
            {
                moveud = NULL;
            } else {
                firing--;
            }
            break;
        case PHEvent::touchMoved:
            if (evt->userData() == moveud)
            {
                PHPoint lp = evt->lastDrawableLocation()->pointInView(d);
                PHPoint p = evt->drawableLocation()->pointInView(d);
                PHPoint delta = p-lp;
                if (scripting)
                {
                    PHLuaGetWeakRef(L, this);
                    if (lua_istable(L, -1))
                    {
                        lua_getfield(L, -1, "touchMoved");
                        if (lua_isfunction(L, -1))
                        {
                            lua_pushvalue(L, -2);
                            delta.saveToLua(L);
                            PHLuaCall(L, 2, 0);
                        } else
                            lua_pop(L, 1);
                    }
                    lua_pop(L, 1);
                }
            }
            break;
    }
}

//--- Lua Interface ---

PHLuaBoolGetter(IGInput, isFiring);

void IGInput::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGInput");

    PHLuaAddMethod(IGInput, isFiring);

    lua_pop(L, 1);
}

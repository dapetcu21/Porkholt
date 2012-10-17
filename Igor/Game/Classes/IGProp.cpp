/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGProp.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHTransformDrawable.h>
#include <Porkholt/Core/PHLua.h>

IGSCRIPTING_REGISTERCLASS("IGProp", IGProp)

IGProp::IGProp(IGWorld * world) : IGObject(world)
{
}

IGProp::~IGProp()
{
}

void IGProp::setPosition(const PHPoint & p)
{
    if (body)
    {
    } else {
        PHTransformDrawable * td = dynamic_cast<PHTransformDrawable*>(td);
        if (td)
            td->setTranslation(p);
    }
    pos = p;
}

PHDrawable * IGProp::loadDrawable()
{
    PHTransformDrawable * td = new PHTransformDrawable;
    configureDrawable(td);
    return td;
}

//--- Lua Interface ---

PHLuaPointGetter(IGProp, position)
PHLuaPointSetter(IGProp, setPosition)

void IGProp::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGProp");

    PHLuaAddMethod(IGProp, position);
    PHLuaAddMethod(IGProp, setPosition);
}


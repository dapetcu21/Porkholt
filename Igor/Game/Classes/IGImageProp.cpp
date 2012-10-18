/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGImageProp.h"
#include "IGScripting.h"
#include "IGWorld.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHImageView.h>

IGSCRIPTING_REGISTERCLASS("IGImageProp", IGImageProp)

IGImageProp::IGImageProp(IGWorld * world) : IGProp(world), iv(NULL)
{
    iv = new PHImageView();
}

IGImageProp::~IGImageProp()
{
    iv->release();
}

void IGImageProp::configureDrawable(PHDrawable * d)
{
    d->addChild(iv);
}

static int IGImageProp_setImage(lua_State * L)
{
    IGImageProp * p = (IGImageProp*)PHLuaThisPointer(L);
    p->imageView()->setImage(p->getWorld()->gameManager()->imageNamed(string(lua_tostring(L, 2))));
    return 0;
}

static int IGImageProp_setBounds(lua_State * L)
{
    IGImageProp * p = (IGImageProp*)PHLuaThisPointer(L);
    p->imageView()->setFrame(PHRect::fromLua(L, 2));
    return 0;
}

static int IGImageProp_bounds(lua_State * L)
{
    IGImageProp * p = (IGImageProp*)PHLuaThisPointer(L);
    p->imageView()->frame().saveToLua(L);
    return 1;
}

//--- Lua Interface ---

void IGImageProp::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGImageProp");

    PHLuaAddMethod(IGImageProp, setImage)
    PHLuaAddMethod(IGImageProp, setBounds)
    PHLuaAddMethod(IGImageProp, bounds)

    lua_pop(L, 1);
}

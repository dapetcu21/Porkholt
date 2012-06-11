/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHDIALOG_H
#define PHDIALOG_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHLua.h>

class PHDialog : public PHObject
{
private:
    lua_State * L;
public:
    PHLNPC * npc;
    string text;
    bool inStack;
    bool cbck;
    PHDialog() : npc(NULL), text(), L(NULL), inStack(false), cbck(false) {}
    virtual void callback()
    {
        if (cbck) return;
        cbck = true;
        _callback();
    }
    virtual void _callback()
    {
        if (!L) return;
        lua_getglobal(L, "PHCallbackHelper");
        PHLuaGetHardRef(L, this);
        PHLuaCall(L, 1, 0);
        PHLuaDeleteHardRef(L, this);
        L = NULL;
    }
    void setCallback(lua_State * l)
    {
        L = l;
        PHLuaSetHardRef(L, this);
    }
    ~PHDialog()
    {
        if (L)
            PHLuaDeleteHardRef(L, this);
    }
};


#endif

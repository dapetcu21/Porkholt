//
//  PHDialog.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHDIALOG_H
#define PHDIALOG_H

#include "PHMain.h"
#include "PHLua.h"

class PHDialog : public PHObject
{
private:
    lua_State * L;
public:
    PHLNPC * npc;
    string text;
    PHDialog() : npc(NULL), text(), L(NULL) {}
    void callback()
    {
        if (!L) return;
        lua_getglobal(L, "PHDialog_runCallback");
        PHLuaGetHardRef(L, this);
        PHLuaCall(L, 1, 0);
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

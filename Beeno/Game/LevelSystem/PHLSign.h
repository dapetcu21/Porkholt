/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLSIGN_H
#define PHLSIGN_H

#include "PHLNPC.h"

class PHDialog;
class PHLSign : public PHLNPC
{
private:
    string text;
    list<PHDialog*> dialogs;
    void advance();
    friend class PHSignDialog;
public:
    PHLSign();
    virtual ~PHLSign();
    
    void display() { display(NULL); }
    void display(lua_State * L);
    
    void loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc);
    
    static void registerLuaInterface(lua_State * L);
};

#endif

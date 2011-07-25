//
//  PHLSign.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/25/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

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
    
    void loadFromLua(lua_State * L, const string & root, b2World * world);
    
    static void registerLuaInterface(lua_State * L);
};

#endif

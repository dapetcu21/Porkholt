/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLUACONSOLE_H
#define PHLUACONSOLE_H

#include "PHTCPConsole.h"

class PHLuaConsole : public PHTCPConsole
{
private:
    lua_State * L;
protected:
    void autoCompleteCLI();
    bool pharseCLI();
public:
    PHLuaConsole(lua_State * L);
    
    virtual void clientConnected();
    virtual void clientDisconnected();
};

#endif

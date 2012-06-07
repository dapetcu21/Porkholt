/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLPOWERUP_H
#define PHLPOWERUP_H

#include "PHLSensor.h"

class PHLPowerup : public PHLSensor
{
private:
    void _collected();
    bool dismissing;
    ph_float remaining;
    virtual void collected();
    virtual void objectEntered(PHLObject * obj);
    void dismissed();
public:
    
    virtual void updatePosition();
    
    PHLPowerup();
    void dismiss();
    void collect();
    static void registerLuaInterface(lua_State * L);
};

#endif

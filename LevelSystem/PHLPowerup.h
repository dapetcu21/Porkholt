//
//  PHLPowerup.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/25/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLPOWERUP_H
#define PHLPOWERUP_H

#include "PHLSensor.h"

class PHLPowerup : public PHLSensor
{
private:
    void _collected();
    bool dismissing;
    double remaining;
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

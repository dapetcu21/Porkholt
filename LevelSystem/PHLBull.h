//
//  PHLBull.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/24/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLBULL_H
#define PHLBULL_H

#include "PHLMob.h"

class PHLBull : public PHLMob
{
public:
    PHLBull();
    virtual ~PHLBull();
    
    static void registerLuaInterface(lua_State * L);
};

#endif

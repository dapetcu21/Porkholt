//
//  PHLMob.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/24/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLMOB_H
#define PHLMOB_H

#include "PHLNPC.h"

class PHLMob: public PHLNPC
{
public:
    PHLMob();
    virtual ~PHLMob();
    
    static void registerLuaInterface(lua_State * L);
};

#endif

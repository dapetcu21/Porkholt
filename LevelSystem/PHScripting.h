//
//  PHScripting.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 6/27/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHSCRIPTING_H
#define PHSCRIPTING_H

#include "PHMain.h"

class PHWorld;
struct lua_State;

class PHScripting : public PHObject
{
private:
    PHWorld * world;
    lua_State * L;
public:
    PHScripting(PHWorld * world,string level_dir);
    ~PHScripting();
    void scriptingStep(double timeElapsed);
    void loadLevelController();
};

#endif
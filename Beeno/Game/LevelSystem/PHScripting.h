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

#if defined(PH_SIMULATOR) || (defined(PH_DESKTOP) && defined (PH_DEBUG))
#define PH_SCRIPTING_CONSOLE
class PHLuaConsole;
#endif

class PHWorld;
struct lua_State;

class PHScripting : public PHObject
{
private:
    PHWorld * world;
    lua_State * L;
public:
    PHScripting(PHWorld * world,string level_dir);
    virtual ~PHScripting();
    void scriptingStep(ph_float timeElapsed);
    void loadWorld();
        
    void worldHasFadedAway(void * ud);
    
    lua_State * luaState() { return L; }
    
#ifdef PH_SCRIPTING_CONSOLE
public:
    void executeConsole();
private:
    PHLuaConsole * console;
#endif
    
};

#endif
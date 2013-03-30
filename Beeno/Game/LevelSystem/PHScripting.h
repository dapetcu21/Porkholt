/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSCRIPTING_H
#define PHSCRIPTING_H

#include <Porkholt/Core/PHMain.h>

#if defined(PH_SIMULATOR) || (defined(PH_DESKTOP) && defined (PH_DEBUG))
#define PH_SCRIPTING_CONSOLE
class PHLuaConsole;
#endif

class PHWorld;
struct lua_State;
class PHDirectory;

class PHScripting : public PHObject
{
private:
    PHWorld * world;
    lua_State * L;
public:
    PHScripting(PHWorld * world, PHDirectory * level_dir);
    virtual ~PHScripting();
    void scriptingStep(float timeElapsed);
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

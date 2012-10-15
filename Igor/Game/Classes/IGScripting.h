/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHAnimator.h>

struct lua_State;
class PHDirectory;
class PHGameManager;
class IGWorld;

class IGScripting : public PHAnimator
{
    protected:
        lua_State * L;
        PHDirectory * dir;
        PHGameManager * gm;
        IGWorld * world;

        void loadCInterface();
    public:
        IGScripting(PHGameManager * gm, PHDirectory * dir, IGWorld * world);
        ~IGScripting();

        lua_State * luaState() { return L; }

        void advanceAnimation(ph_float elapsed);


        static map<string, PHAllocator> * luaClasses;
};

#define IGSCRIPTING_REGISTERCLASS(name, clss) PH_REGISTERCLASS(IGScripting::luaClasses)

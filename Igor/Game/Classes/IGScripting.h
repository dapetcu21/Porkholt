/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGSCRIPTING_H
#define IGSCRIPTING_H

#include <Porkholt/Core/PHAnimator.h>

struct lua_State;
class PHDirectory;
class PHGameManager;
class IGWorld;
class IGObject;
class IGScripting;

typedef void (*IGScriptingIface)(IGScripting *);
typedef IGObject * (*IGScriptingAllocator)(IGWorld * w);

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
        PHPoint screenSize();

        void advanceAnimation(ph_float elapsed);

        static map<string, IGScriptingAllocator> * luaClasses;
        static list<IGScriptingIface> * luaInterfaces;

    //lua functions, don't use
        void classFromName(const string & s); 
};


#define IG_REGISTERLUAINTERFACE(clss) PH_INITIALIZER( PH_TOKENPASTE(PH_TOKENPASTE(IGScriptingRegister_, clss), PH_UNIQUE_TOKEN)) {\
    if (!IGScripting::luaInterfaces) \
        IGScripting::luaInterfaces = new list<IGScriptingIface>;\
    IGScripting::luaInterfaces->push_back(&clss::loadLuaInterface); \
}

#define IG_REGISTERCLASS(name,clss) \
IGObject * PH_TOKENPASTE(IGScriptingAlloc_, clss) (IGWorld * w) \
{ \
    return new clss(w); \
} \
PH_INITIALIZER( PH_TOKENPASTE(PH_TOKENPASTE(IGScriptingRegisterC_, clss), PH_UNIQUE_TOKEN)) {\
    if (!IGScripting::luaClasses) \
        IGScripting::luaClasses = new map<string,IGScriptingAllocator>;\
    IGScripting::luaClasses->insert(make_pair(name,(IGScriptingAllocator)&PH_TOKENPASTE(IGScriptingAlloc_, clss)));\
}
 
#define IGSCRIPTING_REGISTERCLASS(name, clss) \
IG_REGISTERCLASS(name, clss) \
IG_REGISTERLUAINTERFACE(clss)

#endif

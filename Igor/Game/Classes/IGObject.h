/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>

class IGScripting;
class IGWorld;
class PHDrawable;
struct lua_State;

class IGObject : public PHObject
{
    protected:
        IGWorld * world;
        lua_State * L;
        IGScripting * scripting;
        PHDrawable * drawable;
        list<IGObject*>::iterator world_pos;

    public:
        IGObject(IGWorld * world);
        ~IGObject();

        void attachToWorld(IGWorld * w, bool before = true, IGObject * ref = NULL);

        virtual const char * luaClass() { return "IGObject"; } 
        static void initLuaState(IGScripting * scripting);

        void getLuaObject(IGScripting * scripting);
        PHDrawable * getDrawable();

        virtual void animate(ph_float elapsed);

    protected:
        virtual void attachedToWorld();
        virtual PHDrawable * loadDrawable();
};

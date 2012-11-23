/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGOBJECT_H
#define IGOBJECT_H

#include <Porkholt/Core/PHMain.h>

class IGScripting;
class IGWorld;
class PHDrawable;
struct lua_State;
class b2Body;

class IGObject : public PHObject
{
    protected:
        IGWorld * world;
        lua_State * L;
        IGScripting * scripting;
        PHDrawable * drawable;
        list<IGObject*>::iterator world_pos;
        friend class IGWorld;

    public:
        IGObject(IGWorld * world);
        virtual ~IGObject();

        void attachToWorld(IGWorld * w, bool before = true, IGObject * ref = NULL);
        void removeFromWorld();

        virtual const char * luaClass() { return "IGObject"; } 
        static void loadLuaInterface(IGScripting * scripting);

        void getLuaObject(IGScripting * scripting);
        PHDrawable * getDrawable();

        virtual void animate(ph_float elapsed);
        virtual void adjustPhysics(ph_float elapsed);

        virtual b2Body * physicsBody() { return NULL; }
        virtual void setPhysicsBody(b2Body * b) {}

        IGWorld * getWorld() { return world; }

        IGScripting * scriptingContext() { return scripting; }

    protected:
        virtual void attachedToWorld();
        virtual PHDrawable * loadDrawable();
};

#endif

/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHOBJECT_H
#define PHOBJECT_H

#include <Porkholt/Core/PHUtilities.h>

//#define VIRTUAL_PHOBJECT
//#define DEBUG_ZOMBIES
//#define DEBUG_ALLOCATIONS
#define LUA_INTERFACE
//#define PH_PROFILING

#ifdef VIRTUAL_PHOBJECT
#define PHOBJECT_PREFIX virtual
#else
#define PHOBJECT_PREFIX
#endif

#ifdef DEBUG_ALLOCATIONS
#include <Porkholt/Core/PHAllocationProfiler.h>
#endif

class PHInvocation;

class PHObject
{
private:
    int _refcount;
    void * inv;

    void zombie();
public:
	PHObject(): _refcount(1), inv(NULL)
    {
#ifdef DEBUG_ALLOCATIONS
        PHAllocationProfiler::singleton().objectRetain(this);
#endif
    }
	PHOBJECT_PREFIX void retain() 
    { 
        _refcount++;
#ifdef DEBUG_ALLOCATIONS
        PHAllocationProfiler::singleton().objectRetain(this);
#endif
    };
	PHOBJECT_PREFIX PHObject * release() 
    { 
#ifdef DEBUG_ALLOCATIONS
        PHAllocationProfiler::singleton().objectRelease(this);
#endif
        _refcount--; 
        if (!_refcount) 
        { 
#ifdef DEBUG_ZOMBIES
            this->~PHObject();
#else
            delete this; 
#endif
            return NULL; 
        }
        if (_refcount<0) 
        {
            zombie();
            return NULL;
        }
        return this; 
    };
    void autorelease();

    void clearInvocations();
    void bindInvocation(PHInvocation * inv);
    void unbindInvocation(PHInvocation * inv);

	virtual ~PHObject() { if (inv) clearInvocations(); }
	int referenceCount() { return _refcount; };

#ifdef LUA_INTERFACE
    void pushLuaInstance(lua_State * L, bool strongReference = true);
    void releaseLuaInterface(lua_State * L);
    std::string luaClass();
#endif

};

#endif

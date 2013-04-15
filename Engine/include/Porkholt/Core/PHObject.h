/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHOBJECT_H
#define PHOBJECT_H

#include <Porkholt/Core/PHUtilities.h>

//#define VIRTUAL_PHOBJECT
//#define DEBUG_ZOMBIES
//#define DEBUG_ALLOCATIONS
#define LUA_INTERFACE
//#define PH_PROFILING

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
	void retain() 
    { 
        _refcount++;
#ifdef DEBUG_ALLOCATIONS
        PHAllocationProfiler::singleton().objectRetain(this);
#endif
    };
	PHObject * release() 
    { 
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
#ifdef DEBUG_ALLOCATIONS
        else
            PHAllocationProfiler::singleton().objectRelease(this);
#endif
        if (_refcount<0) 
        {
            zombie();
            return NULL;
        }
        return this; 
    };
    void fakeRelease()
    {
#ifdef DEBUG_ALLOCATIONS 
        PHAllocationProfiler::singleton().objectRelease(this);
#endif
    }
    void fakeRetain()
    {
#ifdef DEBUG_ALLOCATIONS 
        PHAllocationProfiler::singleton().objectRetain(this);
#endif
    }
    void autorelease();

    void clearInvocations();
    void bindInvocation(PHInvocation * inv);
    void unbindInvocation(PHInvocation * inv);

	virtual ~PHObject() 
    { 
        if (inv) clearInvocations(); 
#ifdef DEBUG_ALLOCATIONS
        PHAllocationProfiler::singleton().objectDealloc(this);
#endif
    }
	int referenceCount() { return _refcount; };

#ifdef LUA_INTERFACE
    void pushLuaInstance(lua_State * L, bool strongReference = true);
    void releaseLuaInterface(lua_State * L);
    std::string luaClass();
#endif

};

#endif

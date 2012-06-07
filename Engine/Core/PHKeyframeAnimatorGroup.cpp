/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <set>
#include "PHLua.h"
#include "PHKeyframeAnimator.h"
#include "PHKeyframeAnimatorGroup.h"

map<string, PHKeyframeAnimatorGroup*> PHKeyframeAnimatorGroup::groups;

void PHKeyframeAnimatorGroup::addKeyframeAnimator(PHKeyframeAnimator * anim)
{
    if (!anim) return;
    if (keyframeAnimators.insert(anim).second)
        anim->addIntoGroup(this);
}

void PHKeyframeAnimatorGroup::removeKeyframeAnimator(PHKeyframeAnimator * anim)
{
    if (!anim) return;
    if (keyframeAnimators.erase(anim))
        anim->removeFromGroup(this);
}


void PHKeyframeAnimatorGroup::playSection(const string & name, const PHInvocation & callback, ph_float seek)
{
    PHInvocation inv = callback;
    for (set<PHKeyframeAnimator*>::iterator i = keyframeAnimators.begin(); i != keyframeAnimators.end(); i++)
    {
        if (filterEnabled && ((*i)->tag()!=filter))
            continue;
        (*i)->playSection(name,inv,seek);
        if (inv.valid())
            inv = PHInvocation();
    }
}

void PHKeyframeAnimatorGroup::playSection(int section, const PHInvocation & callback, ph_float seek)
{
    PHInvocation inv = callback;
    for (set<PHKeyframeAnimator*>::iterator i = keyframeAnimators.begin(); i != keyframeAnimators.end(); i++)
    {
        if (filterEnabled && ((*i)->tag()!=filter))
            continue;
        (*i)->playSection(section,inv,seek);
        if (inv.valid())
            inv = PHInvocation();
    }
}

void PHKeyframeAnimatorGroup::pause()
{
    for (set<PHKeyframeAnimator*>::iterator i = keyframeAnimators.begin(); i != keyframeAnimators.end(); i++)
    {
        if (filterEnabled && ((*i)->tag()!=filter))
            continue;
        (*i)->pause();
    }
}

void PHKeyframeAnimatorGroup::resume()
{
    for (set<PHKeyframeAnimator*>::iterator i = keyframeAnimators.begin(); i != keyframeAnimators.end(); i++)
    {
        if (filterEnabled && ((*i)->tag()!=filter))
            continue;
        (*i)->resume();
    }
}

PHKeyframeAnimatorGroup::PHKeyframeAnimatorGroup() : PHLuaObject("PHKeyframeAnimatorGroup"), filter(0), filterEnabled(false) 
{
    
}

PHKeyframeAnimatorGroup::~PHKeyframeAnimatorGroup()
{
    for (set<PHKeyframeAnimator*>::iterator i = keyframeAnimators.begin(); i != keyframeAnimators.end(); i++)
        (*i)->removeFromGroup(this);
    for (set<PHLuaCallback*>::iterator i = callbacks.begin(); i!=callbacks.end(); i++)
        (*i)->release();
}

PHKeyframeAnimatorGroup * PHKeyframeAnimatorGroup::groupNamed(const string & name)
{
    map<string, PHKeyframeAnimatorGroup*>::iterator i = groups.find(name);
    if (i == groups.end())
    {
        PHKeyframeAnimatorGroup * g = new PHKeyframeAnimatorGroup();
        groups.insert(make_pair<string, PHKeyframeAnimatorGroup*>(name, g));
        return g;
    }
    return i->second;
}

void PHKeyframeAnimatorGroup::releaseGroupNamed(const string & name)
{
    map<string, PHKeyframeAnimatorGroup*>::iterator i = groups.find(name);
    if (i == groups.end()) return;
    i->second->release();
    groups.erase(i);
}

PHLuaDefineCall(PHKeyframeAnimatorGroup, pause);
PHLuaDefineCall(PHKeyframeAnimatorGroup, resume);
PHLuaDefineCall(PHKeyframeAnimatorGroup, clearFilterTag);
static int PHKeyframeAnimatorGroup_setFilterTag(lua_State * L)
{
    PHKeyframeAnimatorGroup * a = (PHKeyframeAnimatorGroup*)PHLuaThisPointer(L);
    if (lua_isnumber(L, 2))
        a->setFilterTag((int)lua_tonumber(L, 2));
    else
        a->clearFilterTag();
    return 0;
}
static int PHKeyframeAnimatorGroup_groupNamed(lua_State * L)
{
    luaL_checkstring(L, 1);
    PHKeyframeAnimatorGroup * g = PHKeyframeAnimatorGroup::groupNamed(lua_tostring(L, 1));
    g->luaInstance(L,g);
    return 1;
}

void PHKeyframeAnimatorGroup::removeLuaCallback(PHLuaCallback * callback)
{
    if (callbacks.erase(callback))
        callback->release();
}

void PHKeyframeAnimatorGroup::addLuaCallback(PHLuaCallback * callback)
{
    callbacks.insert(callback);
}

static int PHKeyframeAnimatorGroup_playSection(lua_State * L)
{
    PHKeyframeAnimatorGroup * a = (PHKeyframeAnimatorGroup*)PHLuaThisPointer(L);
    PHInvocation inv;
    if (lua_isfunction(L, 3))
    {
        PHLuaCallback * cb = new PHLuaCallback(L,3);
        a->addLuaCallback(cb);
        cb->setConsequentCallback(PHInv(a,PHKeyframeAnimatorGroup::removeLuaCallback,cb));
        inv = PHInv(cb,PHLuaCallback::call,NULL);
    }
    ph_float seek = 0;
    if (lua_isnumber(L, 4))
        seek = lua_tonumber(L, 4);
    if (lua_isnumber(L, 2))
        a->playSection((int)lua_tonumber(L, 2),inv,seek);
    else
        if (lua_isstring(L, 2))
            a->playSection(lua_tostring(L, 2),inv,seek);
    return 0;
}


void PHKeyframeAnimatorGroup::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHKeyframeAnimatorGroup");
    
    PHLuaAddMethod(PHKeyframeAnimatorGroup, pause);
    PHLuaAddMethod(PHKeyframeAnimatorGroup, resume);
    PHLuaAddMethod(PHKeyframeAnimatorGroup, setFilterTag);
    PHLuaAddMethod(PHKeyframeAnimatorGroup, clearFilterTag);
    PHLuaAddMethod(PHKeyframeAnimatorGroup, playSection);
    PHLuaAddMethod(PHKeyframeAnimatorGroup, groupNamed);
    
    lua_pop(L, 1);
}

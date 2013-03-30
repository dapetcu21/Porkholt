/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHKEYFRAMEANIMATORGROUP_H
#define PHKEYFRAMEANIMATORGROUP_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHLuaObject.h>

class PHKeyframeAnimator;
class PHLuaCallback;
class PHKeyframeAnimatorGroup : public PHObject, public PHLuaObject
{
private:
    size_t filter;
    bool filterEnabled;
    
    set<PHKeyframeAnimator*> keyframeAnimators;
    
public:
    PHKeyframeAnimatorGroup();
    virtual ~PHKeyframeAnimatorGroup();
    
    void addKeyframeAnimator(PHKeyframeAnimator * anim);
    void removeKeyframeAnimator(PHKeyframeAnimator * anim);
    
    
    void playSection(const string & name) {
        playSection(name, PHInvocation(), 0.0f); }
    void playSection(int section) {
        playSection(section, PHInvocation(), 0.0f); }
    void playSection(const string & name, const PHInvocation & callback) { 
        playSection(name, callback, 0.0f); }
    void playSection(int section, const PHInvocation & callback) {
        playSection(section, callback, 0.0f); }
    void playSection(const string & name, const PHInvocation & callback, float seek);
    void playSection(int section, const PHInvocation & callback, float seek);
    
    void pause();
    void resume();
    
    void setFilterTag(size_t tag) { filter = tag; filterEnabled = true; }
    void clearFilterTag() { filterEnabled = false; }
    
private:
    set<PHLuaCallback*> callbacks;
public:
    void removeLuaCallback(PHLuaCallback * callback);
    void addLuaCallback(PHLuaCallback * callback);
    
public:
    static PHKeyframeAnimatorGroup * groupNamed(const string & name);
    static void releaseGroupNamed(const string & name);
    
    static void registerLuaInterface(lua_State * L);
private:
    static map<string, PHKeyframeAnimatorGroup*> groups;
};

#endif

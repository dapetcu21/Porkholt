//
//  PHKeyframeAnimator.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 1/8/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHKeyframeAnimator.h"
#include "PHLua.h"
#include "PHCinematicActor.h"

void PHKeyframeAnimator::Keyframe::addPosition(const PHPoint & pos)
{
    flags|= hasPosition;
    position = pos;
}

void PHKeyframeAnimator::Keyframe::addRotation(double rot)
{
    flags|= hasRotation;
    rotation = rot;
}

void PHKeyframeAnimator::Keyframe::addScale(const PHSize & scl)
{
    flags|= hasScale;
    scale = scl;
}

void PHKeyframeAnimator::Keyframe::addBgColor(const PHColor & clr)
{
    flags|= hasBgColor;
    bgColor = clr;
}

void PHKeyframeAnimator::Keyframe::addCustomColor(const PHColor & clr)
{
    flags|= hasCustomColor;
    customColor = clr;
}

void PHKeyframeAnimator::Keyframe::addCustomValue(double val)
{
    flags|= hasCustomValue;
    customValue = val;
}

void PHKeyframeAnimator::Keyframe::addJump(int sectionIndex, double time)
{
    flags|= hasJump;
    jmpSec = sectionIndex;
    jmpTime = time;
}

PHKeyframeAnimator::Keyframe PHKeyframeAnimator::positionFrame(double tm, const PHPoint & pos)
{
    Keyframe k(tm);
    k.addPosition(pos);
    return k;
}

PHKeyframeAnimator::Keyframe PHKeyframeAnimator::rotationFrame(double tm, double rot)
{
    Keyframe k(tm);
    k.addRotation(rot);
    return k;
}

PHKeyframeAnimator::Keyframe PHKeyframeAnimator::scaleFrame(double tm, const PHSize & scale)
{
    Keyframe k(tm);
    k.addScale(scale);
    return k;
}

PHKeyframeAnimator::Keyframe PHKeyframeAnimator::bgColorFrame(double tm, const PHColor & clr)
{
    Keyframe k(tm);
    k.addBgColor(clr);
    return k;
}

PHKeyframeAnimator::Keyframe PHKeyframeAnimator::customColorFrame(double tm, const PHColor & clr)
{
    Keyframe k(tm);
    k.addCustomColor(clr);
    return k;
}

PHKeyframeAnimator::Keyframe PHKeyframeAnimator::customValueFrame(double tm, double val)
{
    Keyframe k(tm);
    k.addCustomValue(val);
    return k;
}

PHKeyframeAnimator::Keyframe PHKeyframeAnimator::nullFrame(double tm) 
{ 
    return Keyframe(tm);
}

PHKeyframeAnimator::Keyframe PHKeyframeAnimator::jumpFrame(double tm, int sectionIndex, double time)
{
    Keyframe k(tm);
    k.addJump(sectionIndex, time);
    return k;
}


PHKeyframeAnimator::PHKeyframeAnimator(const vector<Section> & s) : playing(false), section(-1), sections(s)
{
    
}

int PHKeyframeAnimator::sectionWithName(const string & name)
{
    for(int i=0; i<sections.size(); i++)
        if (name == sections[i].name)
            return i;
    return -1;
}

void PHKeyframeAnimator::advanceNexts(int frame)
{
    vector<Keyframe> & v = sections[section].frames;
    size_t n = v.size();
    
#define advnext(ivar,kflag) \
    if (ivar <= frame) \
        ivar = frame+1; \
    while (ivar<n && ((v[ivar].flags & Keyframe::kflag)==0)) \
        ++ivar;
    
    advnext(nextPos,hasPosition);
    advnext(nextRot,hasRotation);
    advnext(nextScale,hasScale);
    advnext(nextBgColor,hasBgColor);
    advnext(nextCColor,hasCustomColor);
    advnext(nextCValue,hasCustomValue);
}

void PHKeyframeAnimator::playSection(int sec, const PHInvocation & callback, double seek)
{
    section = sec;
    if (section>=sections.size() || section<0)
    {
        section = -1;
        return;
    }
    time = seek;
        
    //frame = search(seek)
    vector<Keyframe> & v = sections[section].frames;
    size_t n = v.size(), step, i;
    for (step = 1; step < n; step <<= 1);
    for (i = 0; step; step >>= 1)
        if (i + step < n && v[i+step].time < seek)
            i += step;
    if (v[i].time < seek)
        i++;
    if (i==n)
        i = -1;
    frame = i;
    
    if (frame == -1)
    {
        section = -1;
        return;
    }
    nextPos = nextRot = nextScale = nextBgColor = nextCColor = nextCValue = frame;
    advanceNexts(frame-1);
    playing = true;
}

void PHKeyframeAnimator::advanceAnimation(double elapsedTime)
{
    if (!(playing && _actor && section>=0 && frame>=0))
        return;
    
    vector<Keyframe> & v = sections[section].frames;
    size_t n = v.size();
    
    while (elapsedTime>0)
    {
        double nextTime;
        bool touch = (elapsedTime>=v[frame].time-time);
        if (touch)
            nextTime = v[frame].time;
        else
            nextTime = time+elapsedTime;
        double delta = nextTime-time;
        elapsedTime-=delta;
        
#define advanceProperty(next,type,actorgetter,actorsetter,framegetter) \
        if (next>=0 && next<n) \
        { \
            type keyval = v[next].framegetter; \
            if ((v[next].time-time == 0) || ( touch && (frame==next))) \
                {_actor->actorsetter(keyval); PHLog("pula");} \
            else { \
                type old = _actor->actorgetter(); \
                _actor->actorsetter(old+(keyval-old)*(delta/(v[next].time-time))); \
            } \
        }
        
#define advancePropertyColor(next,actorgetter,actorsetter,framegetter) \
        if (next>=0 && next<n) \
        { \
            PHColor keyval = v[next].framegetter; \
            if ((v[next].time-time == 0) || ( touch && (frame==next))) \
                {_actor->actorsetter(keyval); PHLog("pula");} \
            else { \
                PHColor old = _actor->actorgetter(); \
                if (!old.isValid()) old = PHColor(1,1,1,0); \
                double f = (delta/(v[next].time-time)); \
                old.r += f*(keyval.r-old.r); \
                old.g += f*(keyval.g-old.g); \
                old.b += f*(keyval.b-old.b); \
                old.a += f*(keyval.a-old.a); \
                _actor->actorsetter(old); \
            } \
        }
        
        advanceProperty(nextPos,PHPoint,cinematicPosition,setCinematicPosition,position);
        advanceProperty(nextRot, double, cinematicRotation, setCinematicRotation, rotation);
        advanceProperty(nextScale, PHSize, cinematicScale, setCinematicScale, scale);
        advancePropertyColor(nextBgColor, cinematicBgColor, setCinematicBgColor, bgColor);
        advancePropertyColor(nextCColor, cinematicCustomColor, setCinematicCustomColor, customColor);
        advanceProperty(nextCValue, double, cinematicCustomValue, setCinematicCustomValue, customValue);
        
        time = nextTime;
        if (touch)
        {
            if (v[frame].flags & Keyframe::hasJump)
            {
                playSection(v[frame].jmpSec, PHInvocation(callback), v[frame].jmpTime);
                advanceAnimation(elapsedTime);
                return;
            } 
            advanceNexts(frame);
            frame++;
            if (frame>=n)
            {
                callback.call(this);
                section = -1;
                return;
            }
        }
    }
}

PHKeyframeAnimator * PHKeyframeAnimator::fromLua(lua_State * L)
{
    PHKeyframeAnimator * a = NULL;
    if (!lua_istable(L, -1)) return NULL;
    
    lua_getfield(L, -1, "sections");
    if (lua_istable(L, -1))
    {
        int sections_index = lua_gettop(L);
        lua_pushnil(L);
        int i = 0;
        while (lua_next(L, sections_index) != 0)
        {
            if (lua_istable(L, -1))
            {
                lua_pushnumber(L, i++);
                lua_setfield(L, -2, "index");
            }
            lua_pop(L, 1);
        }
        
        vector<Section> s;
        lua_pushnil(L);
        while (lua_next(L, sections_index) != 0)
        {
            vector<Keyframe> k;
            if (lua_istable(L, -1))
            {
                int n = 0;
                PHLuaGetNumberField(n, "n");
                for (int i=0; i<n; i++)
                {
                    lua_pushnumber(L, i);
                    lua_gettable(L, -2);
                    
                    if (lua_istable(L, -1))
                    {
                        lua_getfield(L, -1, "time");
                        if (lua_isnumber(L,-1))
                        {
                            Keyframe kf(nullFrame(lua_tonumber(L, -1)));
                            lua_pop(L, 1);
                            
                            lua_getfield(L, -1, "rotation");
                            if (lua_isnumber(L, -1))
                                kf.addRotation(-toRad(lua_tonumber(L, -1)));
                            lua_pop(L, 1);
                            
                            lua_getfield(L, -1, "customValue");
                            if (lua_isnumber(L, -1))
                                kf.addCustomValue(lua_tonumber(L, -1));
                            lua_pop(L, 1);
                            
                            lua_getfield(L, -1, "position");
                            if (lua_istable(L, -1))
                                kf.addPosition(PHPoint::fromLua(L, -1));
                            lua_pop(L, 1);
                            
                            lua_getfield(L, -1, "scale");
                            if (lua_istable(L, -1))
                                kf.addScale(PHSize::fromLua(L, -1));
                            lua_pop(L, 1);
                            
                            lua_getfield(L, -1, "bgColor");
                            if (lua_istable(L, -1))
                                kf.addBgColor(PHColor::fromLua(L, -1));
                            lua_pop(L, 1);
                            
                            lua_getfield(L, -1, "customColor");
                            if (lua_istable(L, -1))
                                kf.addCustomColor(PHColor::fromLua(L, -1));
                            lua_pop(L, 1);
                            
                            lua_getfield(L, -1, "jumpSection");
                            if (!lua_isnil(L, -1))
                            {    
                                lua_gettable(L, sections_index);
                                if (lua_istable(L, -1))
                                    lua_getfield(L, -1, "index");
                                else
                                    lua_pushnil(L);
                                lua_getfield(L, -3, "jumpTime");
                                if (lua_isnumber(L, -2) && lua_isnumber(L, -1))
                                    kf.addJump((int)lua_tonumber(L, -2), lua_tonumber(L, -1));
                                lua_pop(L, 3);
                            } else 
                                lua_pop(L, 1);
                            
                            k.push_back(kf);
                        } else
                            lua_pop(L, 1);
                    }
                    lua_pop(L,1);
                }
            }
            lua_pop(L, 1);
            if (!k.empty())
            {
                string nm("");
                lua_pushvalue(L, -1);
                if (lua_isstring(L, -1))
                    nm = lua_tostring(L, -1);
                lua_pop(L, 1);
                s.push_back(Section(nm,k));
            }
        }
        if (!s.empty())
            a = new PHKeyframeAnimator(s);
    }
    lua_pop(L, 1);
    
    return a;
}


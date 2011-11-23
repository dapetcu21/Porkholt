//
//  PHParticleAnimator.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/23/11.
//  Copyright (c) 2011 Porkholt Labs!. All rights reserved.
//

#include "PHParticleAnimator.h"
#include "PHLua.h"

PHParticleAnimator * PHParticleAnimator::fromLua(lua_State *L, bool unique)
{
    if (!L) return NULL;
    if (lua_istable(L, -1))
    {
        if (unique)
        {
            lua_getfield(L, -1, "ud");
            if (lua_islightuserdata(L, -1))
            {
                PHParticleAnimator * p = (PHParticleAnimator*)lua_touserdata(L, -1);
                p->retain();
                lua_pop(L,1);
                return p;
            }
            lua_pop(L,1);
        }
        
        PHParticleAnimator * p = new PHParticleAnimator;
        lua_pushvalue(L, -1);
        PHLuaSetHardRef(L, p);
        p->L = L;
        lua_pushlightuserdata(L, p);
        lua_setfield(L, -2, "ud");
        
        PHMessage::messageWithName("luaDestroy")->addListener(p, (PHCallback)&PHParticleAnimator::luaDestroy);
        
        p->loadFromLua(L);
        
        return p;
    }
    return NULL;
}

void PHParticleAnimator::luaDestroy(PHObject * sender, void * ud)
{
    if ((lua_State*)ud == L)
        L = NULL;
}

PHParticleAnimator::~PHParticleAnimator()
{
    mutex->lock();
    PHMessage::messageWithName("luaDestroy")->removeListener(this);
    if (L)
        PHLuaDeleteHardRef(L, this);
    clear();
    mutex->unlock();
    delete mutex;
}

void PHParticleAnimator::loadFromLua(lua_State *L)
{
    if (!lua_istable(L, -1)) return;
    mutex->lock();
    PHLuaGetBoolField(playing,"isPlaying");
    PHLuaGetBoolField(generating,"isGenerating");
    PHLuaGetNumberField(genFor,"generatingFor");
    PHLuaGetNumberField(pps,"particlesPerSecond");
    PHLuaGetRectField(genArea,"generationArea");
    PHLuaGetBoolField(elArea,"elipticalArea");
    PHLuaGetPointField(vel,"velocity");
    PHLuaGetNumberField(deltavel,"velocityVariation");
    PHLuaGetAngleField(spreadAngl,"spreadAngle");
    PHLuaGetPointField(grav,"gravity");
    PHLuaGetPointField(initSize,"initialSize");
    PHLuaGetPointField(endSize,"finalSize");
    PHLuaGetNumberField(lifetime,"particleLifetime");
    PHLuaGetNumberField(deltalifetime,"lifetimeVariation");
    PHLuaGetColorField(initColor,"initialColor");
    PHLuaGetColorField(endColor,"finalColor");
    PHLuaGetNumberField(rotQuot,"rotationQuotient");
    mutex->unlock();
}

PHLuaBoolGetter(PHParticleAnimator,isPlaying);
PHLuaBoolSetter(PHParticleAnimator,setPlaying);
PHLuaBoolGetter(PHParticleAnimator,isGenerating);
PHLuaBoolSetter(PHParticleAnimator,setGenerating);
PHLuaNumberGetter(PHParticleAnimator,generatingFor);
PHLuaNumberSetter(PHParticleAnimator,setGeneratingFor);
PHLuaNumberGetter(PHParticleAnimator,particlesPerSecond);
PHLuaNumberSetter(PHParticleAnimator,setParticlesPerSecond);
PHLuaRectGetter(PHParticleAnimator,generationArea);
PHLuaRectSetter(PHParticleAnimator,setGenerationArea);
PHLuaBoolGetter(PHParticleAnimator,elipticalArea);
PHLuaBoolSetter(PHParticleAnimator,setElipticalArea);
PHLuaPointGetter(PHParticleAnimator,velocity);
PHLuaPointSetter(PHParticleAnimator,setVelocity);
PHLuaNumberGetter(PHParticleAnimator,velocityVariation);
PHLuaNumberSetter(PHParticleAnimator,setVelocityVariation);
PHLuaAngleGetter(PHParticleAnimator,spreadAngle);
PHLuaAngleSetter(PHParticleAnimator,setSpreadAngle);
PHLuaPointGetter(PHParticleAnimator,gravity);
PHLuaPointSetter(PHParticleAnimator,setGravity);
PHLuaPointGetter(PHParticleAnimator,initialSize);
PHLuaPointSetter(PHParticleAnimator,setInitSize);
PHLuaPointGetter(PHParticleAnimator,finalSize);
PHLuaPointSetter(PHParticleAnimator,setFinalSize);
PHLuaNumberGetter(PHParticleAnimator,particleLifetime);
PHLuaNumberSetter(PHParticleAnimator,setParticleLifetime);
PHLuaNumberGetter(PHParticleAnimator,lifetimeVariation);
PHLuaNumberSetter(PHParticleAnimator,setLifetimeVariation);
PHLuaColorGetter(PHParticleAnimator,initialColor);
PHLuaColorSetter(PHParticleAnimator,setInitColor);
PHLuaColorGetter(PHParticleAnimator,finalColor);
PHLuaColorSetter(PHParticleAnimator,setFinalColor);
PHLuaNumberGetter(PHParticleAnimator,rotationQuotient);
PHLuaNumberSetter(PHParticleAnimator,setRotationQuotient);
PHLuaDefineCall(PHParticleAnimator,play);
PHLuaDefineCall(PHParticleAnimator,pause);
PHLuaDefineCall(PHParticleAnimator,clear);

void PHParticleAnimator::registerLuaInterface(lua_State *L)
{
    lua_getglobal(L, "PHParticleAnimator");
    if (lua_istable(L, -1))
    {
        PHLuaAddMethod(PHParticleAnimator,isPlaying);
        PHLuaAddMethod(PHParticleAnimator,setPlaying);
        PHLuaAddMethod(PHParticleAnimator,isGenerating);
        PHLuaAddMethod(PHParticleAnimator,setGenerating);
        PHLuaAddMethod(PHParticleAnimator,generatingFor);
        PHLuaAddMethod(PHParticleAnimator,setGeneratingFor);
        PHLuaAddMethod(PHParticleAnimator,particlesPerSecond);
        PHLuaAddMethod(PHParticleAnimator,setParticlesPerSecond);
        PHLuaAddMethod(PHParticleAnimator,generationArea);
        PHLuaAddMethod(PHParticleAnimator,setGenerationArea);
        PHLuaAddMethod(PHParticleAnimator,elipticalArea);
        PHLuaAddMethod(PHParticleAnimator,setElipticalArea);
        PHLuaAddMethod(PHParticleAnimator,velocity);
        PHLuaAddMethod(PHParticleAnimator,setVelocity);
        PHLuaAddMethod(PHParticleAnimator,velocityVariation);
        PHLuaAddMethod(PHParticleAnimator,setVelocityVariation);
        PHLuaAddMethod(PHParticleAnimator,spreadAngle);
        PHLuaAddMethod(PHParticleAnimator,setSpreadAngle);
        PHLuaAddMethod(PHParticleAnimator,gravity);
        PHLuaAddMethod(PHParticleAnimator,setGravity);
        PHLuaAddMethod(PHParticleAnimator,initialSize);
        PHLuaAddMethod(PHParticleAnimator,setInitSize);
        PHLuaAddMethod(PHParticleAnimator,finalSize);
        PHLuaAddMethod(PHParticleAnimator,setFinalSize);
        PHLuaAddMethod(PHParticleAnimator,particleLifetime);
        PHLuaAddMethod(PHParticleAnimator,setParticleLifetime);
        PHLuaAddMethod(PHParticleAnimator,lifetimeVariation);
        PHLuaAddMethod(PHParticleAnimator,setLifetimeVariation);
        PHLuaAddMethod(PHParticleAnimator,initialColor);
        PHLuaAddMethod(PHParticleAnimator,setInitColor);
        PHLuaAddMethod(PHParticleAnimator,finalColor);
        PHLuaAddMethod(PHParticleAnimator,setFinalColor);
        PHLuaAddMethod(PHParticleAnimator,rotationQuotient);
        PHLuaAddMethod(PHParticleAnimator,setRotationQuotient);
        PHLuaAddMethod(PHParticleAnimator,play);
        PHLuaAddMethod(PHParticleAnimator,pause);
        PHLuaAddMethod(PHParticleAnimator,clear);
    }
    lua_pop(L,-1);
}

vector<PHParticleAnimator::particle> * PHParticleAnimator::calculatedParticles()
{
    vector<particle_state*>::iterator i;
    vector<particle> * v = new vector<particle>;
    mutex->lock();
    v->reserve(heap.size());
    for (i = heap.begin(); i!=heap.end(); i++)
        v->push_back((*i)->particle);
    mutex->unlock();
    return v;
}

bool PHParticleAnimator::hp_cmp(particle_state * p1, particle_state * p2)
{
    return p2->lifespan < p1->lifespan;
}

void PHParticleAnimator::hp_swap(int i, int j)
{
    particle_state * aux = heap[i];
    heap[i] = heap[j];
    heap[j] = aux;
}

void PHParticleAnimator::hp_down(int i)
{
    int n = heap.size();
    while (1)
    {
        int max = i;
        int p1 = (i<<1)+1;
        int p2 = (i<<1)+2;
        if (p1<n && hp_cmp(heap[max], heap[p1]))
            max = p1;
        if (p2<n && hp_cmp(heap[max], heap[p2]))
            max = p2;
        if (max==i) break;
        hp_swap(max, i);
        i = max;
    }
}

void PHParticleAnimator::hp_up(int i)
{
    while (i)
    {
        int p = ((i+1)>>1)-1;
        if (!hp_cmp(heap[p], heap[i]))
            break;
        hp_swap(i, p);
        i = p;
    }
}

PHParticleAnimator::particle_state * PHParticleAnimator::hp_top()
{
    if (heap.empty()) 
        return NULL;
    return heap[0];
}

PHParticleAnimator::particle_state * PHParticleAnimator::hp_pop()
{
    if (heap.empty()) 
        return NULL;
    particle_state * t = heap[0];
    heap[0] = heap[heap.size()-1];
    heap.pop_back();
    hp_down(0);
    return t;
}

void PHParticleAnimator::hp_push(particle_state * p)
{
    heap.push_back(p);
    hp_up(heap.size()-1);
}

void PHParticleAnimator::clear()
{
    for (vector<particle_state*>::iterator i = heap.begin(); i!=heap.end(); i++)
        delete *i;
    heap.clear();
}

PHParticleAnimator::PHParticleAnimator() : mutex(new PHMutex), playing(true), generating(true), genFor(INFINITY), pps(5), genArea(PHRect(0,0,0,0)), elArea(false), vel(PHOriginPoint), deltavel(0), spreadAngl(0), grav(0,-9.81), initSize(0.05,0.05), endSize(0.05,0.05), lifetime(1), deltalifetime(0), initColor(PHWhiteColor), endColor(PHWhiteColor), rotQuot(0)
{
    particle_state * ps;
    ps = new particle_state;
    ps->particle.color = PHWhiteColor;
    ps->particle.position = PHPoint(0,0);
    ps->particle.rotation = 0;
    ps->particle.size = PHSize(0.05,0.05);
    heap.push_back(ps);
    
    ps = new particle_state;
    ps->particle.color = PHWhiteColor;
    ps->particle.position = PHPoint(0,0.5);
    ps->particle.rotation = 0;
    ps->particle.size = PHSize(0.05,0.05);
    heap.push_back(ps);
    
    ps = new particle_state;
    ps->particle.color = PHWhiteColor;
    ps->particle.position = PHPoint(0,0.25);
    ps->particle.rotation = 0;
    ps->particle.size = PHSize(0.05,0.05);
    heap.push_back(ps);
    
    ps = new particle_state;
    ps->particle.color = PHWhiteColor;
    ps->particle.position = PHPoint(0,1);
    ps->particle.rotation = 0;
    ps->particle.size = PHSize(0.05,0.05);
    heap.push_back(ps);
}

void PHParticleAnimator::advanceAnimation(double elapsedTime)
{
    mutex->lock();
    
    mutex->unlock();
}
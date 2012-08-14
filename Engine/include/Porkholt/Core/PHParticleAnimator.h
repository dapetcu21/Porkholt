/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHPARTICLEANIMATOR_H
#define PHPARTICLEANIMATOR_H

#include <Porkholt/Core/PHAnimator.h>

#define getter(type,name,prop) type name() { mutex->lock(); type r(prop); mutex->unlock(); return r; }
#define setter(type,name,prop) void name(type v) { mutex->lock(); prop = v; mutex->unlock(); }
#define setter_(type,name,prop) setter(const type & ,name,prop)

class PHParticleAnimator : public PHAnimator
{
private:
    PHMutex * mutex;
    
    bool playing,generating;
    ph_float genFor;
    ph_float pps;
    PHRect genArea;
    bool elArea;
    PHPoint vel;
    ph_float deltavel;
    ph_float spreadAngl;
    PHPoint grav;
    PHSize initSize,endSize;
    ph_float lifetime;
    ph_float deltalifetime;
    PHColor initColor,endColor;
    bool rotates;
    ph_float rotoff;
    
    lua_State * L;
    
public:
    void pause() { setPlaying(false); }
    void play() { setPlaying(true); }
    void clear();
    getter(bool,isPlaying,playing);
    setter(bool,setPlaying,playing);
    getter(bool,isGenerating,generating);
    setter(bool,setGenerating,generating);
    getter(ph_float,generatingFor,genFor);
    setter(ph_float,setGeneratingFor,genFor);
    getter(ph_float,particlesPerSecond,pps);
    setter(ph_float,setParticlesPerSecond,pps);
    getter(PHRect,generationArea,genArea);
    setter_(PHRect,setGenerationArea,genArea);
    getter(bool,elipticalArea,elArea);
    setter(bool,setElipticalArea,elArea);
    getter(PHPoint,velocity,vel);
    void setVelocity(const PHPoint & p);
//    setter_(PHPoint,setVelocity,vel);
    getter(ph_float,velocityVariation,deltavel);
    setter(ph_float,setVelocityVariation,deltavel);
    getter(ph_float,spreadAngle,spreadAngl);
    setter(ph_float,setSpreadAngle,spreadAngl);
    getter(PHPoint,gravity,grav);
    setter_(PHPoint,setGravity,grav);
    getter(PHSize,initialSize,initSize);
    setter_(PHSize,setInitSize,initSize);
    getter(PHSize,finalSize,endSize);
    setter_(PHSize,setFinalSize,endSize);
    getter(ph_float,particleLifetime,lifetime);
    setter(ph_float,setParticleLifetime,lifetime);
    getter(ph_float,lifetimeVariation,deltalifetime);
    setter(ph_float,setLifetimeVariation,deltalifetime);
    getter(PHColor,initialColor,initColor);
    setter_(PHColor,setInitColor,initColor);
    getter(PHColor,finalColor,endColor);
    setter_(PHColor,setFinalColor,endColor);
    getter(bool,rotatesWithVelocity,rotates);
    setter(bool,setRotatesWithVelocity,rotates);
    getter(ph_float,rotationOffset,rotoff);
    setter(ph_float,setRotationOffset,rotoff);
    
    PHParticleAnimator();
    virtual void advanceAnimation(ph_float elapsedTime);
    virtual ~PHParticleAnimator();
    
    void luaDestroy(PHObject * sender, void * ud);
    virtual void loadFromLua(lua_State * L);
    static PHParticleAnimator * fromLua(lua_State * L) { return fromLua(L,true); }
    static PHParticleAnimator * fromLua(lua_State * L, bool unique);
    static void registerLuaInterface(lua_State * L);
    
    struct particle
    {
        PHPoint position;
        ph_float rotation;
        PHSize size;
        PHColor color;
    };
    
    vector<particle> * calculatedParticles();
    
private:
    
    struct particle_state
    {
        particle part;
        PHPoint velocity;
        ph_float lifespan;
        ph_float totalLife;
    };
    ph_float genQueue;
    
    vector<particle_state*> heap;
    
    bool hp_cmp(particle_state * p1, particle_state * p2);
    void hp_swap(int i, int j);
    void hp_down(int i);
    void hp_up(int i);
    particle_state * hp_top();
    particle_state * hp_pop();
    void hp_push(particle_state * p);
    
    void animateParticle(particle_state * p,ph_float elapsed);
};

#undef setter
#undef setter_
#undef getter

#endif

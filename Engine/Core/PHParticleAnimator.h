//
//  PHParticleAnimator.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/22/11.
//  Copyright (c) 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHPARTICLEANIMATOR_H
#define PHPARTICLEANIMATOR_H

#include "PHAnimator.h"

#define getter(type,name,prop) type name() { mutex->lock(); type r(prop); mutex->unlock(); return r; }
#define setter(type,name,prop) void name(type v) { mutex->lock(); prop = v; mutex->unlock(); }
#define setter_(type,name,prop) setter(const type & ,name,prop)

class PHParticleAnimator : public PHAnimator
{
private:
    PHMutex * mutex;
    
    bool playing,generating;
    double genFor;
    double pps;
    PHRect genArea;
    bool elArea;
    PHPoint vel;
    double deltavel;
    double spreadAngl;
    PHPoint grav;
    PHSize initSize,endSize;
    double lifetime;
    double deltalifetime;
    PHColor initColor,endColor;
    double rotQuot;
    
    lua_State * L;
    
public:
    void pause() { setPlaying(false); }
    void play() { setPlaying(true); }
    void clear();
    getter(bool,isPlaying,playing);
    setter(bool,setPlaying,playing);
    getter(bool,isGenerating,generating);
    setter(bool,setGenerating,generating);
    getter(double,generatingFor,genFor);
    setter(double,setGeneratingFor,genFor);
    getter(double,particlesPerSecond,pps);
    setter(double,setParticlesPerSecond,pps);
    getter(PHRect,generationArea,genArea);
    setter_(PHRect,setGenerationArea,genArea);
    getter(bool,elipticalArea,elArea);
    setter(bool,setElipticalArea,elArea);
    getter(PHPoint,velocity,vel);
    void setVelocity(const PHPoint & p);
//    setter_(PHPoint,setVelocity,vel);
    getter(double,velocityVariation,deltavel);
    setter(double,setVelocityVariation,deltavel);
    getter(double,spreadAngle,spreadAngl);
    setter(double,setSpreadAngle,spreadAngl);
    getter(PHPoint,gravity,grav);
    setter_(PHPoint,setGravity,grav);
    getter(PHSize,initialSize,initSize);
    setter_(PHSize,setInitSize,initSize);
    getter(PHSize,finalSize,endSize);
    setter_(PHSize,setFinalSize,endSize);
    getter(double,particleLifetime,lifetime);
    setter(double,setParticleLifetime,lifetime);
    getter(double,lifetimeVariation,deltalifetime);
    setter(double,setLifetimeVariation,deltalifetime);
    getter(PHColor,initialColor,initColor);
    setter_(PHColor,setInitColor,initColor);
    getter(PHColor,finalColor,endColor);
    setter_(PHColor,setFinalColor,endColor);
    getter(double,rotationQuotient,rotQuot);
    setter(double,setRotationQuotient,rotQuot);
    
    PHParticleAnimator();
    virtual void advanceAnimation(double elapsedTime);
    virtual ~PHParticleAnimator();
    
    void luaDestroy(PHObject * sender, void * ud);
    virtual void loadFromLua(lua_State * L);
    static PHParticleAnimator * fromLua(lua_State * L) { return fromLua(L,true); }
    static PHParticleAnimator * fromLua(lua_State * L, bool unique);
    static void registerLuaInterface(lua_State * L);
    
    struct particle
    {
        PHPoint position;
        double rotation;
        PHSize size;
        PHColor color;
    };
    
    vector<particle> * calculatedParticles();
    
private:
    
    struct particle_state
    {
        particle particle;
        PHPoint velocity;
        double lifespan;
        double totalLife;
    };
    double genQueue;
    
    vector<particle_state*> heap;
    
    bool hp_cmp(particle_state * p1, particle_state * p2);
    void hp_swap(int i, int j);
    void hp_down(int i);
    void hp_up(int i);
    particle_state * hp_top();
    particle_state * hp_pop();
    void hp_push(particle_state * p);
    
    void animateParticle(particle_state * p,double elapsed);
};

#undef setter
#undef setter_
#undef getter

#endif

/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHGENERICPARTICLEANIMATOR_H
#define PHGENERICPARTICLEANIMATOR_H

#include <Porkholt/Core/PHParticleAnimator.h>
#include <Porkholt/Core/PHMutex.h>

#define getter(type,name,prop) type name() { mutex->lock(); type r(prop); mutex->unlock(); return r; }
#define setter(type,name,prop) void name(type v) { mutex->lock(); prop = v; mutex->unlock(); }
#define setter_(type,name,prop) setter(const type & ,name,prop)

class PHGenericParticleAnimator : public PHParticleAnimator 
{
    private:
        PHMutex * mutex;
        
        bool playing,generating;
        float genFor;
        float pps;
        PHRect genArea;
        bool elArea;
        PHPoint vel;
        float deltavel;
        float spreadAngl;
        PHPoint grav;
        float deltaSize;
        PHSize initSize,endSize;
        float lifetime;
        float deltalifetime;
        PHColor initColor,endColor;
        bool rotates;
        float rotoff;
        
        lua_State * L;
        
    public:
        void pause() { setPlaying(false); }
        void play() { setPlaying(true); }
        void clear();
        getter(bool, isPlaying, playing);
        setter(bool, setPlaying, playing);
        getter(bool, isGenerating, generating);
        setter(bool, setGenerating, generating);
        getter(float, generatingFor, genFor);
        setter(float, setGeneratingFor, genFor);
        getter(float, particlesPerSecond, pps);
        setter(float, setParticlesPerSecond, pps);
        getter(PHRect, generationArea, genArea);
        setter_(PHRect, setGenerationArea, genArea);
        getter(bool, elipticalArea, elArea);
        setter(bool, setElipticalArea, elArea);
        getter(PHPoint, velocity, vel);
        void setVelocity(const PHPoint & p);
        getter(float, velocityVariation, deltavel);
        setter(float, setVelocityVariation, deltavel);
        getter(float, spreadAngle, spreadAngl);
        setter(float, setSpreadAngle, spreadAngl);
        getter(PHPoint, gravity, grav);
        setter_(PHPoint, setGravity, grav);
        getter(float, sizeVariation, deltaSize);
        setter(float, setSizeVariation, deltaSize);
        getter(PHSize, initialSize, initSize);
        setter_(PHSize, setInitSize, initSize);
        getter(PHSize, finalSize, endSize);
        setter_(PHSize, setFinalSize, endSize);
        getter(float, particleLifetime, lifetime);
        setter(float, setParticleLifetime, lifetime);
        getter(float, lifetimeVariation, deltalifetime);
        setter(float, setLifetimeVariation, deltalifetime);
        getter(PHColor, initialColor, initColor);
        setter_(PHColor, setInitColor, initColor);
        getter(PHColor, finalColor, endColor);
        setter_(PHColor, setFinalColor, endColor);
        getter(bool, rotatesWithVelocity, rotates);
        setter(bool, setRotatesWithVelocity, rotates);
        getter(float, rotationOffset, rotoff);
        setter(float, setRotationOffset, rotoff);
        
        PHGenericParticleAnimator();
        ~PHGenericParticleAnimator();
        void advanceAnimation(float elapsedTime);
        
        void luaDestroy(PHObject * sender, void * ud);
        virtual void loadFromLua(lua_State * L);

        static PHGenericParticleAnimator * fromLua(lua_State * L) { return fromLua(L,true); }
        static PHGenericParticleAnimator * fromLua(lua_State * L, bool unique);
        static void registerLuaInterface(lua_State * L);

        particles * calculatedParticles();
    private:
        particles * cache;
        bool cached;
        
        struct particle_state
        {
            particle2D part;
            PHPoint velocity;
            float lifespan;
            float totalLife;
        };
        float genQueue;
        
        vector<particle_state*> heap;
        
        bool hp_cmp(particle_state * p1, particle_state * p2);
        void hp_swap(int i, int j);
        void hp_down(int i);
        void hp_up(int i);
        particle_state * hp_top();
        particle_state * hp_pop();
        void hp_push(particle_state * p);
        
        void animateParticle(particle_state * p,float elapsed);
};

#undef setter
#undef setter_
#undef getter

#endif

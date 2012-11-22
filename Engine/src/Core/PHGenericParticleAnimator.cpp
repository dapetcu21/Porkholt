/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGenericParticleAnimator.h>
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHMessage.h>

PHGenericParticleAnimator * PHGenericParticleAnimator::fromLua(lua_State *L, bool unique)
{
    if (!L) return NULL;
    if (lua_istable(L, -1))
    {
        if (unique)
        {
            lua_getfield(L, -1, "ud");
            if (lua_islightuserdata(L, -1))
            {
                PHGenericParticleAnimator * p = (PHGenericParticleAnimator*)lua_touserdata(L, -1);
                p->retain();
                lua_pop(L,1);
                return p;
            }
            lua_pop(L,1);
        }
        
        PHGenericParticleAnimator * p = new PHGenericParticleAnimator;
        lua_pushvalue(L, -1);
        PHLuaSetHardRef(L, p);
        p->L = L;
        lua_pushlightuserdata(L, p);
        lua_setfield(L, -2, "ud");
        
        PHMessage::messageWithName("luaDestroy")->addListener(PHInvBindN(p, PHGenericParticleAnimator::luaDestroy));
        
        p->loadFromLua(L);
        
        return p;
    }
    return NULL;
}

void PHGenericParticleAnimator::luaDestroy(PHObject * sender, void * ud)
{
    if ((lua_State*)ud == L)
        L = NULL;
}

PHGenericParticleAnimator::~PHGenericParticleAnimator()
{
    mutex->lock();
    PHMessage::messageWithName("luaDestroy")->removeListener(this);
    if (L)
        PHLuaDeleteHardRef(L, this);
    clear();
    mutex->unlock();
    if (cache)
    {
        free(cache->v2d);
        delete cache;
    }
    delete mutex;
}

void PHGenericParticleAnimator::loadFromLua(lua_State *L)
{
    if (!lua_istable(L, -1)) return;
    mutex->lock();
    PHLuaGetBoolField(playing, "isPlaying");
    PHLuaGetBoolField(generating, "isGenerating");
    PHLuaGetNumberField(genFor, "generatingFor");
    PHLuaGetNumberField(pps, "particlesPerSecond");
    PHLuaGetRectField(genArea, "generationArea");
    PHLuaGetBoolField(elArea, "elipticalArea");
    PHLuaGetPointField(vel, "velocity");
    PHLuaGetNumberField(deltavel, "velocityVariation");
    PHLuaGetAngleField(spreadAngl, "spreadAngle");
    PHLuaGetPointField(grav, "gravity");
    PHLuaGetPointField(initSize, "initialSize");
    PHLuaGetNumberField(deltaSize,  "sizeVariation");
    PHLuaGetPointField(endSize, "finalSize");
    PHLuaGetNumberField(lifetime, "particleLifetime");
    PHLuaGetNumberField(deltalifetime, "lifetimeVariation");
    PHLuaGetColorField(initColor, "initialColor");
    PHLuaGetColorField(endColor, "finalColor");
    PHLuaGetBoolField(rotates, "rotatesWithVelocity");
    PHLuaGetAngleField(rotoff, "rotationOffset");
    mutex->unlock();
}

PHLuaBoolGetter(PHGenericParticleAnimator, isPlaying);
PHLuaBoolSetter(PHGenericParticleAnimator, setPlaying);
PHLuaBoolGetter(PHGenericParticleAnimator, isGenerating);
PHLuaBoolSetter(PHGenericParticleAnimator, setGenerating);
PHLuaNumberGetter(PHGenericParticleAnimator, generatingFor);
PHLuaNumberSetter(PHGenericParticleAnimator, setGeneratingFor);
PHLuaNumberGetter(PHGenericParticleAnimator, particlesPerSecond);
PHLuaNumberSetter(PHGenericParticleAnimator, setParticlesPerSecond);
PHLuaRectGetter(PHGenericParticleAnimator, generationArea);
PHLuaRectSetter(PHGenericParticleAnimator, setGenerationArea);
PHLuaBoolGetter(PHGenericParticleAnimator, elipticalArea);
PHLuaBoolSetter(PHGenericParticleAnimator, setElipticalArea);
PHLuaPointGetter(PHGenericParticleAnimator, velocity);
PHLuaPointSetter(PHGenericParticleAnimator, setVelocity);
PHLuaNumberGetter(PHGenericParticleAnimator, velocityVariation);
PHLuaNumberSetter(PHGenericParticleAnimator, setVelocityVariation);
PHLuaAngleGetter(PHGenericParticleAnimator, spreadAngle);
PHLuaAngleSetter(PHGenericParticleAnimator, setSpreadAngle);
PHLuaPointGetter(PHGenericParticleAnimator, gravity);
PHLuaPointSetter(PHGenericParticleAnimator, setGravity);
PHLuaNumberGetter(PHGenericParticleAnimator, sizeVariation);
PHLuaNumberSetter(PHGenericParticleAnimator, setSizeVariation);
PHLuaPointGetter(PHGenericParticleAnimator, initialSize);
PHLuaPointSetter(PHGenericParticleAnimator, setInitSize);
PHLuaPointGetter(PHGenericParticleAnimator, finalSize);
PHLuaPointSetter(PHGenericParticleAnimator, setFinalSize);
PHLuaNumberGetter(PHGenericParticleAnimator, particleLifetime);
PHLuaNumberSetter(PHGenericParticleAnimator, setParticleLifetime);
PHLuaNumberGetter(PHGenericParticleAnimator, lifetimeVariation);
PHLuaNumberSetter(PHGenericParticleAnimator, setLifetimeVariation);
PHLuaColorGetter(PHGenericParticleAnimator, initialColor);
PHLuaColorSetter(PHGenericParticleAnimator, setInitColor);
PHLuaColorGetter(PHGenericParticleAnimator, finalColor);
PHLuaColorSetter(PHGenericParticleAnimator, setFinalColor);
PHLuaBoolGetter(PHGenericParticleAnimator, rotatesWithVelocity);
PHLuaBoolSetter(PHGenericParticleAnimator, setRotatesWithVelocity);
PHLuaAngleGetter(PHGenericParticleAnimator, rotationOffset);
PHLuaAngleSetter(PHGenericParticleAnimator, setRotationOffset);
PHLuaDefineCall(PHGenericParticleAnimator, play);
PHLuaDefineCall(PHGenericParticleAnimator, pause);
PHLuaDefineCall(PHGenericParticleAnimator, clear);

void PHGenericParticleAnimator::registerLuaInterface(lua_State *L)
{
    lua_getglobal(L, "PHGenericParticleAnimator");
    if (lua_istable(L, -1))
    {
        PHLuaAddMethod(PHGenericParticleAnimator, isPlaying);
        PHLuaAddMethod(PHGenericParticleAnimator, setPlaying);
        PHLuaAddMethod(PHGenericParticleAnimator, isGenerating);
        PHLuaAddMethod(PHGenericParticleAnimator, setGenerating);
        PHLuaAddMethod(PHGenericParticleAnimator, generatingFor);
        PHLuaAddMethod(PHGenericParticleAnimator, setGeneratingFor);
        PHLuaAddMethod(PHGenericParticleAnimator, particlesPerSecond);
        PHLuaAddMethod(PHGenericParticleAnimator, setParticlesPerSecond);
        PHLuaAddMethod(PHGenericParticleAnimator, generationArea);
        PHLuaAddMethod(PHGenericParticleAnimator, setGenerationArea);
        PHLuaAddMethod(PHGenericParticleAnimator, elipticalArea);
        PHLuaAddMethod(PHGenericParticleAnimator, setElipticalArea);
        PHLuaAddMethod(PHGenericParticleAnimator, velocity);
        PHLuaAddMethod(PHGenericParticleAnimator, setVelocity);
        PHLuaAddMethod(PHGenericParticleAnimator, velocityVariation);
        PHLuaAddMethod(PHGenericParticleAnimator, setVelocityVariation);
        PHLuaAddMethod(PHGenericParticleAnimator, spreadAngle);
        PHLuaAddMethod(PHGenericParticleAnimator, setSpreadAngle);
        PHLuaAddMethod(PHGenericParticleAnimator, gravity);
        PHLuaAddMethod(PHGenericParticleAnimator, setGravity);
        PHLuaAddMethod(PHGenericParticleAnimator, sizeVariation);
        PHLuaAddMethod(PHGenericParticleAnimator, setSizeVariation);
        PHLuaAddMethod(PHGenericParticleAnimator, initialSize);
        PHLuaAddMethod(PHGenericParticleAnimator, setInitSize);
        PHLuaAddMethod(PHGenericParticleAnimator, finalSize);
        PHLuaAddMethod(PHGenericParticleAnimator, setFinalSize);
        PHLuaAddMethod(PHGenericParticleAnimator, particleLifetime);
        PHLuaAddMethod(PHGenericParticleAnimator, setParticleLifetime);
        PHLuaAddMethod(PHGenericParticleAnimator, lifetimeVariation);
        PHLuaAddMethod(PHGenericParticleAnimator, setLifetimeVariation);
        PHLuaAddMethod(PHGenericParticleAnimator, initialColor);
        PHLuaAddMethod(PHGenericParticleAnimator, setInitColor);
        PHLuaAddMethod(PHGenericParticleAnimator, finalColor);
        PHLuaAddMethod(PHGenericParticleAnimator, setFinalColor);
        PHLuaAddMethod(PHGenericParticleAnimator, rotatesWithVelocity);
        PHLuaAddMethod(PHGenericParticleAnimator, setRotatesWithVelocity);
        PHLuaAddMethod(PHGenericParticleAnimator, rotationOffset);
        PHLuaAddMethod(PHGenericParticleAnimator, setRotationOffset);
        PHLuaAddMethod(PHGenericParticleAnimator, play);
        PHLuaAddMethod(PHGenericParticleAnimator, pause);
        PHLuaAddMethod(PHGenericParticleAnimator, clear);
    }
    lua_pop(L, -1);
}

PHGenericParticleAnimator::particles * PHGenericParticleAnimator::calculatedParticles()
{
    mutex->lock();
    if (!cached)
    {
        cached = true;
        if (!cache)
        {
            cache = new particles;
            cache->n = 0;
            cache->v2d = NULL;
        }
        size_t n = heap.size();
        if (n > cache->n)
            cache->v2d = (particle2D*)realloc(cache->v2d, sizeof(particle2D)*n);
        cache->n = n;
        for (int i = 0; i < n; i++)
            cache->v2d[i] = heap[i]->part;
    }
    particles * p = cache;
    mutex->unlock();
    return p;
}

bool PHGenericParticleAnimator::hp_cmp(particle_state * p1, particle_state * p2)
{
    return p2->lifespan < p1->lifespan;
}

void PHGenericParticleAnimator::hp_swap(int i, int j)
{
    particle_state * aux = heap[i];
    heap[i] = heap[j];
    heap[j] = aux;
}

void PHGenericParticleAnimator::hp_down(int i)
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

void PHGenericParticleAnimator::hp_up(int i)
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

PHGenericParticleAnimator::particle_state * PHGenericParticleAnimator::hp_top()
{
    if (heap.empty()) 
        return NULL;
    return heap[0];
}

PHGenericParticleAnimator::particle_state * PHGenericParticleAnimator::hp_pop()
{
    if (heap.empty()) 
        return NULL;
    particle_state * t = heap[0];
    heap[0] = heap[heap.size()-1];
    heap.pop_back();
    hp_down(0);
    return t;
}

void PHGenericParticleAnimator::hp_push(particle_state * p)
{
    heap.push_back(p);
    hp_up(heap.size()-1);
}

void PHGenericParticleAnimator::clear()
{
    for (vector<particle_state*>::iterator i = heap.begin(); i!=heap.end(); i++)
        delete *i;
    heap.clear();
}

PHGenericParticleAnimator::PHGenericParticleAnimator() : mutex(new PHMutex), playing(true), generating(true), genFor(INFINITY), pps(5), genArea(PHRect(0,0,0,0)), elArea(false), vel(PHOriginPoint), deltavel(0), spreadAngl(0), grav(0,-9.81), deltaSize(1), initSize(0.1,0.1), endSize(0.05,0.05), lifetime(1), deltalifetime(0), initColor(PHWhiteColor), endColor(PHWhiteColor), rotates(false), rotoff(0), L(NULL), genQueue(0), cache(NULL), cached(false)
{}

void PHGenericParticleAnimator::animateParticle(PHGenericParticleAnimator::particle_state * p, ph_float elapsed)
{
    p->lifespan -= elapsed;
    if (p->lifespan<=0) return;
    p->part.position += p->velocity*elapsed;
    p->velocity += grav*elapsed;
    ph_float q = (p->lifespan)/(p->totalLife);
    p->part.size += (endSize-initSize)*(elapsed / p->totalLife); 
    p->part.color = PHColor(initColor.r*q+endColor.r*(1-q),
                                initColor.g*q+endColor.g*(1-q),
                                initColor.b*q+endColor.b*(1-q),
                                initColor.a*q+endColor.a*(1-q));
    if (rotates)
        p->part.rotation = PHAngleFromVector(p->velocity)-M_PI_2;
    else
        p->part.rotation = 0;
    p->part.rotation += rotoff;
}

void PHGenericParticleAnimator::setVelocity(const PHPoint &v)
{
    mutex->lock(); vel = v; mutex->unlock();
}

void PHGenericParticleAnimator::advanceAnimation(ph_float elapsedTime)
{
    mutex->lock();
    if (playing)
    {
        if (generating)
        {
            genQueue+=elapsedTime;
            genFor-=elapsedTime;
            if (genFor<=0)
            {
                genFor=0;
                generating = false;
            }
        }
        int n = heap.size();
        for (int i=0; i<n; i++)
            animateParticle(heap[i],elapsedTime);
        while (hp_top() && hp_top()->lifespan<=0)
            delete hp_pop();
        
        ph_float interval = 1.0f/pps;
        while (genQueue>=interval)
        {
            genQueue-=interval;
            particle_state * st = new particle_state;
            ph_float r1 = (ph_float)rand()/RAND_MAX;
            ph_float r2 = (ph_float)rand()/RAND_MAX;
            ph_float r3 = (ph_float)rand()/RAND_MAX;
            PHPoint p;
            if (elArea)
                p = PHPoint((cos(r2*M_PI*2)*r2+1)/2,(sin(r1*M_PI*2)*r2+1)/2);
            else
                p = PHPoint(r1,r2);
            p.x = genArea.x + p.x*genArea.width;
            p.y = genArea.y + p.y*genArea.height;
            st->part.position = p;
            st->part.size = initSize * (1 + (deltaSize - 1) * r3);
            st->part.color = initColor;
            ph_float ang = PHAngleFromVector(vel);
            ph_float module = vel.length()+((ph_float)rand()/RAND_MAX)*deltavel;
            ang += (((ph_float)rand()/RAND_MAX)-0.5)*spreadAngl;
            st->part.rotation = rotates?(ang-M_PI_2):0;
            st->velocity = PHPoint(cos(ang)*module,sin(ang)*module);
            st->lifespan = st->totalLife = lifetime+((ph_float)rand()/RAND_MAX)*deltalifetime;
            animateParticle(st,genQueue);
            if (st->lifespan>0)
                hp_push(st);
        }

        cached = false;
    }
    mutex->unlock();
}

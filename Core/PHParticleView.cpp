//
//  PHParticleView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/18/11.
//  Copyright (c) 2011 Porkholt Labs!. All rights reserved.
//

#include "PHParticleView.h"
#include "PHParticleAnimator.h"
#include "PHAnimatorPool.h"
#include "PHLua.h"
#include "PHNormalImage.h"
#include "PHImageAnimator.h"

#define INIT particleM(new PHMutex), particleAnim(NULL), particlePool(PHAnimatorPool::currentAnimatorPool())

PHParticleView::PHParticleView() : PHImageView(), INIT 
{
    init();
}
PHParticleView::PHParticleView(const PHRect &frame) : PHImageView(frame), INIT 
{
    init();
}

PHParticleView::PHParticleView(PHImage * image) : PHImageView(image), INIT
{
    init();
}

void PHParticleView::init()
{
    luaClass = "PHParticleView";
    setParticleAnimator(new PHParticleAnimator);
}

PHParticleView::~PHParticleView()
{
    setParticleAnimator(NULL);
    particleM->unlock();
}

void PHParticleView::setParticleAnimator(PHParticleAnimator * anim )
{
    particleM->lock();
    if (anim)
    {
        anim->retain();
        anim->setAnimatorPool(particlePool);
    }
    if (particleAnim)
        particleAnim->release();
    particleAnim = anim;
    particleM->unlock();
}

void PHParticleView::setParticleAnimatorPool(PHAnimatorPool * animPool )
{
    particleM->lock();
    particlePool = animPool;
    if (particleAnim)
        particleAnim->setAnimatorPool(particlePool);
    particleM->unlock();
}

void PHParticleView::loadFromLua(lua_State * L)
{
    PHImageView::loadFromLua(L);
    if (lua_istable(L,-1))
    {
        lua_getfield(L, -1, "particleAnimator");
        if (lua_istable(L, -1))
        {
            PHParticleAnimator * pa = PHParticleAnimator::fromLua(L);
            setParticleAnimator(pa);
        }
    }
}

static int PHParticleView_particleAnimator(lua_State * L)
{
    PHParticleView * pv = (PHParticleView*)PHLuaThisPointer(L);
    if (!pv->particleAnimator())
        lua_pushnil(L);
    else
    {
        lua_getglobal(L, "PHParticleAnimator");
        if (lua_istable(L, -1))
        {
            lua_getfield(L, -1, "new");
            lua_getglobal(L, "PHParticleAnimator");
            lua_newtable(L);
            lua_pushlightuserdata(L, pv->particleAnimator());
            lua_setfield(L, -1, "ud");
            PHLuaCall(L, 2, 1);
            lua_remove(L, -2);
        }
    }
    return 1;
}

void PHParticleView::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHParticleView");
    if (lua_istable(L, -1))
    {
        PHLuaAddMethod(PHParticleView, particleAnimator);
    }
    lua_pop(L, -1);
}

void PHParticleView::renderParticles(void * p, const PHRect & texCoord, const PHColor & tint)
{
    vector<PHParticleAnimator::particle> * particles = (vector<PHParticleAnimator::particle>*)p;
    if (!p) return;
}

void PHParticleView::render()
{
    particleM->lock();
    if (!particleAnim || !_image) 
    {
        particleM->unlock();
        return;
    }
    vector<PHParticleAnimator::particle> * particles = particleAnim->calculatedParticles();
    particleM->unlock();
    if (!particles) return;
    if (!particles->empty())
    {
        PHColor t = tint;
        if (!t.isValid())
            t = PHWhiteColor;
        _image->load();
        if (_image->isNormal())
        {
            ((PHNormalImage*)_image)->bindToTexture();
            renderParticles(particles,((PHNormalImage*)_image)->textureCoordinates(textureCoordinates()),t);
        }
        if (_image->isAnimated())
        {
            _animator->bindCurrentFrameToTexture();
            bool fd = _animator->isFading();
            double rem = fd?(_animator->remainingFrameTime()/_animator->currentFrameTime()):0;
            renderParticles(particles, _animator->currentFrameTextureCoordinates(textureCoordinates()), t*(1-rem));
            if (fd)
            {
                _animator->bindLastFrameToTexture();
                renderParticles(particles, _animator->lastFrameTextureCoordinates(textureCoordinates()), t*rem);
            }   
        }
    }
    delete particles;
}
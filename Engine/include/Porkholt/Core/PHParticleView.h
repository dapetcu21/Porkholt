/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHPARTICLEVIEW_H
#define PHPARTICLEVIEW_H

#include <Porkholt/Core/PHImageView.h>

class PHParticleAnimator;
class PHAnimatorPool;
class PHGLVertexArrayObject;
class PHGLVertexBufferObject;
class PHTextureAtlas;

class PHParticleView : public PHImageView
{
private:
    PHAnimatorPool * particlePool;
    PHParticleAnimator * particleAnim;
    PHMutex * particleM;
    
    void renderParticles(void * particles, const PHRect & texCoord, const PHColor & tint, PHTextureAtlas * atlas = NULL);
    
    PHGLVertexArrayObject * vao;
    
    bool useBytes;
    size_t maxN;
    
    ph_float cacheTime;
    ph_float cacheLeft;
    
    bool udindex;

    void attachedToGameManager();
    
public:
    PHParticleView();
    PHParticleView(const PHRect &frame);
    PHParticleView(PHImage * image);
    void init();
    
    void setIndexCacheTime(ph_float time) { cacheLeft = cacheTime = time; }
    ph_float indexCacheTime() { return cacheTime; }
    
    virtual ~PHParticleView();
    virtual void render();
    
    PHAnimatorPool * particleAnimatorPool() { return particlePool; }
    PHParticleAnimator * particleAnimator() { return particleAnim; }
    void setParticleAnimator(PHParticleAnimator * anim );
    void setParticleAnimatorPool(PHAnimatorPool * animPool );

    void setUseUDAsAtlasIndex(bool u) { udindex = u; }
    bool useUDAsAtlasIndex() { return udindex; }
    
    virtual void loadFromLua(lua_State * L);
    static void registerLuaInterface(lua_State * L);
    
    static const string _luaClass;
};


#endif

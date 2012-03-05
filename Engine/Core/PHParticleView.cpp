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
#include "PHGameManager.h"
#include "PHGLVertexArrayObject.h"
#include "PHGLVertexBufferObject.h"
#include "PHDeferredView.h"

PH_REGISTERIMAGEVIEW(PHParticleView)

#define INIT particleM(new PHMutex), particleAnim(NULL), particlePool(PHAnimatorPool::currentAnimatorPool()), vao(NULL), vbo(NULL), indexVBO(NULL), maxN(0), cacheTime(15), cacheLeft(15)

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
    PHParticleAnimator * panim = new PHParticleAnimator;
    setParticleAnimator(panim);
    panim->release();
}

PHParticleView::~PHParticleView()
{
    if (vao)
        vao->release();
    if (vbo)
        vbo->release();
    if (indexVBO)
        indexVBO->release();
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
        lua_pop(L, 1);
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
    if (!p || !particles->size()) return;
    size_t n = particles->size();
    size_t nrVertices = n*4;
    GLfloat * buffer = new GLfloat[nrVertices*(2+2+1)];
    GLfloat * b = buffer;
    GLfloat stdTxC[8] = 
    {
        texCoord.x, texCoord.y,
        texCoord.x+texCoord.width, texCoord.y,
        texCoord.x, texCoord.y+texCoord.height,
        texCoord.x+texCoord.width, texCoord.y+texCoord.height
    };
    for (size_t i = 0; i<n; i++)
    {
        PHParticleAnimator::particle & p = particles->at(i);
        const PHSize & sz = p.size;
        PHPoint pp[4];
        pp[0] = PHPoint(-sz.width/2,-sz.height/2);
        pp[1] = PHPoint(+sz.width/2,-sz.height/2);
        pp[2] = PHPoint(-sz.width/2,+sz.height/2);
        pp[3] = PHPoint(+sz.width/2,+sz.height/2);
        
        for (int j=0; j<4; j++)
        {
            pp[j].rotate(p.rotation);
            pp[j]+=p.position;
            
            b[0] = pp[j].x;
            b[1] = pp[j].y;
            
            b[2] = stdTxC[(j<<1)];
            b[3] = stdTxC[(j<<1)+1];
            
            PH24BitColor c = PH24BitColor(p.color);
            *((uint32_t*)(b+4)) = *((uint32_t*)&c);
            
            b+=5;
        }
    }
    
    if (!indexVBO)
        indexVBO = new PHGLVertexBufferObject(gm);
    if (!vbo)
        vbo = new PHGLVertexBufferObject(gm);
    
    vbo->bindTo(PHGLVBO::arrayBuffer);
    vbo->setData(NULL, sizeof(GLfloat)*nrVertices*5, gm->hasCapability(PHGLCapabilityGLES1)?PHGLVBO::dynamicDraw:PHGLVBO::streamDraw);
    vbo->setSubData(buffer, 0, sizeof(GLfloat)*nrVertices*5);
    
    cacheLeft -= (1.0f / gm->framesPerSecond());
    if (cacheLeft <=0)
    {
        cacheLeft = cacheTime;
        maxN = 0;
    }
    
    if (n>maxN)
    {
        maxN = n;
        
        size_t nrIndices = n?(n*6-2):0;
        useBytes = (nrVertices<=256);
        
        GLubyte * bIndices = useBytes?(new GLubyte[nrIndices]):NULL;
        GLushort * sIndices = useBytes?NULL:(new GLushort[nrIndices]);
        void * indexBuffer = useBytes?(void*)bIndices:(void*)sIndices;
        size_t indexSize = nrIndices*(useBytes?sizeof(GLubyte):sizeof(GLushort));
        
        if (useBytes)
        {
            if (n)
            {
                bIndices[0] = 0;
                bIndices[1] = 1;
                bIndices[2] = 2;
                bIndices[3] = 3;
            }
            for (size_t i=1; i<n; i++)
            {
                size_t i6 = i*6;
                size_t i4 = i*4;
                bIndices[i6-2] = i4-1;
                bIndices[i6-1] = i4;
                bIndices[i6] = i4;
                bIndices[i6+1] = i4+1;
                bIndices[i6+2] = i4+2;
                bIndices[i6+3] = i4+3;
            }
        } else {
            if (n)
            {
                sIndices[0] = 0;
                sIndices[1] = 1;
                sIndices[2] = 2;
                sIndices[3] = 3;
            }
            for (size_t i=1; i<n; i++)
            {
                size_t i6 = i*6;
                size_t i4 = i*4;
                sIndices[i6-2] = i4-1;
                sIndices[i6-1] = i4;
                sIndices[i6] = i4;
                sIndices[i6+1] = i4+1;
                sIndices[i6+2] = i4+2;
                sIndices[i6+3] = i4+3;
            }
        }
        
        indexVBO->bindTo(PHGLVBO::elementArrayBuffer);
        indexVBO->setData(NULL, indexSize, PHGLVBO::dynamicDraw);
        indexVBO->setSubData(indexBuffer, 0, indexSize);
        indexVBO->unbind();

        if (useBytes)
            delete [] bIndices;
        else
            delete [] sIndices;
    }
    
    if (!vao)
    {
        vao = new PHGLVertexArrayObject(gm);
        vao->bindToEdit();
        vao->vertexPointer(PHIMAGEATTRIBUTE_POS, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, 0, vbo);
        vao->vertexPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, sizeof(GLfloat)*2, vbo);
        vao->vertexPointer(PHIMAGEATTRIBUTE_CLR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GLfloat)*5, sizeof(GLfloat)*4, vbo);
        indexVBO->bindTo(PHGLVBO::elementArrayBuffer);
        vao->unbind();
    }
    vbo->unbind();
    delete [] buffer;   
    
    gm->setGLStates(PHGLTexture);
    gm->applyShader(gm->coloredSpriteShader());
    
    vao->bind();
    glDrawElements(GL_TRIANGLE_STRIP, n?(n*6-2):0, useBytes?GL_UNSIGNED_BYTE:GL_UNSIGNED_SHORT, NULL);
    vao->unbind();
}

void PHParticleView::render()
{
    particleM->lock();
    if (!particleAnim || !_image) 
    {
        particleM->unlock();
        return;
    }
    PHImage * img = _image;
    switch (gm->renderMode())
    {
        case PHGameManager::defaultRenderMode:
            break;
        case PHDeferredView::normalMapRenderMode:
            img = _image->normalMap();
            if (!img)
                return;
            break;
        default:
            return;
    }
    vector<PHParticleAnimator::particle> * particles = particleAnim->calculatedParticles();
    particleM->unlock();
    if (!particles) return;
    if (!particles->empty())
    {
        PHMatrix om = gm->modelViewMatrix();
        gm->setModelViewMatrix(om * applyMatrices());
        
        PHColor t = tint;
        if (!t.isValid())
            t = PHWhiteColor;
        img->load();
        if (img->isNormal())
        {
            ((PHNormalImage*)img)->bindToTexture(0);
            renderParticles(particles,((PHNormalImage*)img)->textureCoordinates(textureCoordinates()),t);
        }
        if (img->isAnimated())
        {
            _animator->bindCurrentFrameToTexture(0);
            bool fd = _animator->isFading();
            ph_float rem = fd?(_animator->remainingFrameTime()/_animator->currentFrameTime()):0;
            renderParticles(particles, _animator->currentFrameTextureCoordinates(textureCoordinates()), t*(1-rem));
            if (fd)
            {
                _animator->bindLastFrameToTexture(0);
                renderParticles(particles, _animator->lastFrameTextureCoordinates(textureCoordinates()), t*rem);
            }   
        }
        
        gm->setModelViewMatrix(om);
    }
    delete particles;
}
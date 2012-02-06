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

PH_REGISTERIMAGEVIEW(PHParticleView)

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
    if (!p || !particles->size()) return;
    int n = (int)(particles->size());
    int nrVertices = (int)(particles->size()*6-2);
    GLfloat * vertices = new GLfloat[nrVertices*2];
    GLfloat * textureCoords = new GLfloat[nrVertices*2];
    GLfloat * colors = new GLfloat[nrVertices*4];
    GLfloat * v = vertices;
    GLfloat * txC = textureCoords;
    GLfloat * clr = colors;
    GLfloat stdTxC[8] = 
    {
        texCoord.x, texCoord.y,
        texCoord.x+texCoord.width, texCoord.y,
        texCoord.x, texCoord.y+texCoord.height,
        texCoord.x+texCoord.width, texCoord.y+texCoord.height
    };
    for (int i = 0; i<n; i++)
    {
        PHParticleAnimator::particle & p = particles->at(i);
        const PHSize & sz = p.size;
        PHPoint pp[4];
        pp[0] = PHPoint(-sz.width/2,-sz.height/2);
        pp[1] = PHPoint(+sz.width/2,-sz.height/2);
        pp[2] = PHPoint(-sz.width/2,+sz.height/2);
        pp[3] = PHPoint(+sz.width/2,+sz.height/2);
        if (i)
        {
            v[0] = v[-2];
            v[1] = v[-1];
            pp[0].rotate(p.rotation);
            pp[0]+=p.position;
            v[2] = pp[0].x; v[3] = pp[0].y;
            txC[0] = stdTxC[6];
            txC[1] = stdTxC[7];
            txC[2] = stdTxC[0];
            txC[3] = stdTxC[1];
            clr[0] = clr[-4];
            clr[1] = clr[-3];
            clr[2] = clr[-2];
            clr[3] = clr[-1];
            clr[4] = p.color.r;
            clr[5] = p.color.g;
            clr[6] = p.color.b;
            clr[7] = p.color.a;
            v+=4;
            txC+=4;
            clr+=8;
        }
        for (int j=0; j<4; j++)
        {
            if ((i&&j) || !i)
            {
                pp[j].rotate(p.rotation);
                pp[j]+=p.position;
            }
            v[(j<<1)] = pp[j].x; v[(j<<1)+1] = pp[j].y;
            clr[(j<<2)] = p.color.r;
            clr[(j<<2)+1] = p.color.g;
            clr[(j<<2)+2] = p.color.b;
            clr[(j<<2)+3] = p.color.a;
        }
        memcpy(txC, stdTxC, sizeof(GLfloat)*8);
        v+=8;
        txC+=8;
        clr+=16;
    }
    PHGLSetStates(PHGLTexture | PHGLVertexArray | PHGLTextureCoordArray | PHGLColorArray);
    _gameManager->pushSpriteShader(_gameManager->coloredSpriteShader());
    _gameManager->applySpriteShader();
    _gameManager->popSpriteShader();
    PHGLVertexPointer(2, GL_FLOAT, 0, vertices);
    PHGLTexCoordPointer(2, GL_FLOAT, 0, textureCoords);
    PHGLColorPointer(4, GL_FLOAT, 0, colors);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, nrVertices);
    
    delete [] vertices;
    delete [] colors;
    delete [] textureCoords;
}

void PHParticleView::render()
{
    particleM->lock();
    if (!particleAnim || !_image) 
    {
        particleM->unlock();
        return;
    }
    PHMatrix om = PHGLModelView();
    PHGLSetModelView(om * applyMatrices());
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
            ((PHNormalImage*)_image)->bindToTexture(0);
            renderParticles(particles,((PHNormalImage*)_image)->textureCoordinates(textureCoordinates()),t);
        }
        if (_image->isAnimated())
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
    }
    delete particles;
    PHGLSetModelView(om);
}
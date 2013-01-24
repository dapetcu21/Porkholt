/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHParticleView.h>
#include <Porkholt/Core/PHGenericParticleAnimator.h>
#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/Core/PHImageAnimator.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLVertexBufferObject.h>
#include <Porkholt/Core/PHDeferredView.h>

PH_REGISTERIMAGEVIEW(PHParticleView)

#define INIT particlePool(NULL), particleAnim(NULL), particleM(new PHMutex), vao(NULL), maxN(0), cacheTime(15), cacheLeft(15)

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

const string PHParticleView::_luaClass("PHParticleView");

void PHParticleView::init()
{
    luaClass = &_luaClass;
}

PHParticleView::~PHParticleView()
{
    if (vao)
        vao->release();
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
            PHParticleAnimator * pa = PHGenericParticleAnimator::fromLua(L);
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

char color_sprites[] = "color_sprites";

void PHParticleView::renderParticles(void * p, const PHRect & texCoord, const PHColor & tint)
{
    PHParticleAnimator::particles * particles = (PHParticleAnimator::particles*)p;
    if (!p || !particles->n) return;
    size_t n = particles->n;
    size_t nrVertices = n*4;
    GLfloat * buffer = new GLfloat[nrVertices*(3+2+1)];
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
        PHParticleAnimator::particle2D & p = particles->v2d[i];
        const PHSize & sz = p.size;
        PH3DPoint pp[4];
        pp[0] = PH3DPoint(-sz.width/2,-sz.height/2, 0);
        pp[1] = PH3DPoint(+sz.width/2,-sz.height/2, 0);
        pp[2] = PH3DPoint(-sz.width/2,+sz.height/2, 0);
        pp[3] = PH3DPoint(+sz.width/2,+sz.height/2, 0);
        PH24BitColor c = PH24BitColor(p.color);

        for (int j=0; j<4; j++)
        {
            pp[j].rotate(p.rotation);
            pp[j]+=p.position;
            
            b[0] = pp[j].x;
            b[1] = pp[j].y;
            b[2] = pp[j].z;
            
            b[3] = stdTxC[(j<<1)];
            b[4] = stdTxC[(j<<1)+1];
            
            *((uint32_t*)(b+5)) = *((uint32_t*)&c);
            
            b+=6;
        }
    }
    
    PHGLVBO * vbo = vao ? vao->attributeVBO(PHIMAGEATTRIBUTE_POS) : NULL;
    PHGLVBO * indexVBO = vao ? vao->elementArrayVBO() : NULL;

    if (!indexVBO)
        indexVBO = new PHGLVertexBufferObject(gm);
    else
        indexVBO->retain();

    if (!vbo)
        vbo = new PHGLVertexBufferObject(gm);
    else
        vbo->retain();

    vbo->setDataOptimally(buffer, sizeof(GLfloat)*nrVertices*6, PHGLVBO::streamDraw);
    
    cacheLeft -= gm->frameInterval();
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
            bIndices[0] = 0;
            bIndices[1] = 1;
            bIndices[2] = 2;
            bIndices[3] = 3;
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
            sIndices[0] = 0;
            sIndices[1] = 1;
            sIndices[2] = 2;
            sIndices[3] = 3;
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
        
        indexVBO->setDataOptimally(indexBuffer, indexSize, PHGLVBO::dynamicDraw);

        if (useBytes)
            delete [] bIndices;
        else
            delete [] sIndices;
    }
    
    if (!vao)
    {
        vao = new PHGLVertexArrayObject(gm);
        vao->bindToEdit();
        vao->vertexPointer(PHIMAGEATTRIBUTE_POS, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, 0, vbo);
        vao->vertexPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, sizeof(GLfloat)*3, vbo);
        vao->vertexPointer(PHIMAGEATTRIBUTE_CLR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GLfloat)*6, sizeof(GLfloat)*5, vbo);
        indexVBO->bindTo(PHGLVBO::elementArrayBuffer);
        vao->unbind();
    }
    vbo->unbind();
    vbo->release();
    indexVBO->release();
    delete [] buffer;   

    vao->setDrawElements(GL_TRIANGLE_STRIP, n?(n*6-2):0, useBytes ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT, 0);
    gm->pushSpriteShader(gm->shaderProgramNamed<color_sprites>());
    _material->renderVAO(vao, gm->spriteUniformStates());
    gm->popSpriteShader();
    
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
    int rm = gm->renderMode();
    if (!supportsRenderMode(rm))
    {
        particleM->unlock();
        return;
    }
    switch (rm)
    {
        case PHGameManager::defaultRenderMode:
            break;
        case PHDeferredView::normalMapRenderMode:
            img = _image->normalMap();
            break;
        default:
            break;
    }
    PHParticleAnimator::particles * particles = particleAnim->calculatedParticles();
    particleM->unlock();
    if (!particles) return;
    if (particles->n)
    {
        PHMatrix om = gm->modelViewMatrix();
        gm->setModelViewMatrix(om * applyMatrices());
        gm->updateMatrixUniform();
        
        PHColor t = tint;
        if (!t.isValid())
            t = PHWhiteColor;
        img->load();
        if (img->isNormal())
        {
            gm->setTextureUniform(((PHNormalImage*)img)->texture());
            renderParticles(particles,((PHNormalImage*)img)->textureCoordinates(textureCoordinates()),t);
        }
        if (img->isAnimated())
        {
            bool fd = _animator->isFading();
            ph_float rem = fd?(_animator->remainingFrameTime()/_animator->currentFrameTime()):0;
            if (fd)
            {
                gm->setTextureUniform(_animator->lastFrameTexture());
                renderParticles(particles, _animator->lastFrameTextureCoordinates(textureCoordinates()), t.multipliedAlpha(rem));
            }   
            gm->setTextureUniform(_animator->currentFrameTexture());
            renderParticles(particles, _animator->currentFrameTextureCoordinates(textureCoordinates()), t.multipliedAlpha(1-rem));
        }
        
        gm->setTextureUniform(NULL);
        gm->setModelViewMatrix(om);
    }
}

void PHParticleView::attachedToGameManager()
{
    PHImageView::attachedToGameManager();
    if (!particlePool)
        setParticleAnimatorPool(gm->animatorPool());
}

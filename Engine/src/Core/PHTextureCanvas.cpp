/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHTextureCanvas.h>
#include <Porkholt/Core/PHGLFramebuffer.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHGLTexture.h>
#include <Porkholt/Core/PHGLRenderbuffer.h>
#include <Porkholt/Core/PHAutoreleasePool.h>

PHTextureCanvas::PHTextureCanvas(PHGameManager * _gm) : fbo(NULL), changed(true), dit(false),  dfmt(PHGLFBOAttachment::None), dtex(NULL), ow(-1), oh(-1), w(0), h(0), fs(true), cmask(PHGameManager::colorBuffers | PHGameManager::stencilBuffer | PHGameManager::depthBuffer), ccolor(PHClearColor), cdepth(1.0f), cstencil(0), _renderMode(PHGameManager::defaultRenderMode)
{
    setGameManager(_gm);
}

void PHTextureCanvas::setColorFormat(enum PHGLFBOAttachment::pixelFormat fmt, int att)
{
    if (att <= cfmt.size())
        cfmt.resize(att+1, PHGLFBOAttachment::None);
    cfmt[att] = fmt;
    changed = true;
}

void PHTextureCanvas::setDepthFormat(enum PHGLFBOAttachment::pixelFormat fmt)
{
    dfmt = fmt;
    changed = true;
}

void PHTextureCanvas::setDepthIsTexture(bool v)
{
    dit = v;
    if (!v && dtex)
    {
        dtex->release();
        dtex = NULL;
    }
    changed = true;
}

void PHTextureCanvas::setColorTexture(PHGLTexture2D * tex, int t)
{
    if (t >= ctex.size())
        ctex.resize(t+1);
    if (tex)
        tex->retain();
    if (ctex[t])
        ctex[t]->release();
    ctex[t] = tex;
    changed = true;
}

void PHTextureCanvas::setDepthTexture(PHGLTexture2D * tex)
{
    if (tex && !dit)
        setDepthIsTexture(true);
    if (tex)
        tex->retain();
    if (dtex)
        dtex->release();
    dtex = tex;
    changed = true;
}

PHGLTexture2D * PHTextureCanvas::depthTexture()
{
    if (!dit)
        return NULL;
    if (!dtex)
    {
        dtex = new PHGLTexture2D(gm);
        dtex->setWrapS(PHGLTexture::clampToEdge);
        dtex->setWrapT(PHGLTexture::clampToEdge);
        dtex->setMinFilter(PHGLTexture::linear);
        dtex->setMagFilter(PHGLTexture::linear);
    }
    return dtex;
}

PHGLTexture2D * PHTextureCanvas::colorTexture(int t)
{
    if (t >= ctex.size())
        ctex.resize(t+1);
    if (!ctex[t])
    {
        PHGLTexture2D * tex;
        ctex[t] = tex = new PHGLTexture2D(gm);
        tex->setWrapS(PHGLTexture::clampToEdge);
        tex->setWrapT(PHGLTexture::clampToEdge);
        tex->setMinFilter(PHGLTexture::linear);
        tex->setMagFilter(PHGLTexture::linear);
        return tex;
    }
    return ctex[t];
}

PHTextureCanvas::~PHTextureCanvas()
{
    if (fbo)
        fbo->release();
    if (dtex)
        dtex->release();
    for (vector<PHGLTexture2D*>::iterator i =ctex.begin(); i!= ctex.end(); i++)
        if (*i)
            (*i)->release();
}

PHGLFBOAttachment * PHTextureCanvas::rebuildAttachment(PHGLFBOAttachment * f, enum PHGLFBOAttachment::pixelFormat fmt, bool isTex, int w, int h, PHGLTexture2D ** cache)
{
    if (fmt == PHGLFBOAttachment::None)
    {
        if (*cache)
        {
            (*cache)->release();
            *cache = NULL;
        }
        return NULL;
    }
    if (isTex)
    {
        PHGLTexture2D * t = dynamic_cast<PHGLTexture2D*>(*cache);
        if (!t)
        {
            *cache = t = new PHGLTexture2D(gm);
            t->setWrapS(PHGLTexture::clampToEdge);
            t->setWrapT(PHGLTexture::clampToEdge);
            t->setMinFilter(PHGLTexture::linear);
            t->setMagFilter(PHGLTexture::linear);
        }
        if (t->format() != fmt || t->width() != w || t->height() != h)
            t->setData(NULL, w, h, fmt);
        return t;
    } else {
        PHGLRenderbuffer * r = dynamic_cast<PHGLRenderbuffer*>(f);
        if (!r || r->format() != fmt || r->width() != w || r->height() != h)
        {
            r = new PHGLRenderbuffer(gm, w, h, fmt);
            r->autorelease();
        } 
        if (*cache)
        {
            (*cache)->release();
            *cache = NULL;
        }
        return r;
    }
}

void PHTextureCanvas::render()
{
    int rm = gm->renderMode();
    if (!supportsRenderMode(rm)) return;
    cb.call(this);

    PHGLFramebuffer * f = gm->boundFramebuffer();
    
    int sw = w; int sh = h;
    if (fs)
    {
        sw = gm->screenWidth();
        sh = gm->screenHeight();
    }
    if ((sw!=ow) || (sh!=oh))
    {
        changed = true;
        ow = sw;
        oh = sh;
    }

    if (changed)
    {
        changed = false;
        PHAutoreleasePool pool;
        if (!fbo)
            fbo = new PHGLFramebuffer(gm);
        fbo->attachDepth(rebuildAttachment(fbo->depthAttachment(), dfmt, dit, sw, sh, &dtex));
        int n = min<size_t>(cfmt.size(), gm->colorAttachmentCount());
        if (ctex.size() < n)
            ctex.resize(n);
        for (int i=0; i<n; i++)
            fbo->attachColor(rebuildAttachment(fbo->colorAttachment(i), cfmt[i], true, sw, sh, &ctex[i]), i);
    }

    int mask = (dfmt != PHGLFBOAttachment::None) ? PHGameManager::depthBuffer : 0;
    if ((dfmt == PHGLFBOAttachment::Depth24Stencil8) ||
        (dfmt == PHGLFBOAttachment::Depth32fStencil8)) 
        mask |= PHGameManager::stencilBuffer;
    if (cfmt.size() && cfmt[0]!=PHGLFBOAttachment::None)
        mask |= PHGameManager::colorBuffers;
    mask &= cmask;

    if (mask & PHGameManager::colorBuffers)
        gm->setClearColor(ccolor);
    if (mask & PHGameManager::depthBuffer)
        gm->setDepthClearValue(cdepth);
    if (mask & PHGameManager::stencilBuffer)
        gm->setStencilClearValue(cstencil);

    gm->bindFramebuffer(fbo);
    gm->clearBuffers(mask & cmask);
    gm->setRenderMode(_renderMode);
    renderChildren();
    gm->setRenderMode(rm);
    gm->bindFramebuffer(f);
}

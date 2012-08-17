/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLFramebuffer.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHGLTexture.h>
#include <Porkholt/Core/PHGLRenderbuffer.h>

PHGLFramebuffer::PHGLFramebuffer(PHGameManager * gam) : gm(gam), dth(NULL), stc(NULL)
{
    gm->glGenFramebuffers(1, &id);
    nclr = gm->colorAttachmentCount();
    clr = new PHGLFBOAttachment * [nclr];
    memset(clr, 0, sizeof(PHGLFBOAttachment*)*nclr);
}

PHGLFramebuffer::~PHGLFramebuffer()
{
    gm->glDeleteFramebuffers(1, &id);
    for (int i = 0; i<nclr; i++)
        if (clr[i])
            clr[i]->release();
    if (stc)
        stc->release();
    if (dth)
        dth->release();
    delete[] clr;
}

void PHGLFramebuffer::attachColor(PHGLFBOAttachment * a, int index)
{
    if (a == clr[index]) return;
    if (a)
        a->retain();
    attachToTarget(a, GL_COLOR_ATTACHMENT0 + index);
    if (clr[index])
        clr[index]->release();
    clr[index] = a;
}

#ifndef GL_DEPTH_STENCIL_ATTACHMENT
#ifdef GL_DEPTH_STENCIL_ATTACHMENT_OES
#define GL_DEPTH_STENCIL_ATTACHMENT GL_DEPTH_STENCIL_ATTACHMENT_OES
#else
#define GL_DEPTH_STENCIL_ATTACHMENT GL_DEPTH_ATTACHMENT
#endif
#endif

void PHGLFramebuffer::attachDepth(PHGLFBOAttachment * a)
{
    if (a == dth) return;
    if (a)
        a->retain();
    enum PHGLFBOAttachment::pixelFormat f = a->format();
    if (f == PHGLFBOAttachment::Depth24Stencil8 ||
        f == PHGLFBOAttachment::Depth32fStencil8)
        attachToTarget(a, GL_DEPTH_STENCIL_ATTACHMENT);
    else
        attachToTarget(a, GL_DEPTH_ATTACHMENT);
    if (dth)
        dth->release();
    dth = a;
}

void PHGLFramebuffer::attachStencil(PHGLFBOAttachment * a)
{
    if (a == stc) return;
    if (a)
        a->retain();
    attachToTarget(a, GL_STENCIL_ATTACHMENT);
    if (stc)
        stc->release();
    stc = a;
}

void PHGLFramebuffer::attachToTarget(PHGLFBOAttachment * a, GLenum t)
{
    PHGLFramebuffer * f = gm->boundFramebuffer();
    gm->bindFramebuffer(this);
    if (a)
    {
        PHGLTexture2D * tex = dynamic_cast<PHGLTexture2D*>(a);
        if (tex)
            gm->glFramebufferTexture2D(GL_FRAMEBUFFER, t, tex->target, tex->tex, 0);
        else
        {
            PHGLRenderbuffer * rb = dynamic_cast<PHGLRenderbuffer*>(a);
            if (rb)
               gm->glFramebufferRenderbuffer(GL_FRAMEBUFFER, t, GL_RENDERBUFFER, rb->id);
        }
    } else 
        gm->glFramebufferRenderbuffer(GL_FRAMEBUFFER, t, GL_RENDERBUFFER, 0);
    gm->bindFramebuffer(f);
}

void PHGLFramebuffer::bind()
{
    gm->bindFramebuffer(this);
}

void PHGLFramebuffer::unbind()
{
    gm->bindFramebuffer(NULL);
}

bool PHGLFramebuffer::isComplete()
{
    PHGLFramebuffer * f = gm->boundFramebuffer();
    gm->bindFramebuffer(this);
    bool b = (gm->glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    gm->bindFramebuffer(f);
    return b;
}

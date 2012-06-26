/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLRenderBuffer.h>
#include <Porkholt/Core/PHGameManager.h>

PHGLRenderBuffer::PHGLRenderBuffer(PHGameManager * gm, int width, int height, enum pixelFormat fmt)
{
    gm->glGenRenderbuffers(1, &id);
    gm->glBindRenderbuffer(GL_RENDERBUFFER, id);
    gm->glRenderbufferStorage(GL_RENDERBUFFER, PHGLInternalFormats[fmt], width, height);
}

PHGLRenderBuffer::PHGLRenderBuffer(PHGameManager * gm, int width, int height, enum pixelFormat fmt, int samples)
{
    gm->glGenRenderbuffers(1, &id);
    gm->glBindRenderbuffer(GL_RENDERBUFFER, id);
    if (gm->glRenderbufferStorageMultisample)
        gm->glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, PHGLInternalFormats[fmt], width, height);
    else
        gm->glRenderbufferStorage(GL_RENDERBUFFER, PHGLInternalFormats[fmt], width, height);
}

PHGLRenderBuffer::~PHGLRenderBuffer()
{
    glDeleteRenderbuffers(1, &id);
}


/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLRenderbuffer.h>
#include <Porkholt/Core/PHGameManager.h>

#define INIT w(width), h(height)

PHGLRenderbuffer::PHGLRenderbuffer(PHGameManager * gm, int width, int height, enum pixelFormat fmt) : INIT
{
    pfmt = fmt;
    gm->glGenRenderbuffers(1, &id);
    gm->glBindRenderbuffer(GL_RENDERBUFFER, id);
    gm->glRenderbufferStorage(GL_RENDERBUFFER, PHGLInternalFormats[fmt], width, height);
}

PHGLRenderbuffer::PHGLRenderbuffer(PHGameManager * gm, int width, int height, enum pixelFormat fmt, int samples) : INIT
{
    pfmt = fmt;
    gm->glGenRenderbuffers(1, &id);
    gm->glBindRenderbuffer(GL_RENDERBUFFER, id);
    if (gm->glRenderbufferStorageMultisample)
        gm->glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, PHGLInternalFormats[fmt], width, height);
    else
        gm->glRenderbufferStorage(GL_RENDERBUFFER, PHGLInternalFormats[fmt], width, height);
}

PHGLRenderbuffer::~PHGLRenderbuffer()
{
    glDeleteRenderbuffers(1, &id);
}


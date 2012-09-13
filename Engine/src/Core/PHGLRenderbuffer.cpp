/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLRenderbuffer.h>
#include <Porkholt/Core/PHGameManager.h>

#define INIT w(width), h(height)

PHGLRenderbuffer::PHGLRenderbuffer(PHGameManager * gm, int width, int height, enum pixelFormat fmt) : INIT
{
    pfmt = fmt;
    PHGL::glGenRenderbuffers(1, &id);
    PHGL::glBindRenderbuffer(GL_RENDERBUFFER, id);
    PHGL::glRenderbufferStorage(GL_RENDERBUFFER, PHGLInternalFormats[fmt], width, height);
}

PHGLRenderbuffer::PHGLRenderbuffer(PHGameManager * gm, int width, int height, enum pixelFormat fmt, int samples) : INIT
{
    pfmt = fmt;
    PHGL::glGenRenderbuffers(1, &id);
    PHGL::glBindRenderbuffer(GL_RENDERBUFFER, id);
    if (PHGL::glRenderbufferStorageMultisample)
        PHGL::glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, PHGLInternalFormats[fmt], width, height);
    else
        PHGL::glRenderbufferStorage(GL_RENDERBUFFER, PHGLInternalFormats[fmt], width, height);
}

PHGLRenderbuffer::~PHGLRenderbuffer()
{
    PHGL::glDeleteRenderbuffers(1, &id);
}


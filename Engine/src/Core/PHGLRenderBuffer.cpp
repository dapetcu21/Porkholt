/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLRenderBuffer.h>

PHGLRenderBuffer::PHGLRenderBuffer(PHGameManager * gm, int width, int height, enum pixelFormat fmt)
{
    glGenRenderbuffers(1, &id);
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    glRenderbufferStorage(GL_RENDERBUFFER, PHGLInternalFormats[fmt], width, height);
}

PHGLRenderBuffer::PHGLRenderBuffer(PHGameManager * gm, int width, int height, enum pixelFormat fmt, int samples)
{
    glGenRenderbuffers(1, &id);
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, PHGLInternalFormats[fmt], width, height);
}

PHGLRenderBuffer::~PHGLRenderBuffer()
{
    glDeleteRenderbuffers(1, &id);
}


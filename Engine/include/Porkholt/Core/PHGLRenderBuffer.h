/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGLRENDERBUFFER_H
#define PHGLRENDERBUFFER_H

#include <Porkholt/Core/PHGLFBOAttachment.h>

class PHGameManager;

class PHGLRenderBuffer : public PHGLFBOAttachment
{
protected:
    GLuint id;
    friend class PHGLFramebuffer;
public:
    PHGLRenderBuffer(PHGameManager * gm, int width, int height, enum pixelFormat fmt);
    PHGLRenderBuffer(PHGameManager * gm, int width, int height, enum pixelFormat fmt, int samples);
    ~PHGLRenderBuffer();
};

#endif

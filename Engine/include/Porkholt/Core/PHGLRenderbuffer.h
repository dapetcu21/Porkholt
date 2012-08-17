/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGLRENDERBUFFER_H
#define PHGLRENDERBUFFER_H

#include <Porkholt/Core/PHGLFBOAttachment.h>

class PHGameManager;

class PHGLRenderbuffer : public PHGLFBOAttachment
{
protected:
    GLuint id;
    int w, h;

    friend class PHGLFramebuffer;
public:
    PHGLRenderbuffer(PHGameManager * gm, int width, int height, enum pixelFormat fmt);
    PHGLRenderbuffer(PHGameManager * gm, int width, int height, enum pixelFormat fmt, int samples);
    ~PHGLRenderbuffer();
    
    int width() { return w; }
    int height() { return h; }
};

#endif

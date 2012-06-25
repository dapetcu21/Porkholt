/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGLFRAMEBUFFER_H
#define PHGLFRAMEBUFFER_H

#include <Porkholt/Core/PHMain.h>

class PHGameManager;
class PHGLFBOAttachment;

class PHGLFramebuffer : public PHObject
{
protected:
    PHGameManager * gm;
    GLuint id;
    int nclr;
    PHGLFBOAttachment ** clr;
    PHGLFBOAttachment * dth;
    PHGLFBOAttachment * stc;

    friend class PHGameManager;

    void attachToTarget(PHGLFBOAttachment * a, GLenum target);
public:
    PHGLFramebuffer(PHGameManager * gameManager);
    ~PHGLFramebuffer();

    PHGLFBOAttachment * colorAttachment(int index) { return clr[index]; }
    PHGLFBOAttachment * depthAttachment() { return dth; }
    PHGLFBOAttachment * stencilAttachment() { return stc; }
    
    void attachColor(PHGLFBOAttachment * a, int index);
    void attachDepth(PHGLFBOAttachment * a);
    void attachStencil(PHGLFBOAttachment * a);

    void bind();
    bool isComplete();
};

#endif 

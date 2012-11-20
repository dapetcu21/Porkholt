/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGLFBOATTACHMENT_H
#define PHGLFBOATTACHMENT_H

#include <Porkholt/Core/PHMain.h>

class PHGLFBOAttachment : public PHObject
{ 
public:
    enum pixelFormat
    {
        None = 0,
        Depth16,
        Depth24,
        Depth32,
        Depth32f,
        Depth24Stencil8,
        Depth32fStencil8,
        A8,
        A16,
        L8,
        L16,
        R8,
        R16,
        RG8,
        RG16,
        LA8,
        LA16,
        RGBA8,
        RGBA16,
        RGB8,
        RGB16,
        R16f,
        RG16f,
        RGB16f,
        RGBA16f,
        R32f,
        RG32f,
        RGB32f,
        RGBA32f,
        InvalidFormat
    };

protected:
    enum pixelFormat pfmt;

public:
    enum pixelFormat format() { return pfmt; }
};

extern GLenum PHGLInternalFormats[];
extern GLenum PHGLFormats[];
extern GLenum PHGLTypes[];


#endif

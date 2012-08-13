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
        L8,
        LA8,
        RGBA8,
        RGBA16,
        RGB8,
        RGB16,
        InvalidFormat
    };

protected:
    enum pixelFormat pfmt;

public:
    enum pixelFormat format() { return pfmt; }
};

extern const GLenum PHGLInternalFormats[];
extern const GLenum PHGLFormats[];
extern const GLenum PHGLTypes[];


#endif

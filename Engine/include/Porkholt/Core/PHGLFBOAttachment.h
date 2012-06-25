/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGLFBOATTACHMENT_H
#define PHGLFBOATTACHMENT_H

#include <Porkholt/Core/PHMain.h>

class PHGLFBOAttachment : public PHObject
{ 
public:
    enum pixelFormat
    {
        A8,
        L8,
        LA8,
        RGBA8,
        RGBA16,
        RGB8,
        RGB16,
        InvalidFormat
    };
};

extern const GLenum PHGLInternalFormats[];
extern const GLenum PHGLFormats[];
extern const GLenum PHGLTypes[];


#endif

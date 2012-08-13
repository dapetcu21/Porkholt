/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLFBOAttachment.h>

const GLenum PHGLInternalFormats[] = {
    0,
    GL_DEPTH_COMPONENT16,
    GL_DEPTH_COMPONENT24,
    GL_DEPTH_COMPONENT32,
    GL_DEPTH_COMPONENT32F,
    GL_DEPTH24_STENCIL8,
    GL_DEPTH32F_STENCIL8,
#ifdef GL_ALPHA8 
    GL_ALPHA8,
#else
    GL_ALPHA,
#endif
#ifdef GL_LUMINANCE8
    GL_LUMINANCE8, 
#else
    GL_LUMINANCE,
#endif
    GL_LUMINANCE_ALPHA,
#ifdef GL_RGBA8
    GL_RGBA8, 
#else
    GL_RGBA,
#endif
#ifdef GL_RGBA16
    GL_RGBA16,
#else
    GL_RGBA,
#endif
#ifdef GL_RGB8
    GL_RGB8, 
#else
    GL_RGB,
#endif
#ifdef GL_RGB16
    GL_RGB16
#else
    GL_RGB
#endif
};

const GLenum PHGLFormats[]  = {
    0, 
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_STENCIL,
    GL_DEPTH_STENCIL,
    GL_ALPHA, 
    GL_LUMINANCE, 
    GL_LUMINANCE_ALPHA, 
    GL_RGBA, 
    GL_RGBA, 
    GL_RGB, 
    GL_RGB
};

const GLenum PHGLTypes[] = {
    0, 
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_INT,
    GL_UNSIGNED_INT,
    GL_FLOAT,
    GL_UNSIGNED_INT,
    GL_FLOAT,
    GL_UNSIGNED_BYTE, 
    GL_UNSIGNED_BYTE, 
    GL_UNSIGNED_BYTE, 
    GL_UNSIGNED_BYTE, 
    GL_UNSIGNED_SHORT, 
    GL_UNSIGNED_BYTE, 
    GL_UNSIGNED_SHORT
};

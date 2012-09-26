/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLFBOAttachment.h>

GLenum PHGLInternalFormats[] = {
    0,
#ifdef GL_DEPTH_COMPONENT16
    GL_DEPTH_COMPONENT16,
#elif defined(GL_DEPTH_COMPONENT16_OES)
    GL_DEPTH_COMPONENT16_OES,
#else
    GL_DEPTH_COMPONENT,
#endif
#ifdef GL_DEPTH_COMPONENT24
    GL_DEPTH_COMPONENT24,
#elif defined(GL_DEPTH_COMPONENT24_OES)
    GL_DEPTH_COMPONENT24_OES,
#else
    GL_DEPTH_COMPONENT,
#endif
#ifdef GL_DEPTH_COMPONENT32
    GL_DEPTH_COMPONENT32,
#elif defined(GL_DEPTH_COMPONENT32_OES)
    GL_DEPTH_COMPONENT32_OES,
#else
    GL_DEPTH_COMPONENT,
#endif
#ifdef GL_DEPTH_COMPONENT32F
    GL_DEPTH_COMPONENT32F,
#elif defined(GL_DEPTH_COMPONENT32F_OES)
    GL_DEPTH_COMPONENT32F_OES,
#else
    GL_DEPTH_COMPONENT,
#endif
#ifdef GL_DEPTH24_STENCIL8
    GL_DEPTH24_STENCIL8,
#elif defined(GL_DEPTH24_STENCIL8_OES)
    GL_DEPTH24_STENCIL8_OES,
#else
    0,
#endif
#ifdef GL_DEPTH32F_STENCIL8
    GL_DEPTH32F_STENCIL8,
#elif defined(GL_DEPTH32F_STENCIL8_OES)
    GL_DEPTH32F_STENCIL8_OES,
#else
    0,
#endif
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
#ifdef GL_RED8
    GL_RED8,
#elif defined(GL_RED)
    GL_RED,
#elif defined(GL_LUMINANCE8)
    GL_LUMINANCE8,
#else
    GL_LUMINANCE,
#endif
#ifdef GL_RG8
    GL_RG8,
#elif defined(GL_RG)
    GL_RG,
#elif defined(GL_LUMINANCE_ALPHA8)
    GL_LUMINANCE_ALPHA8,
#else
    GL_LUMINANCE_ALPHA,
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

GLenum PHGLFormats[]  = {
    0, 
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
#ifdef GL_DEPTH_STENCIL
    GL_DEPTH_STENCIL,
#elif defined(GL_DEPTH_STENCIL_OES)
    GL_DEPTH_STENCIL_OES,
#endif
#ifdef GL_DEPTH_STENCIL
    GL_DEPTH_STENCIL,
#elif defined(GL_DEPTH_STENCIL_OES)
    GL_DEPTH_STENCIL_OES,
#endif
    GL_ALPHA, 
    GL_LUMINANCE,
#ifdef GL_RED
    GL_RED,
#else
    GL_LUMINANCE,
#endif
#ifdef GL_RG
    GL_RG,
#else
    GL_LUMINANCE_ALPHA,
#endif
    GL_LUMINANCE_ALPHA,
    GL_RGBA, 
    GL_RGBA, 
    GL_RGB, 
    GL_RGB
};

GLenum PHGLTypes[] = {
    0, 
    GL_UNSIGNED_SHORT, //D16
    GL_UNSIGNED_INT, //D24
    GL_UNSIGNED_INT, //D32
    GL_FLOAT, //D32F
    GL_UNSIGNED_INT, //D24S8
    GL_FLOAT, //D32FS8
    GL_UNSIGNED_BYTE, //A8
    GL_UNSIGNED_BYTE, //L8
    GL_UNSIGNED_BYTE, //R8
    GL_UNSIGNED_BYTE, //RG8
    GL_UNSIGNED_BYTE, //LA8
    GL_UNSIGNED_BYTE, //RGBA8
    GL_UNSIGNED_SHORT, //RGBA16
    GL_UNSIGNED_BYTE, //RGB8
    GL_UNSIGNED_SHORT //RGB16
};

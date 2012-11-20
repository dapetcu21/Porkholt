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
#ifdef GL_ALPHA16
    GL_ALPHA16,
#else
    GL_ALPHA,
#endif
#ifdef GL_LUMINANCE8
    GL_LUMINANCE8, 
#else
    GL_LUMINANCE,
#endif
#ifdef GL_LUMINANCE16
    GL_LUMINANCE16, 
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
#ifdef GL_RED16
    GL_RED16,
#elif defined(GL_RED)
    GL_RED,
#elif defined(GL_LUMINANCE16)
    GL_LUMINANCE16,
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
#ifdef GL_RG16
    GL_RG16,
#elif defined(GL_RG)
    GL_RG,
#elif defined(GL_LUMINANCE_ALPHA16)
    GL_LUMINANCE_ALPHA16,
#else
    GL_LUMINANCE_ALPHA,
#endif
    GL_LUMINANCE_ALPHA,
#ifdef GL_LUMINANCE_ALPHA16
    GL_LUMINANCE_ALPHA16,
#else
    GL_LUMINANCE_ALPHA,
#endif
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
    GL_RGB16,
#else
    GL_RGB,
#endif

#ifdef GL_R16F
    GL_R16F,
#elif defined(GL_RED)
    GL_RED,
#else
    GL_LUMINANCE,
#endif
#ifdef GL_RG16F
    GL_RG16F,
#elif defined(GL_RG)
    GL_RG,
#else
    GL_LUMINANCE_ALPHA,
#endif
#ifdef GL_RGB16F
    GL_RGB16F, 
#else
    GL_RGB,
#endif
#ifdef GL_RGBA16F
    GL_RGBA16F, 
#else
    GL_RGBA,
#endif

#ifdef GL_R32F
    GL_R32F,
#elif defined(GL_LUMINANCE32F_ARB)
    GL_LUMINANCE32F_ARB,
#elif defined(GL_RED)
    GL_RED,
#else
    GL_LUMINANCE,
#endif
#ifdef GL_RG32F
    GL_RG32F,
#elif defined(GL_RG)
    GL_RG,
#else
    GL_LUMINANCE_ALPHA,
#endif
#ifdef GL_RGB32F
    GL_RGB32F, 
#else
    GL_RGB,
#endif
#ifdef GL_RGBA16F
    GL_RGBA32F, 
#else
    GL_RGBA
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
    GL_ALPHA, 
    GL_LUMINANCE,
    GL_LUMINANCE,
#ifdef GL_RED
    GL_RED,
    GL_RED,
#else
    GL_LUMINANCE,
    GL_LUMINANCE,
#endif
#ifdef GL_RG
    GL_RG,
    GL_RG,
#else
    GL_LUMINANCE_ALPHA,
    GL_LUMINANCE_ALPHA,
#endif
    GL_LUMINANCE_ALPHA,
    GL_LUMINANCE_ALPHA,
    GL_RGBA, 
    GL_RGBA, 
    GL_RGB, 
    GL_RGB,

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
    GL_RGB,
    GL_RGBA, 

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
    GL_RGB,
    GL_RGBA
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
    GL_UNSIGNED_SHORT, //A16
    GL_UNSIGNED_BYTE, //L8
    GL_UNSIGNED_SHORT, //L16
    GL_UNSIGNED_BYTE, //R8
    GL_UNSIGNED_SHORT, //R16
    GL_UNSIGNED_BYTE, //RG8
    GL_UNSIGNED_SHORT, //RG16
    GL_UNSIGNED_BYTE, //LA8
    GL_UNSIGNED_SHORT, //LA16
    GL_UNSIGNED_BYTE, //RGBA8
    GL_UNSIGNED_SHORT, //RGBA16
    GL_UNSIGNED_BYTE, //RGB8
    GL_UNSIGNED_SHORT, //RGB16
    GL_FLOAT, //R16F
    GL_FLOAT, //RG16F
    GL_FLOAT, //RGB16F
    GL_FLOAT, //RGBA16F
    GL_FLOAT, //R32F
    GL_FLOAT, //RG32F
    GL_FLOAT, //RGB32F
    GL_FLOAT //RGBA32F
};

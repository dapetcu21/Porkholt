/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGameManager.h>

#define USE_DL

#ifdef USE_DL
#include <dlfcn.h>
#endif

void * PHGameManager::glFunctionAddress(const char * s)
{
#if defined(_WIN32)
    return wglGetProcAddress((LPCSTR)s);
#elif defined(USE_DL)
    static void * h = NULL;
    static void * f;
    if (!h)
    {
        if ((h = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL)) == NULL)
            return NULL;
        PHLog("%llx", (long long unsigned)(size_t)h);
        f = dlsym(h, "glXGetProcAddress");
        if (!f)
            f = dlsym(h, "glXGetProcAddressARB");
        PHLog("%llx", (long long unsigned)(size_t)f);
    }
    if (f)
        return ((void*(*)(const GLubyte*))f)((const GLubyte*)s);
    else
        return dlsym(h, s);
#else
    return glXGetProcAddressARB((const GLubyte*)s);
#endif
}


#define set(name, val) name = (name ## _t)(val)
#define setf(name, val) name = (name ## _t)(glFunctionAddress(#val))

void PHGameManager::loadExtensionCompat()
{
    bool es = isGLES();
    int glMaj = openGLMajorVersion();
    int glMin = openGLMinorVersion();
    #ifndef PH_FORCE_FAKE_VAO
    if ((glMaj >= 3 && !es) || hasExtension("GL_ARB_vertex_array_object"))
    {
        setf(glBindVertexArray, glBindVertexArray);
        setf(glDeleteVertexArrays, glDeleteVertexArrays);
        setf(glGenVertexArrays, glGenVertexArrays);
    }
    else
    if (hasExtension("GL_OES_vertex_array_object"))
    {
        setf(glBindVertexArray, glBindVertexArrayOES);
        setf(glDeleteVertexArrays, glDeleteVertexArraysOES);
        setf(glGenVertexArrays, glGenVertexArraysOES);
    }
    else
    if (hasExtension("GL_APPLE_vertex_array_object"))
    {
        setf(glBindVertexArray, glBindVertexArrayAPPLE);
        setf(glDeleteVertexArrays, glDeleteVertexArraysAPPLE);
        setf(glGenVertexArrays, glGenVertexArraysAPPLE);
    }
    else
    #endif
    {
        set(glBindVertexArray, NULL);
        set(glDeleteVertexArrays, NULL);
        set(glGenVertexArrays, NULL);
    }
    if ((es?(glMaj>=2):(glMaj >=3)) || hasExtension("GL_ARB_framebuffer_object"))
    {
        setf(glIsRenderbuffer, glIsRenderbuffer);
        setf(glBindRenderbuffer, glBindRenderbuffer);
        setf(glDeleteRenderbuffers, glDeleteRenderbuffers);
        setf(glGenRenderbuffers, glGenRenderbuffers);
        setf(glRenderbufferStorage, glRenderbufferStorage);
        setf(glRenderbufferStorageMultisample, glRenderbufferStorageMultisample);
        setf(glGetRenderbufferParameteriv, glGetRenderbufferParameteriv);
        setf(glIsFramebuffer, glIsFramebuffer);
        setf(glBindFramebuffer, glBindFramebuffer);
        setf(glDeleteFramebuffers, glDeleteFramebuffers);
        setf(glGenFramebuffers, glGenFramebuffers);
        setf(glCheckFramebufferStatus, glCheckFramebufferStatus);
        setf(glFramebufferTexture1D, glFramebufferTexture1D);
        setf(glFramebufferTexture2D, glFramebufferTexture2D);
        setf(glFramebufferTexture3D, glFramebufferTexture3D);
        setf(glFramebufferTextureLayer, glFramebufferTextureLayer);
        setf(glFramebufferRenderbuffer, glFramebufferRenderbuffer);
        setf(glGetFramebufferAttachmentParameteriv, glGetFramebufferAttachmentParameteriv);
        setf(glBlitFramebuffer, glBlitFramebuffer);
        setf(glGenerateMipmap, glGenerateMipmap);
    } 
    else 
    if (hasExtension("GL_OES_framebuffer_object"))
    {
        setf(glIsRenderbuffer, glIsRenderbufferOES);
        setf(glBindRenderbuffer, glBindRenderbufferOES);
        setf(glDeleteRenderbuffers, glDeleteRenderbuffersOES);
        setf(glGenRenderbuffers, glGenRenderbuffersOES);
        setf(glRenderbufferStorage, glRenderbufferStorageOES);
        set (glRenderbufferStorageMultisample, NULL);
        setf(glGetRenderbufferParameteriv, glGetRenderbufferParameterivOES);
        setf(glIsFramebuffer, glIsFramebufferOES);
        setf(glBindFramebuffer, glBindFramebufferOES);
        setf(glDeleteFramebuffers, glDeleteFramebuffersOES);
        setf(glGenFramebuffers, glGenFramebuffersOES);
        setf(glCheckFramebufferStatus, glCheckFramebufferStatusOES);
        set (glFramebufferTexture1D, NULL);
        setf(glFramebufferTexture2D, glFramebufferTexture2DOES);
        set (glFramebufferTexture3D, NULL);
        set (glFramebufferTextureLayer, NULL);
        setf(glFramebufferRenderbuffer, glFramebufferRenderbufferOES);
        setf(glGetFramebufferAttachmentParameteriv, glGetFramebufferAttachmentParameterivOES);
        set (glBlitFramebuffer, NULL);
        setf(glGenerateMipmap, glGenerateMipmapOES);
    } else 
    {
        set(glIsRenderbuffer, NULL);
        set(glBindRenderbuffer, NULL);
        set(glDeleteRenderbuffers, NULL);
        set(glGenRenderbuffers, NULL);
        set(glRenderbufferStorage, NULL);
        set(glRenderbufferStorageMultisample, NULL);
        set(glGetRenderbufferParameteriv, NULL);
        set(glIsFramebuffer, NULL);
        set(glBindFramebuffer, NULL);
        set(glDeleteFramebuffers, NULL);
        set(glGenFramebuffers, NULL);
        set(glCheckFramebufferStatus, NULL);
        set(glFramebufferTexture1D, NULL);
        set(glFramebufferTexture2D, NULL);
        set(glFramebufferTexture3D, NULL);
        set(glFramebufferTextureLayer, NULL);
        set(glFramebufferRenderbuffer, NULL);
        set(glGetFramebufferAttachmentParameteriv, NULL);
        set(glBlitFramebuffer, NULL);
        set(glGenerateMipmap, NULL);
    }
    
    if (es)
        set(glClearDepth, glClearDepthf);
    else
        set(glClearDepth, glClearDepth);
}

#ifndef GL_MAX_COLOR_ATTACHMENTS
#define GL_MAX_COLOR_ATTACHMENTS 0x8CDF
#endif

int PHGameManager::colorAttachmentCount()
{
    if (!clat)
    {
        GLint v;
        glGetError();
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &v);
        if (glGetError() == GL_INVALID_ENUM)
            clat = 1;
        else
            clat = v;
    }
    return clat;
}

void PHGameManager::setClearColor(const PHColor & c)
{
    glClearColor(c.r, c.g, c.b, c.a);
}

void PHGameManager::setDepthClearValue(float val)
{
    glClearDepth(val);
}

void PHGameManager::setStencilClearValue(int val)
{
    glClearStencil(val);
}

void PHGameManager::clearBuffers(int m)
{
    GLbitfield mask = 0;
    if (m&colorBuffers) 
        mask |= GL_COLOR_BUFFER_BIT;
    if (m&depthBuffer)
        mask |= GL_DEPTH_BUFFER_BIT;
    if (m&stencilBuffer)
        mask |= GL_STENCIL_BUFFER_BIT;
    glClear(mask);
}

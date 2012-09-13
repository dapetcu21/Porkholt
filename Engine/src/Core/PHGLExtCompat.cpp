/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGameManager.h>

#define USE_DL

#ifdef USE_DL
#include <dlfcn.h>
#endif

void * PHGL::glFunctionAddress(const char * s)
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
        f = dlsym(h, "glXGetProcAddress");
        if (!f)
            f = dlsym(h, "glXGetProcAddressARB");
    }
    if (f)
        return ((void*(*)(const GLubyte*))f)((const GLubyte*)s);
    else
        return dlsym(h, s);
#else
    return glXGetProcAddressARB((const GLubyte*)s);
#endif
}

void PHGameManager::loadCapabilities()
{
    if (!parsedExtensions)
    {
        const char * ver = (char*) PHGL::glGetString(GL_VERSION);
        const char * vers = ver;
        bool es = strstr(ver, "ES");
        openGLCaps[PHGLCapabilityOpenGLES] = es;
        
        int number = 0;
        bool major = true;
        while (*ver)
        {
            if ((*ver)=='.')
            {
                if (major)
                {
                    openGLVersionMajor = number;
                    major = false;
                    number = 0;
                }
                else 
                    break;
            } else
                if (('0'<=(*ver)) && ((*ver)<='9'))
                    number=number*10+*ver-'0';
                else
                    if (number || !major)
                        break;
            ver++;
        }
        openGLVersionMinor = number;
        
#ifdef GL_NUM_EXTENSIONS
        if (!es && openGLVersionMajor>=3)
        {
            GLint n;
            PHGL::glGetIntegerv(GL_NUM_EXTENSIONS, &n);
            for (int i=0; i<n; i++)
                extensions.insert(string((const char*)PHGL::glGetStringi(GL_EXTENSIONS, i)));
        } else
#endif
        {
            const char * ext = (char*) PHGL::glGetString(GL_EXTENSIONS);
            while (ext && ext[0])
            {
                const char * pos = strpbrk(ext, " ");
                if (!pos)
                    pos = ext+strlen(ext);
                extensions.insert(string(ext,pos-ext));
                if (!pos[0])
                    ext = pos;
                else
                    ext = pos+1;
            }
        }
        parsedExtensions = true;
        
        glslVersion = 0;
        const char * glslvp = (const char*)PHGL::glGetString(GL_SHADING_LANGUAGE_VERSION);
        const char * glslv = glslvp;
        for (;(*glslv) && glslVersion<100; glslv++)
        {
            char c = *glslv;
            if (c>='0' && c<='9')
                glslVersion = glslVersion*10 + (c-'0');
        }
        while (glslVersion && glslVersion<100)
            glslVersion *= 10;
        
#ifdef PH_DEBUG
        string s;
        for (set<string>::iterator i = extensions.begin(); i!=extensions.end(); i++)
        {
            s+=*i;
            s+=" ";
        }
        
        PHLog("OpenGL Version: \"%s\" -> %s %d.%d",vers,es?"ES":"",openGLVersionMajor,openGLVersionMinor);
        PHLog("GLSL Version: \"%s\" -> %d",glslvp,glslVersion);
        PHLog("OpenGL Extensions: %s",s.c_str());
#endif
        
        openGLCaps[PHGLCapabilityNPOT] = (openGLVersionMajor>=2 || extensions.count("OES_texture_npot") || extensions.count("GL_ARB_texture_non_power_of_two") || extensions.count("GL_IMG_texture_npot"));
        openGLCaps[PHGLCapabilityAppleLimitedNPOT] =
            (extensions.count("GL_APPLE_texture_2D_limited_npot") && !extensions.count("OES_texture_npot")) 
#ifdef PH_IPHONE_OS
            || (es && (openGLVersionMajor==2));
#endif
        ;
        openGLCaps[PHGLCapabilityGLES1] = es && (openGLVersionMajor == 1);
        openGLCaps[PHGLCapabilityGLES2] = es && (openGLVersionMajor >= 2);
        openGLCaps[PHGLCapabilityShaders] = (openGLVersionMajor>=2);
        GLint tmp;
        PHGL::glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &tmp);
        _maxVertexAttribs = (int)tmp;
        
        stringstream sstr;
        sstr<<"#version "<<glslVersion<<"\n";
        sstr<<"#define PH_ENGINE\n";
        sstr<<"#define PH_GLSL_VERSION "<<glslVersion<<"\n";
        sstr<<"#define PH_GL_MINOR_VERSION "<<openGLVersionMinor<<"\n";
        sstr<<"#define PH_GL_MAJOR_VERSION "<<openGLVersionMajor<<"\n";
        if (!es)
        {
            sstr<<"#define highp \n";
            sstr<<"#define mediump \n";
            sstr<<"#define lowp \n";
            sstr<<"#define PH_GL_ES\n";
        }
        
        glslHeader = sstr.str();
        
    }
}

bool PHGameManager::hasExtension(const string & ext)
{
    return extensions.count(ext);
}

bool PHGameManager::phglinited = false;

#define sete(f, e) PHGLSetf(f, f ## e)
#define set(f) PHGLSetf(f, f)
#define setn(f) PHGLSet(f, NULL)

void PHGameManager::initPHGL()
{
    if (phglinited)
    {
        loadCapabilities();
        return;
    }
    phglinited = true;

    //load non-extension stuff

    set(glGetBooleanv);
    set(glGetDoublev);
    set(glGetFloatv);
    set(glGetIntegerv);
    set(glGetString);
    set(glEnable);
    set(glDisable);

    loadCapabilities();

    bool es = isGLES();
    int glMaj = openGLMajorVersion();
    int glMin = openGLMinorVersion();

    if (es && glMaj>=2)
    {
        setn(glEnableClientState);
        setn(glDisableClientState);
    } else {
        set(glEnableClientState);
        set(glDisableClientState);
    }

    #ifndef PH_FORCE_FAKE_VAO
    if ((glMaj >= 3 && !es) || hasExtension("GL_ARB_vertex_array_object"))
    {
        PHGLSetf(glBindVertexArray, glBindVertexArray);
        PHGLSetf(glDeleteVertexArrays, glDeleteVertexArrays);
        PHGLSetf(glGenVertexArrays, glGenVertexArrays);
    }
    else
    if (hasExtension("GL_OES_vertex_array_object"))
    {
        PHGLSetf(glBindVertexArray, glBindVertexArrayOES);
        PHGLSetf(glDeleteVertexArrays, glDeleteVertexArraysOES);
        PHGLSetf(glGenVertexArrays, glGenVertexArraysOES);
    }
    else
    if (hasExtension("GL_APPLE_vertex_array_object"))
    {
        PHGLSetf(glBindVertexArray, glBindVertexArrayAPPLE);
        PHGLSetf(glDeleteVertexArrays, glDeleteVertexArraysAPPLE);
        PHGLSetf(glGenVertexArrays, glGenVertexArraysAPPLE);
    }
    else
    #endif
    {
        PHGLSet(glBindVertexArray, NULL);
        PHGLSet(glDeleteVertexArrays, NULL);
        PHGLSet(glGenVertexArrays, NULL);
    }
    if ((es?(glMaj>=2):(glMaj >=3)) || hasExtension("GL_ARB_framebuffer_object"))
    {
        PHGLSetf(glIsRenderbuffer, glIsRenderbuffer);
        PHGLSetf(glBindRenderbuffer, glBindRenderbuffer);
        PHGLSetf(glDeleteRenderbuffers, glDeleteRenderbuffers);
        PHGLSetf(glGenRenderbuffers, glGenRenderbuffers);
        PHGLSetf(glRenderbufferStorage, glRenderbufferStorage);
        PHGLSetf(glRenderbufferStorageMultisample, glRenderbufferStorageMultisample);
        PHGLSetf(glGetRenderbufferParameteriv, glGetRenderbufferParameteriv);
        PHGLSetf(glIsFramebuffer, glIsFramebuffer);
        PHGLSetf(glBindFramebuffer, glBindFramebuffer);
        PHGLSetf(glDeleteFramebuffers, glDeleteFramebuffers);
        PHGLSetf(glGenFramebuffers, glGenFramebuffers);
        PHGLSetf(glCheckFramebufferStatus, glCheckFramebufferStatus);
        PHGLSetf(glFramebufferTexture1D, glFramebufferTexture1D);
        PHGLSetf(glFramebufferTexture2D, glFramebufferTexture2D);
        PHGLSetf(glFramebufferTexture3D, glFramebufferTexture3D);
        PHGLSetf(glFramebufferTextureLayer, glFramebufferTextureLayer);
        PHGLSetf(glFramebufferRenderbuffer, glFramebufferRenderbuffer);
        PHGLSetf(glGetFramebufferAttachmentParameteriv, glGetFramebufferAttachmentParameteriv);
        PHGLSetf(glBlitFramebuffer, glBlitFramebuffer);
        PHGLSetf(glGenerateMipmap, glGenerateMipmap);
    } 
    else 
    if (hasExtension("GL_OES_framebuffer_object"))
    {
        PHGLSetf(glIsRenderbuffer, glIsRenderbufferOES);
        PHGLSetf(glBindRenderbuffer, glBindRenderbufferOES);
        PHGLSetf(glDeleteRenderbuffers, glDeleteRenderbuffersOES);
        PHGLSetf(glGenRenderbuffers, glGenRenderbuffersOES);
        PHGLSetf(glRenderbufferStorage, glRenderbufferStorageOES);
        PHGLSet (glRenderbufferStorageMultisample, NULL);
        PHGLSetf(glGetRenderbufferParameteriv, glGetRenderbufferParameterivOES);
        PHGLSetf(glIsFramebuffer, glIsFramebufferOES);
        PHGLSetf(glBindFramebuffer, glBindFramebufferOES);
        PHGLSetf(glDeleteFramebuffers, glDeleteFramebuffersOES);
        PHGLSetf(glGenFramebuffers, glGenFramebuffersOES);
        PHGLSetf(glCheckFramebufferStatus, glCheckFramebufferStatusOES);
        PHGLSet (glFramebufferTexture1D, NULL);
        PHGLSetf(glFramebufferTexture2D, glFramebufferTexture2DOES);
        PHGLSet (glFramebufferTexture3D, NULL);
        PHGLSet (glFramebufferTextureLayer, NULL);
        PHGLSetf(glFramebufferRenderbuffer, glFramebufferRenderbufferOES);
        PHGLSetf(glGetFramebufferAttachmentParameteriv, glGetFramebufferAttachmentParameterivOES);
        PHGLSet (glBlitFramebuffer, NULL);
        PHGLSetf(glGenerateMipmap, glGenerateMipmapOES);
    } else 
    {
        PHGLSet(glIsRenderbuffer, NULL);
        PHGLSet(glBindRenderbuffer, NULL);
        PHGLSet(glDeleteRenderbuffers, NULL);
        PHGLSet(glGenRenderbuffers, NULL);
        PHGLSet(glRenderbufferStorage, NULL);
        PHGLSet(glRenderbufferStorageMultisample, NULL);
        PHGLSet(glGetRenderbufferParameteriv, NULL);
        PHGLSet(glIsFramebuffer, NULL);
        PHGLSet(glBindFramebuffer, NULL);
        PHGLSet(glDeleteFramebuffers, NULL);
        PHGLSet(glGenFramebuffers, NULL);
        PHGLSet(glCheckFramebufferStatus, NULL);
        PHGLSet(glFramebufferTexture1D, NULL);
        PHGLSet(glFramebufferTexture2D, NULL);
        PHGLSet(glFramebufferTexture3D, NULL);
        PHGLSet(glFramebufferTextureLayer, NULL);
        PHGLSet(glFramebufferRenderbuffer, NULL);
        PHGLSet(glGetFramebufferAttachmentParameteriv, NULL);
        PHGLSet(glBlitFramebuffer, NULL);
        PHGLSet(glGenerateMipmap, NULL);
    }

    set(glGenBuffers);
    set(glDeleteBuffers);
    set(glIsBuffer);
    set(glBindBuffer);
    set(glBufferData);
    set(glBufferSubData);

    set(glDepthMask);
    set(glDepthFunc);
    set(glCullFace);
    set(glBlendFunc);
    set(glPolygonMode);
    set(glPixelStorei);
    set(glPixelStoref);
    set(glTexEnvi);
    set(glTexEnvf);
    
    if (es)
        PHGLSetf(glClearDepth, glClearDepthf);
    else
        PHGLSetf(glClearDepth, glClearDepth);
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
    if (ccolor == c) return;
    glClearColor(c.r, c.g, c.b, c.a);
    ccolor = c;
}

void PHGameManager::setDepthClearValue(float val)
{
    if (cdepth == val) return;
    PHGL::glClearDepth(val);
    cdepth = val;
}

void PHGameManager::setStencilClearValue(int val)
{
    if (cstencil == val)
    glClearStencil(val);
    cstencil = val;
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

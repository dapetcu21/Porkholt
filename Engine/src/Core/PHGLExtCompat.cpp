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
        const char * libName[] = {NULL, NULL, NULL, NULL, NULL, NULL};
        switch (linkedLibrary)
        {
            case libOpenGL:
                libName[0] = "libGL.so";
                break;
            case libOpenGLES1:
                libName[0] = "libGLESv1_CM.so";
                libName[1] = "libGLES_CM.so";
                libName[2] = "libGLESv1.so";
                libName[3] = "libGLES.so";
                break;
            case libOpenGLES2:
                libName[0] = "libGLESv2.so";
                libName[1] = "libGLESv1_CM.so";
                libName[2] = "libGLES_CM.so";
                libName[3] = "libGLESv1.so";
                libName[4] = "libGLES.so";
                break;
        }
        for (int i = 0; !h && libName[i]; i++)
            h = dlopen(libName[i], RTLD_LAZY | RTLD_LOCAL);
        if (!h)
        {
            h = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL);
            if (!h)
                return NULL;
        }
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
        if (glslvp)
        {
            const char * glslv = glslvp;
            for (;(*glslv) && glslVersion<100; glslv++)
            {
                char c = *glslv;
                if (c>='0' && c<='9')
                    glslVersion = glslVersion*10 + (c-'0');
            }
            while (glslVersion && glslVersion<100)
                glslVersion *= 10;
        }
        
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

void PHGameManager::initPHGL()
{
    if (phglinited)
    {
        loadCapabilities();
        return;
    }
    phglinited = true;

    set(glGetBooleanv);
    set(glGetDoublev);
    set(glGetFloatv);
    set(glGetIntegerv);
    set(glGetString);
    set(glGetStringi);
    set(glEnable);
    set(glDisable);
    set(glGetError);

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
    set(glViewport);
    set(glColor4ub);
    
    set(glGenTextures);
    set(glDeleteTextures);
    set(glBindTexture);
    set(glActiveTexture);
    set(glTexParameteri);
#ifdef GL_TEXTURE_1D
    set(glTexImage1D);
#endif
#ifdef GL_TEXTURE_2D
    set(glTexImage2D);
#endif
#ifdef GL_TEXTURE_3D
    set(glTexImage3D);
#endif

    set(glClearColor);
    set(glClearStencil);
    set(glClear);

    set(glDrawElements);
    set(glDrawArrays);
    
    loadCapabilities();

    bool es = isGLES();
    int glMaj = openGLMajorVersion();
    int glMin = openGLMinorVersion();

    if (es)
        sete(glClearDepth, f);
    else
        set(glClearDepth);

    if (useShaders())
    {
        set(glDisableVertexAttribArray);
        set(glEnableVertexAttribArray);
        set(glBindAttribLocation);
        set(glVertexAttribPointer);

        set(glCreateShader);
        set(glDeleteShader);
        set(glCompileShader);
        set(glAttachShader);
        set(glDetachShader);
        set(glShaderSource);
        set(glGetShaderiv);
        set(glGetShaderInfoLog);

        set(glCreateProgram);
        set(glDeleteProgram);
        set(glLinkProgram);
        set(glUseProgram);
        set(glValidateProgram);
        set(glGetProgramiv);
        set(glGetProgramInfoLog);

        set(glGetUniformLocation);
        set(glUniform1f);
        set(glUniform2f);
        set(glUniform3f);
        set(glUniform4f);
        set(glUniform1i);
        set(glUniform2i);
        set(glUniform3i);
        set(glUniform4i);
        set(glUniformMatrix4fv);
    } else {
        set(glEnableClientState);
        set(glDisableClientState);

        set(glVertexPointer);
        set(glColorPointer);
        set(glNormalPointer);
        set(glTexCoordPointer);

        set(glMatrixMode);
        set(glLoadMatrixf);
    }

    #ifndef PH_FORCE_FAKE_VAO
    if ((glMaj >= 3 && !es) || hasExtension("GL_ARB_vertex_array_object"))
    {
        set(glBindVertexArray);
        set(glDeleteVertexArrays);
        set(glGenVertexArrays);
    }
    else
    if (hasExtension("GL_OES_vertex_array_object"))
    {
        sete(glBindVertexArray, OES);
        sete(glDeleteVertexArrays, OES);
        sete(glGenVertexArrays, OES);
    }
    else
    if (hasExtension("GL_APPLE_vertex_array_object"))
    {
        sete(glBindVertexArray, APPLE);
        sete(glDeleteVertexArrays, APPLE);
        sete(glGenVertexArrays, APPLE);
    }
    #endif

    if ((es?(glMaj>=2):(glMaj >=3)) || hasExtension("GL_ARB_framebuffer_object"))
    {
        set(glIsRenderbuffer);
        set(glBindRenderbuffer);
        set(glDeleteRenderbuffers);
        set(glGenRenderbuffers);
        set(glRenderbufferStorage);
        set(glRenderbufferStorageMultisample);
        set(glGetRenderbufferParameteriv);
        set(glIsFramebuffer);
        set(glBindFramebuffer);
        set(glDeleteFramebuffers);
        set(glGenFramebuffers);
        set(glCheckFramebufferStatus);
        set(glFramebufferTexture1D);
        set(glFramebufferTexture2D);
        set(glFramebufferTexture3D);
        set(glFramebufferTextureLayer);
        set(glFramebufferRenderbuffer);
        set(glGetFramebufferAttachmentParameteriv);
        set(glBlitFramebuffer);
        set(glGenerateMipmap);
    } 
    else 
    if (hasExtension("GL_OES_framebuffer_object"))
    {
        sete(glIsRenderbuffer, OES);
        sete(glBindRenderbuffer, OES);
        sete(glDeleteRenderbuffers, OES);
        sete(glGenRenderbuffers, OES);
        sete(glRenderbufferStorage, OES);
        sete(glGetRenderbufferParameteriv, OES);
        sete(glIsFramebuffer, OES);
        sete(glBindFramebuffer, OES);
        sete(glDeleteFramebuffers, OES);
        sete(glGenFramebuffers, OES);
        sete(glCheckFramebufferStatus, OES);
        sete(glFramebufferTexture2D, OES);
        sete(glFramebufferRenderbuffer, OES);
        sete(glGetFramebufferAttachmentParameteriv, OES);
        sete(glGenerateMipmap, OES);
    }

}

#ifndef GL_MAX_COLOR_ATTACHMENTS
#define GL_MAX_COLOR_ATTACHMENTS 0x8CDF
#endif

int PHGameManager::colorAttachmentCount()
{
    if (!clat)
    {
        GLint v;
        PHGL::glGetError();
        PHGL::glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &v);
        if (PHGL::glGetError() == GL_INVALID_ENUM)
            clat = 1;
        else
            clat = v;
    }
    return clat;
}

void PHGameManager::setClearColor(const PHColor & c)
{
    if (ccolor == c) return;
    PHGL::glClearColor(c.r, c.g, c.b, c.a);
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
    PHGL::glClearStencil(val);
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
    PHGL::glClear(mask);
}

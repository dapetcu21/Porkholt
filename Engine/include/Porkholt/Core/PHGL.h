/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHGL_H
#define PHGL_H

#if defined(PH_IPHONE_OS)
	#include <OpenGLES/ES1/gl.h>
	#include <OpenGLES/ES1/glext.h>
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif defined(PH_MAC_OS)
    #include <OpenGL/gl.h>
    #include <OpenGL/glext.h>
#elif defined(PH_GLES)
    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#else
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif

#ifdef PH_GLES
    typedef double GLdouble;
#endif

namespace PHGL {

#ifdef PHGL_CPP
    #define PHGLDefine(ret, name, ...) \
    typedef ret (* name ## _t )(__VA_ARGS__); \
    name ## _t name
#else 
    #define PHGLDefine(ret, name, ...) \
    typedef ret (* name ## _t )(__VA_ARGS__); \
    extern name ## _t name
#endif


    PHGLDefine(GLvoid,    glGetBooleanv, GLenum, GLboolean*);
    PHGLDefine(GLvoid,    glGetDoublev , GLenum, GLdouble*);
    PHGLDefine(GLvoid,    glGetFloatv, GLenum,   GLfloat*);
    PHGLDefine(GLvoid,    glGetIntegerv, GLenum, GLint*);
    PHGLDefine(const GLubyte*, glGetString, GLenum);

    PHGLDefine(GLvoid,    glEnable, GLenum);
    PHGLDefine(GLvoid,    glDisable, GLenum);
    PHGLDefine(GLvoid,    glEnableClientState, GLenum);
    PHGLDefine(GLvoid,    glDisableClientState, GLenum);

    PHGLDefine(GLvoid,    glBindVertexArray, GLuint);
    PHGLDefine(GLvoid,    glDeleteVertexArrays, GLsizei, const GLuint *);
    PHGLDefine(GLvoid,    glGenVertexArrays, GLsizei n, GLuint *);

    PHGLDefine(GLboolean, glIsRenderbuffer, GLuint);
    PHGLDefine(GLvoid,    glBindRenderbuffer, GLenum, GLuint);
    PHGLDefine(GLvoid,    glDeleteRenderbuffers, GLsizei, const GLuint *);
    PHGLDefine(GLvoid,    glGenRenderbuffers, GLsizei, GLuint *);

    PHGLDefine(GLvoid,    glRenderbufferStorage, GLenum, GLenum, GLsizei, GLsizei);
    PHGLDefine(GLvoid,    glRenderbufferStorageMultisample, GLenum, GLsizei, GLenum, GLsizei, GLsizei);
    PHGLDefine(GLvoid,    glGetRenderbufferParameteriv, GLenum, GLenum, GLint*);

    PHGLDefine(GLboolean, glIsFramebuffer, GLuint);
    PHGLDefine(GLvoid,    glBindFramebuffer, GLenum, GLuint);
    PHGLDefine(GLvoid,    glDeleteFramebuffers, GLsizei, const GLuint *);
    PHGLDefine(GLvoid,    glGenFramebuffers, GLsizei, GLuint *);

    PHGLDefine(GLenum,    glCheckFramebufferStatus, GLenum);
    PHGLDefine(GLvoid,    glFramebufferTexture1D, GLenum, GLenum, GLenum, GLuint, GLint);
    PHGLDefine(GLvoid,    glFramebufferTexture2D, GLenum, GLenum, GLenum, GLuint, GLint);
    PHGLDefine(GLvoid,    glFramebufferTexture3D, GLenum, GLenum, GLenum, GLuint, GLint, GLint);
    PHGLDefine(GLvoid,    glFramebufferTextureLayer, GLenum, GLenum, GLuint, GLint, GLint);
    PHGLDefine(GLvoid,    glFramebufferRenderbuffer, GLenum, GLenum, GLenum, GLuint);
    PHGLDefine(GLvoid,    glGetFramebufferAttachmentParameteriv, GLenum, GLenum, GLenum, GLint *);
    PHGLDefine(GLvoid,    glBlitFramebuffer, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);

    PHGLDefine(GLvoid,    glGenerateMipmap, GLenum);

    PHGLDefine(GLvoid,    glGenBuffers, GLsizei, GLuint*);
    PHGLDefine(GLvoid,    glDeleteBuffers, GLsizei, GLuint*);
    PHGLDefine(GLvoid,    glIsBuffer, GLuint);
    PHGLDefine(GLvoid,    glBindBuffer, GLenum, GLuint);
    PHGLDefine(GLvoid,    glBufferData, GLenum, GLsizeiptr, const GLvoid*, GLenum);
    PHGLDefine(GLvoid,    glBufferSubData, GLenum, GLintptr, GLsizeiptr, const GLvoid*);

#ifdef PH_GLES
    PHGLDefine(GLvoid,    glClearDepth, GLfloat);
#else
    PHGLDefine(GLvoid,    glClearDepth, GLclampd);
#endif
    PHGLDefine(GLvoid,    glDepthMask, GLenum);
    PHGLDefine(GLvoid,    glDepthFunc, GLenum);
    PHGLDefine(GLvoid,    glCullFace, GLenum);
    PHGLDefine(GLvoid,    glBlendFunc, GLenum, GLenum);
    PHGLDefine(GLvoid,    glPolygonMode, GLenum, GLenum);
    PHGLDefine(GLvoid,    glPixelStorei, GLenum, GLint);
    PHGLDefine(GLvoid,    glPixelStoref, GLenum, GLfloat);
    PHGLDefine(GLvoid,    glTexEnvi, GLenum, GLenum, GLint);
    PHGLDefine(GLvoid,    glTexEnvf, GLenum, GLenum, GLfloat);

    enum libs {
        libOpenGL,
        libOpenGLES1,
        libOpenGLES2
    };

#ifndef PHGL_CPP
    extern
#endif
    enum libs linkedLibrary;

    void * glFunctionAddress(const char * s);

    #define PHGLSet(name, val) PHGL::name = (PHGL::name ## _t)(val)
    #define PHGLSetf(name, val) PHGL::name = (PHGL::name ## _t)(PHGL::glFunctionAddress(#val))

};

#endif

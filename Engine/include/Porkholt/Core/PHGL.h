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
    #define def(ret, name, ...) \
    typedef ret (* name ## _t )(__VA_ARGS__); \
    name ## _t name = (name ## _t)NULL
#else 
    #define def(ret, name, ...) \
    typedef ret (* name ## _t )(__VA_ARGS__); \
    extern name ## _t name
#endif

    def(void, glGetBooleanv, GLenum, GLboolean*);
    def(void, glGetDoublev , GLenum, GLdouble*);
    def(void, glGetFloatv, GLenum,   GLfloat*);
    def(void, glGetIntegerv, GLenum, GLint*);
    def(GLenum, glGetError);
    def(const GLubyte*, glGetString, GLenum);
    def(const GLubyte*, glGetStringi, GLenum, GLint);

    def(void, glClearColor, GLclampf, GLclampf, GLclampf, GLclampf);
    def(void, glClearStencil, GLint);
    def(void, glClear, GLbitfield);

    def(void, glEnable, GLenum);
    def(void, glDisable, GLenum);
    def(void, glEnableClientState, GLenum);
    def(void, glDisableClientState, GLenum);

    def(void, glEnableVertexAttribArray, GLuint);
    def(void, glDisableVertexAttribArray, GLuint);
    def(void, glBindAttribLocation, GLuint, GLuint, const GLchar*);
    def(void, glVertexAttribPointer, GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);

    def(void, glBindVertexArray, GLuint);
    def(void, glDeleteVertexArrays, GLsizei, const GLuint *);
    def(void, glGenVertexArrays, GLsizei n, GLuint *);

    def(GLboolean, glIsRenderbuffer, GLuint);
    def(void, glBindRenderbuffer, GLenum, GLuint);
    def(void, glDeleteRenderbuffers, GLsizei, const GLuint *);
    def(void, glGenRenderbuffers, GLsizei, GLuint *);

    def(void, glRenderbufferStorage, GLenum, GLenum, GLsizei, GLsizei);
    def(void, glRenderbufferStorageMultisample, GLenum, GLsizei, GLenum, GLsizei, GLsizei);
    def(void, glGetRenderbufferParameteriv, GLenum, GLenum, GLint*);

    def(GLboolean, glIsFramebuffer, GLuint);
    def(void, glBindFramebuffer, GLenum, GLuint);
    def(void, glDeleteFramebuffers, GLsizei, const GLuint *);
    def(void, glGenFramebuffers, GLsizei, GLuint *);

    def(GLenum, glCheckFramebufferStatus, GLenum);
    def(void, glFramebufferTexture1D, GLenum, GLenum, GLenum, GLuint, GLint);
    def(void, glFramebufferTexture2D, GLenum, GLenum, GLenum, GLuint, GLint);
    def(void, glFramebufferTexture3D, GLenum, GLenum, GLenum, GLuint, GLint, GLint);
    def(void, glFramebufferTextureLayer, GLenum, GLenum, GLuint, GLint, GLint);
    def(void, glFramebufferRenderbuffer, GLenum, GLenum, GLenum, GLuint);
    def(void, glGetFramebufferAttachmentParameteriv, GLenum, GLenum, GLenum, GLint *);
    def(void, glBlitFramebuffer, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);

    def(void, glGenerateMipmap, GLenum);

    def(void, glGenBuffers, GLsizei, GLuint*);
    def(void, glDeleteBuffers, GLsizei, GLuint*);
    def(void, glIsBuffer, GLuint);
    def(void, glBindBuffer, GLenum, GLuint);
    def(void, glBufferData, GLenum, GLsizeiptr, const GLvoid*, GLenum);
    def(void, glBufferSubData, GLenum, GLintptr, GLsizeiptr, const GLvoid*);

#ifdef PH_GLES
    def(void, glClearDepth, GLfloat);
#else
    def(void, glClearDepth, GLclampd);
#endif
    def(void, glDepthMask, GLenum);
    def(void, glDepthFunc, GLenum);
    def(void, glCullFace, GLenum);
    def(void, glBlendFunc, GLenum, GLenum);
    def(void, glPolygonMode, GLenum, GLenum);
    def(void, glPixelStorei, GLenum, GLint);
    def(void, glPixelStoref, GLenum, GLfloat);
    def(void, glTexEnvi, GLenum, GLenum, GLint);
    def(void, glTexEnvf, GLenum, GLenum, GLfloat);
    def(void, glViewport, GLint, GLint, GLsizei, GLsizei);
    def(void, glMatrixMode, GLenum);
    def(void, glLoadMatrixf, const GLfloat *);
    def(void, glColor4ub, GLubyte, GLubyte, GLubyte, GLubyte);

    def(GLuint, glCreateShader, GLenum);
    def(void, glDeleteShader, GLuint);
    def(void, glCompileShader, GLuint);
    def(void, glAttachShader, GLuint, GLuint);
    def(void, glDetachShader, GLuint, GLuint);
    def(void, glShaderSource, GLuint, GLsizei, const GLchar**, const GLint*);
    def(void, glGetShaderiv, GLuint, GLenum, GLint*);
    def(void, glGetShaderInfoLog, GLuint, GLsizei, GLsizei*, GLchar*);

    def(GLuint, glCreateProgram);
    def(void, glDeleteProgram, GLuint);
    def(void, glLinkProgram, GLuint);
    def(void, glValidateProgram, GLuint);
    def(void, glUseProgram, GLuint);
    def(void, glGetProgramiv, GLuint, GLenum, GLint*);
    def(void, glGetProgramInfoLog, GLuint, GLsizei, GLsizei*, GLchar*);

    def(void, glGenTextures, GLsizei, GLuint*);
    def(void, glDeleteTextures, GLsizei, GLuint*);
    def(void, glBindTexture, GLenum, GLuint);
    def(void, glActiveTexture, GLenum);
    def(void, glTexParameteri, GLenum, GLenum, GLint);
    def(void, glTexImage1D, GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
    def(void, glTexImage2D, GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
    def(void, glTexImage3D, GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);

    def(GLint, glGetUniformLocation, GLuint, const GLchar*);
    def(void, glUniform1f, GLint, GLfloat);
    def(void, glUniform2f, GLint, GLfloat, GLfloat);
    def(void, glUniform3f, GLint, GLfloat, GLfloat, GLfloat);
    def(void, glUniform4f, GLint, GLfloat, GLfloat, GLfloat, GLfloat);
    def(void, glUniform1i, GLint, GLint);
    def(void, glUniform2i, GLint, GLint, GLint);
    def(void, glUniform3i, GLint, GLint, GLint, GLint);
    def(void, glUniform4i, GLint, GLint, GLint, GLint, GLint);
    def(void, glUniformMatrix4fv, GLint, GLsizei, GLboolean, const GLfloat*);

    def(void, glDrawArrays, GLenum, GLint, GLsizei);
    def(void, glDrawElements, GLenum, GLsizei, GLenum, const GLvoid *);

    def(void, glVertexPointer, GLint, GLenum, GLsizei, const GLvoid *);
    def(void, glColorPointer, GLint, GLenum, GLsizei, const GLvoid *);
    def(void, glNormalPointer, GLenum, GLsizei, const GLvoid *);
    def(void, glTexCoordPointer, GLint, GLenum, GLsizei, const GLvoid *);

    def(void, glStencilFunc, GLenum, GLint, GLuint);
    def(void, glStencilOp, GLenum, GLenum, GLenum);
    def(void, glStencilMask, GLuint);

#undef def

    enum libs {
        libLinked,
        libOpenGL,
        libOpenGLES1,
        libOpenGLES2
    };

    extern enum libs linkedLibrary;

    void * glFunctionAddress(const char * s);

    #define PHGLSet(name, val) PHGL::name = (PHGL::name ## _t)(val)
    #define PHGLSetf(name, val) PHGL::name = (PHGL::name ## _t)(PHGL::glFunctionAddress(#val))

};

#endif

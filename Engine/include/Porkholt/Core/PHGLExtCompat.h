/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#define PHGLEXTDefine(ret, name, ...) \
typedef ret (* name ## _t )(__VA_ARGS__); \
name ## _t name


private:
PHGLEXTDefine(GLvoid,    glBindVertexArray, GLuint);
PHGLEXTDefine(GLvoid,    glDeleteVertexArrays, GLsizei, const GLuint *);
PHGLEXTDefine(GLvoid,    glGenVertexArrays, GLsizei n, GLuint *);

PHGLEXTDefine(GLboolean, glIsRenderbuffer, GLuint);
PHGLEXTDefine(GLvoid,    glBindRenderbuffer, GLenum, GLuint);
PHGLEXTDefine(GLvoid,    glDeleteRenderbuffers, GLsizei, const GLuint *);
PHGLEXTDefine(GLvoid,    glGenRenderbuffers, GLsizei, GLuint *);

PHGLEXTDefine(GLvoid,    glRenderbufferStorage, GLenum, GLenum, GLsizei, GLsizei);
PHGLEXTDefine(GLvoid,    glRenderbufferStorageMultisample, GLenum, GLsizei, GLenum, GLsizei, GLsizei);
PHGLEXTDefine(GLvoid,    glGetRenderbufferParameteriv, GLenum, GLenum, GLint*);

PHGLEXTDefine(GLboolean, glIsFramebuffer, GLuint);
PHGLEXTDefine(GLvoid,    glBindFramebuffer, GLenum, GLuint);
PHGLEXTDefine(GLvoid,    glDeleteFramebuffers, GLsizei, const GLuint *);
PHGLEXTDefine(GLvoid,    glGenFramebuffers, GLsizei, GLuint *);

PHGLEXTDefine(GLenum,    glCheckFramebufferStatus, GLenum);
PHGLEXTDefine(GLvoid,    glFramebufferTexture1D, GLenum, GLenum, GLenum, GLuint, GLint);
PHGLEXTDefine(GLvoid,    glFramebufferTexture2D, GLenum, GLenum, GLenum, GLuint, GLint);
PHGLEXTDefine(GLvoid,    glFramebufferTexture3D, GLenum, GLenum, GLenum, GLuint, GLint, GLint);
PHGLEXTDefine(GLvoid,    glFramebufferTextureLayer, GLenum, GLenum, GLuint, GLint, GLint);
PHGLEXTDefine(GLvoid,    glFramebufferRenderbuffer, GLenum, GLenum, GLenum, GLuint);
PHGLEXTDefine(GLvoid,    glGetFramebufferAttachmentParameteriv, GLenum, GLenum, GLenum, GLint *);
PHGLEXTDefine(GLvoid,    glBlitFramebuffer, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);

PHGLEXTDefine(GLvoid,    glGenerateMipmap, GLenum);

PHGLEXTDefine(GLvoid,    glClearDepth, GLclampf);



void loadExtensionCompat();

public:
void * glFunctionAddress(const char * s);
void setClearColor(const PHColor & c);
void setDepthClearValue(float val);
void setStencilClearValue(int val);
enum {
    colorBuffers = 1<<0,
    depthBuffer = 1<<2,
    stencilBuffer = 1<<3
};
void clearBuffers(int mask);
void clearColorBuffers() { clearBuffers(colorBuffers); }
void clearDepthBuffer() { clearBuffers(depthBuffer); }
void clearStencilBuffer() { clearBuffers(stencilBuffer); }

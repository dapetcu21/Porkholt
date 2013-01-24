/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLVertexBufferObject.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHThread.h>

GLenum PHGLVertexBufferObject::targets[] = {        
    0,
#ifdef GL_ARRAY_BUFFER
    GL_ARRAY_BUFFER, 
#else
    0,
#endif
#ifdef GL_ELEMENT_ARRAY_BUFFER
    GL_ELEMENT_ARRAY_BUFFER,
#else
    0,
#endif
#ifdef GL_PIXEL_PACK_BUFFER
    GL_PIXEL_PACK_BUFFER,
#else
    0,
#endif
#ifdef GL_PIXEL_UNPACK_BUFFER
    GL_PIXEL_UNPACK_BUFFER
#else
    0
#endif
};

GLenum PHGLVertexBufferObject::usages[] = {
#ifdef GL_STREAM_DRAW
    GL_STREAM_DRAW,
#else
    0,
#endif
#ifdef GL_STREAM_READ
    GL_STREAM_READ,
#else
    0,
#endif
#ifdef GL_STREAM_COPY
    GL_STREAM_COPY,
#else
    0,
#endif
#ifdef GL_STATIC_DRAW
    GL_STATIC_DRAW,
#else
    0,
#endif
#ifdef GL_STATIC_READ
    GL_STATIC_READ,
#else
    0,
#endif
#ifdef GL_STATIC_COPY
    GL_STATIC_COPY,
#else
    0,
#endif
#ifdef GL_DYNAMIC_DRAW
    GL_DYNAMIC_DRAW,
#else
    0,
#endif
#ifdef GL_DYNAMIC_READ
    GL_DYNAMIC_READ,
#else
    0,
#endif
#ifdef GL_DYNAMIC_COPY
    GL_DYNAMIC_COPY
#else
    0
#endif
};

PHGLVertexBufferObject::PHGLVertexBufferObject(PHGameManager * gameManager) : gm(gameManager), bound(0), _usage(usageNone), _size(0)
{
    PHGL::glGenBuffers(1, &vbo);
}

PHGLVertexBufferObject::~PHGLVertexBufferObject()
{
    if (PHThread::currentThread() == PHThread::mainThread())
        PHGL::glDeleteBuffers(1, &vbo);
    else
        gm->queueDeleteVBO(vbo);
}

void PHGLVertexBufferObject::bindTo(int target)
{
    gm->bindVBO(this, target);
}

void PHGLVertexBufferObject::unbind()
{
    if (bound)
        gm->bindVBO(NULL, bound);
}

#define bind_begin bool b = isBound(); PHGLVBO * old; \
if (!b) { \
    old = gm->boundVBO(arrayBuffer); \
    bindTo(arrayBuffer); }
#define bind_end if (!b) gm->bindVBO(old, arrayBuffer);

void PHGLVertexBufferObject::setData(const void * data, size_t size, int usage)
{
    bind_begin
    _size = size;
    _usage = usage;
    PHGL::glBufferData(targets[bound], size, data, usages[usage]);
    bind_end
}

void PHGLVertexBufferObject::setDataOptimally(const void * data, size_t size, int usage)
{
    if (usage == streamDraw && gm->hasCapability(PHGLCapabilityGLES1))
        usage = dynamicDraw;
    bool replace = (size <= _size) && (usage == _usage);
    if (replace)
    {
        switch (_usage)
        {
            case streamDraw:
            case streamCopy:
            case streamRead:
            case staticDraw:
            case staticRead:
            case staticCopy:
                replace = false;
                break;
            default:
                break;
        }
    }
    if (replace)
        setSubData(data, 0, size);
    else
        setData(data, size, usage);
}

void PHGLVertexBufferObject::setSubData(const void * data, size_t offset, size_t size)
{
    bind_begin
    PHGL::glBufferSubData(targets[bound], offset, size, data);
    bind_end
}

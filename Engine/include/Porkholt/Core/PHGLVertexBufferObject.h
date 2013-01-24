/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGLVERTEXBUFFEROBJECT_H
#define PHGLVERTEXBUFFEROBJECT_H

#include <Porkholt/Core/PHMain.h>

class PHGameManager;
class PHGLVertexBufferObject : public PHObject
{
protected:
    GLuint vbo;
    PHGameManager * gm;
    int bound;
    
    static GLenum targets[];
    static GLenum usages[];
    
    friend class PHGameManager;
    friend class PHGLVertexArrayObject;

    int _usage;
    size_t _size;
    
public:
    PHGLVertexBufferObject(PHGameManager * gameManager);
    ~PHGLVertexBufferObject();
    
    
    enum bindTargets
    {
        arrayBuffer = 1,
        elementArrayBuffer = 2,
        pixelPackBuffer = 3,
        pixelUnpackBuffer = 4
    };
    
    void bindTo(int target);
    void bindToArrayBuffer() { bindTo(arrayBuffer); }
    void unbind();
    bool isBound() { return bound != 0; }
    int boundTarget() { return bound; }
    
    enum usageEnum
    {
        streamDraw = 0,
        streamRead,
        streamCopy,
        staticDraw,
        staticRead,
        staticCopy,
        dynamicDraw,
        dynamicRead,
        dynamicCopy,
        usageNone
    };

    size_t size() { return _size; }
    int usage() { return _usage; }
    
    void setData(const void * data, size_t size, int usage = dynamicDraw);
    void setDataOptimally(const void * data, size_t size, int usage);
    void setSubData(const void * data, size_t offset, size_t size);
};

typedef PHGLVertexBufferObject PHGLVBO;

#endif

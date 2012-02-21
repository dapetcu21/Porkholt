//
//  PHGLVertexBufferObject.h
//  Porkholt
//
//  Created by Marius Petcu on 2/18/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHGLVERTEXBUFFEROBJECT_H
#define PHGLVERTEXBUFFEROBJECT_H

#include "PHMain.h"

class PHGameManager;
class PHGLVertexBufferObject : public PHObject
{
protected:
    GLuint vbo;
    PHGameManager * _gameManager;
    int bound;
    
    static GLenum targets[];
    static GLenum usages[];
    
    friend class PHGameManager;
    friend class PHGLVertexArrayObject;
    
public:
    PHGLVertexBufferObject(PHGameManager * gm);
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
        dynamicCopy
    };
    
    void setData(const void * data, size_t size, int usage);
    void setSubData(const void * data, size_t offset, size_t size);
    void setData(void * data, size_t size) {
        setData(data, size, dynamicDraw);
    }
};

typedef PHGLVertexBufferObject PHGLVBO;

#endif

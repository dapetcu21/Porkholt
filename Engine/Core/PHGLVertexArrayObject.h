//
//  PHGLVertexArrayObject.h
//  Porkholt
//
//  Created by Marius Petcu on 2/19/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHGLVERTEXARRAYOBJECT_H
#define PHGLVERTEXARRAYOBJECT_H

#include "PHMain.h"

class PHGameManager;
class PHGLVertexBufferObject;

class PHGLVertexArrayObject : public PHObject
{
protected:
    GLuint vao;
    friend class PHGameManager;
    bool bound;
    PHGameManager * gm;
    
    struct attribute
    {
        GLint size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        size_t offset;
        PHGLVertexBufferObject * vbo;
    };
    
    map<int, attribute* > attributes;
    PHGLVertexBufferObject * elementVBO;
    
    GLenum mode;
    GLsizei count;
    union {
        GLint first;
        size_t offset;
    };
    GLenum type;
    int drawType;
    bool editBind;
    
    enum drawTypes
    {
        drawNone = 0,
        drawElements,
        drawArrays
    };
    
public:
    PHGLVertexArrayObject(PHGameManager * gameManager);
    ~PHGLVertexArrayObject();
    
    void fakeBind();
    void fakeUnbind();

    void bind();
    void bindToEdit();
    void unbind();
    bool isBound() { return bound; }
    
    void vertexPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset, PHGLVertexBufferObject * vbo);
    void disableAttribute(GLuint index);
    
    void setDrawElements(GLenum mode, GLsizei count, GLenum type, size_t offset);
    void setDrawArrays(GLenum mode, GLint first, GLsizei count);
    void disableDrawing() { drawType = drawNone; }
    void draw();
};

typedef PHGLVertexArrayObject PHGLVAO;

#endif

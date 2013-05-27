/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHGLPOLYELEMENTVBO_H

#define PHGLPOLYELEMENTVBO_H

#include <Porkholt/Core/PHGLVertexBufferObject.h>

class PHGLVertexArrayObject;
class PHGLPolyElementVBO : public PHGLVertexBufferObject
{
    protected:
        int poly;
        size_t storage;
        GLenum dtype;
    public:
        PHGLPolyElementVBO(PHGameManager * gm, int poly = 4);
        
        int verticesPerPoly() { return poly; } 
        void setVerticesPerPoly(int p) { if (poly != p) { poly = p; storage = 0; } }
        void reserve(size_t r); //this rebuilds the entire vbo immediately
        void resize(size_t r, PHGLVertexArrayObject * vao = NULL); //this adaps the storage size logaritmically
        size_t storedPolys() { return storage; }
        GLenum dataType() { return dtype; }
};

#endif

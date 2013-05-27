/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PHGLPolyElementVBO.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>


PHGLPolyElementVBO::PHGLPolyElementVBO(PHGameManager * gm, int p) : PHGLVertexBufferObject(gm), poly(p), storage(0), dtype(GL_UNSIGNED_BYTE) {};

template <typename T> 
static void _reserve(PHGLPolyElementVBO * self, size_t r)
{
    int poly = self->verticesPerPoly();
    T * idx = new T[r * (poly + 2) - 2];
    for (T i = 0; i < poly; i++)
        idx[i] = i;
    for (size_t i = poly, j = poly; i < r * poly; i += poly, j += poly + 2)
    {
        idx[j] = i - 1;
        idx[j+1] = i;
        for (int k = 0; k < poly; k++)
            idx[j + 2 + k] = i + k;
    }
    self->setData(idx, sizeof(T) * (r * (poly + 2) - 2), PHGLVBO::dynamicDraw);
    delete[] idx;
}

void PHGLPolyElementVBO::reserve(size_t r)
{
    storage = r;
    if (r * poly -1 <= std::numeric_limits<GLubyte>::max())
    {
        _reserve<GLubyte>(this, r);
        dtype = GL_UNSIGNED_BYTE;
        return;
    }
    if (r * poly -1 <= std::numeric_limits<GLushort>::max())
    {
        _reserve<GLushort>(this, r);
        dtype = GL_UNSIGNED_SHORT;
        return;
    }
    #ifdef PH_DEBUG
    if (!gm->isGLES() && (r * poly -1 <= std::numeric_limits<GLuint>::max()))
    #endif
    {
        _reserve<GLuint>(this, r);
        dtype = GL_UNSIGNED_INT;
        return;
    }
    throw string("Too many vertices");
}

void PHGLPolyElementVBO::resize(size_t r, PHGLVertexArrayObject * vao)
{
    if (!r)
        r = 1;
    if (r > storage)
    {
        size_t s = storage;
        if (!s)
            s = 1;
        while (r > s)
            s <<= 1;
        reserve(s);
    } else
    if (r <= (storage >> 1))
    {
        size_t s = storage;
        while (r <= (s >> 1))
            s >>= 1;
        reserve(s);
    }
    PHAssert(((storage >> 1) < r) && (r <= storage));
    if (vao)
        vao->setDrawElements(GL_TRIANGLE_STRIP, (r * (poly + 2) - 2), dtype, 0);
}

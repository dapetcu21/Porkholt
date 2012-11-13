/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGWALLCURVE_H
#define IGWALLCURVE_H

#include <Porkholt/Core/PHCurve.h>

class IGWallCurve : public PHCurve
{
    protected:
        list<PHPoint> points;
        size_t count;
        ph_float _offset;
        ph_float _width;
        ph_float _limit;

        friend class IGWallManager;
    public:
        IGWallCurve();
        ~IGWallCurve();

        void setOffset(ph_float f) { _offset = f; modelChanged(); }
        ph_float offset() { return _offset; }
        void setWidth(ph_float w) { _width  = w; modelChanged(); }
        ph_float width() { return _width; }
        void setLimit(ph_float l) { _limit = l; modelChanged(); }
        ph_float limit() { return _limit; }

        GLfloat * vertexData(size_t & nvertices, const PHRect & texCoord);
        GLushort * indexData(const GLfloat vertices[] , size_t stride, size_t nvertices, size_t & ntriangles) { return NULL; }
};

#endif

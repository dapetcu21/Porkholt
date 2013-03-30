/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGWALLCURVE_H
#define IGWALLCURVE_H

#include <Porkholt/Core/PHCurve.h>

class IGWallCurve : public PHCurve
{
    protected:
        list<PHPoint> points;
        size_t count;
        float _offset;
        float _width;
        float _limit;

        friend class IGWallManager;

        GLfloat * vertexData(size_t & nvertices, const PHRect & texCoord);
    public:
        IGWallCurve();
        ~IGWallCurve();

        void setOffset(float f) { _offset = f; modelChanged(); }
        float offset() { return _offset; }
        void setWidth(float w) { _width  = w; modelChanged(); }
        float width() { return _width; }
        void setLimit(float l) { _limit = l; modelChanged(); }
        float limit() { return _limit; }

        bool perspectiveCorrectTextureMapping() { return true; }

        void rebuildVAO(PHGLVertexArrayObject * vbo, const PHRect & texCoord);
};

#endif

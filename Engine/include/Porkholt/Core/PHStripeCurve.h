/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSTRIPECURVE_H
#define PHSTRIPECURVE_H

#include <Porkholt/Core/PHCurve.h>

class PHStripeCurve : public PHCurve
{
public:
    class point {
    public:
        PHPoint pos,texCoords;
        point() {}
        point(const point & o) : pos(o.pos), texCoords(o.texCoords) {}
        point(const PHPoint & p, const PHPoint & txc) : pos(p), texCoords(txc) {}
        point(ph_float px, ph_float py, ph_float txc1, ph_float txc2) : pos(px,py), texCoords(txc1,txc2) {}
    };
    
protected:
    list<point> pnts;
    
    const list<point> points() { return pnts; }
    void setPoints(const list<point> p);
    size_t pointCount() { return pnts.size(); }
    void pushBackPoint(const point & p);
    void pushFrontPoint(const point & p);
    void popBackPoint();
    void popFrontPoint();
    
    void moveBy(const PHPoint & p);
    
    GLfloat * vertexData(size_t & nvertices, const PHRect & texCoord) ;
    GLushort * indexData(const GLfloat vertices[],size_t stride, size_t nvertices, size_t & ntriangles);
};

#endif

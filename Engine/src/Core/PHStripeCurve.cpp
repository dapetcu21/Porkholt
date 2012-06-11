/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHStripeCurve.h>

void PHStripeCurve::setPoints(const list<point> p)
{
    pnts = p;
    modelChanged();
}

void PHStripeCurve::pushBackPoint(const point & p)
{
    pnts.push_back(p);
    modelChanged();
}

void PHStripeCurve::pushFrontPoint(const point & p)
{
    pnts.push_front(p);
    modelChanged();
}

void PHStripeCurve::popBackPoint()
{
    pnts.pop_back();
    modelChanged();
}

void PHStripeCurve::popFrontPoint()
{
    pnts.pop_front();
    modelChanged();
}

void PHStripeCurve::moveBy(const PHPoint & p)
{
    for (list<point>::iterator i = pnts.begin(); i!= pnts.end(); i++)
        i->pos += p;
    modelChanged();
}

GLfloat * PHStripeCurve::vertexData(size_t & nvertices, const PHRect & texCoord) 
{
    GLfloat * res, * r;
    res = r = new GLfloat[(nvertices = pointCount())*4];
    for (list<point>::iterator i = pnts.begin(); i!= pnts.end(); i++)
    {
        const point & p = *i;
        r[0] = p.pos.x;
        r[1] = p.pos.y;
        r[2] = texCoord.x + p.texCoords.x * texCoord.width;
        r[4] = texCoord.y + p.texCoords.y * texCoord.height;
        r+=4;
    }

    return res;
}

GLushort * PHStripeCurve::indexData(const GLfloat vertices[],size_t stride, size_t nvertices, size_t & ntriangles)
{
    return NULL;
}

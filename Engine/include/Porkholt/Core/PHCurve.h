/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHCURVE_H
#define PHCURVE_H

#include <Porkholt/Core/PHMain.h>

class PHGLVertexArrayObject;

class PHCurve : public PHObject
{
public:
    class anchorPoint {
    public:
        PHPoint point;
        int tag;
        anchorPoint() : point(PHOriginPoint), tag(0) {}
        anchorPoint(const PHPoint & p, int t) : point(p), tag(t) {}
        anchorPoint(const anchorPoint & o) : point(o.point), tag(o.tag) {}
        bool operator == (const anchorPoint & o) const { return (tag==o.tag && point==o.point); }
    };
    
    static const vector<anchorPoint> * tesselate(const vector<anchorPoint> & points);
    static GLfloat * vertexDataFromAnchorList(const vector<anchorPoint> & anchors, const PHRect & texCoord, size_t & nvertices);
    static void textureCoordinatesFromAnchorList(GLfloat * buffer, size_t stride, const vector<anchorPoint> & anchors, const PHRect & texCoord);
    
    virtual void rebuildVAO(PHGLVertexArrayObject * vao, const PHRect & texCoord) = 0;
    
    static GLushort * triangulatePolygon(const GLfloat v[],size_t stride, size_t nvertices, size_t & ntriangles);
    static GLushort * triangulatePolygon(const vector<anchorPoint> & anchors, size_t & ntriangles);
    
protected:
    int commitDisableCount;
    void modelChanged();
    virtual void _modelChanged() {}
protected:
    multimap<PHObject *, PHInvocation> callbacks;
    
public:
    PHCurve() : commitDisableCount(0) {}
    
    void addCallback(PHInvocation cb) { callbacks.insert(make_pair<PHObject *,PHInvocation>(cb.target,cb)); }
    void removeCallback(PHObject * target) { callbacks.erase(target); }
    
    void beginCommitGrouping();
    void endCommitGrouping();

};


#endif

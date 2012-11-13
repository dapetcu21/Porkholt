/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHBEZIERPATH_H
#define PHBEZIERPATH_H

#include <Porkholt/Core/PHCurve.h>

class PHBezierPath : public PHCurve
{
protected:
    vector<anchorPoint> points;
    set<PHRange> curves;
    vector<anchorPoint> * cache;
    
    vector<anchorPoint> * bezierPath(vector<anchorPoint> & v);
    lua_State * L;
    
public:
    const vector<anchorPoint> & anchorPoints() { return points; }
    const set<PHRange> & bezierCurves() { return curves; }
    void setBezierCurves(const set<PHRange> & c);
    void setAnchorPoints(const vector<anchorPoint> & c);
    void addCurve(const PHRange & rng);
    void removeCurve(const PHRange & rng);
    const anchorPoint & anchorPointAtIndex(int index) { return points[index]; }
    void replaceAnchorPoint(const anchorPoint & pnt, int index);
    void insertAnchorPoints(const anchorPoint * ap, int nr,int index);
    template<class _Input_Iterator>
    void insertAnchorPoints(const anchorPoint * ap, int nr,int index);
    void insertAnchorPoints(vector<anchorPoint>::iterator first, vector<anchorPoint>::iterator last, int index);

    void insertAnchorPoint(const anchorPoint & ap, int index) { insertAnchorPoints(&ap, 1, index); }
    void removeAnchorPoints(const PHRange & range);
    void removeAnchorPoint(int index) { removeAnchorPoints(PHRange(index,1)); }
    
    const vector<anchorPoint> & calculatedVertices();
    
    void rebuildVAO(PHGLVertexArrayObject * vao, const PHRect & texCoord);
    
    PHBezierPath() : cache(NULL), L(NULL) {}
    ~PHBezierPath();
    static PHBezierPath * nonUniqueFromLua(lua_State * L) { return fromLua(L,false); }
    static PHBezierPath * fromLua(lua_State * L) { return fromLua(L,true); }
    static PHBezierPath * fromLua(lua_State * L, bool unique);
    void luaDestroy(PHObject * sender, void * ud);
    bool operator == (const PHBezierPath & othr);
    
protected:
    void insertOffsets(const PHRange & rng);
    void removeOffsets(const PHRange & rng);
    
    void _modelChanged();
};

#endif

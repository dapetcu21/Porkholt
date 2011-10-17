//
//  PHBezierPath.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 10/13/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHBEZIERPATH_H
#define PHBEZIERPATH_H

#include "PHMain.h"

class PHBezierPath : public PHObject
{
public:
    class anchorPoint {
    public:
        PHPoint point;
        int tag;
        anchorPoint() : point(PHOriginPoint), tag(0) {}
        anchorPoint(const PHPoint & p, int t) : point(p), tag(t) {}
        anchorPoint(const anchorPoint & o) : point(o.point), tag(o.tag) {}
    };
    
protected:
    vector<anchorPoint> points;
    set<PHRange> curves;
    int commitDisableCount;
    vector<anchorPoint> * cache;
    
    vector<anchorPoint> * bezierPath(vector<anchorPoint> & v);
    lua_State * L;
    
public:
    const vector<anchorPoint> & anchorPoints() { return points; }
    const set<PHRange> & bezierCurves() { return curves; }
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
    
    static const vector<anchorPoint> * tesselate(const vector<anchorPoint> & points);
    static GLushort * triangulate(const vector<anchorPoint> & points,int & n);
    
    const vector<anchorPoint> & calculatedVertices();
    
    void beginCommitGrouping();
    void endCommitGrouping();
    
    PHBezierPath() : L(NULL), cache(NULL), commitDisableCount(0) {}
    ~PHBezierPath();
    static PHBezierPath * fromLua(lua_State * L);
    void luaDestroy(PHObject * sender, void * ud);
    
protected:
    void modelChanged();
    void insertOffsets(const PHRange & rng);
    void removeOffsets(const PHRange & rng);
    
protected:
    multimap<PHObject *, PHInvocation> callbacks;
    
public:
    void addCallback(PHInvocation cb) { callbacks.insert(make_pair<PHObject *,PHInvocation>(cb.target,cb)); }
    void removeCallback(PHObject * target) { callbacks.erase(target); }
};

#endif

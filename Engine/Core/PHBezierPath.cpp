//
//  PHBezierPath.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 10/13/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHBezierPath.h"
#include "PHLua.h"
#include "PHMessage.h"

PHBezierPath * PHBezierPath::fromLua(lua_State * L, bool unique)
{
    if (!L) return NULL;
    if (lua_istable(L, -1))
    {
        if (unique)
        {
            lua_getfield(L, -1, "ud");
            if (lua_islightuserdata(L, -1))
            {
                PHBezierPath * p = (PHBezierPath*)lua_touserdata(L, -1);
                p->retain();
                lua_pop(L,1);
                return p;
            }
            lua_pop(L,1);
        }
        
        PHBezierPath * p = new PHBezierPath;
        lua_pushvalue(L, -1);
        PHLuaSetHardRef(L, p);
        p->L = L;
        lua_pushlightuserdata(L, p);
        lua_setfield(L, -2, "ud");
        
        PHMessage::messageWithName("luaDestroy")->addListener(p, (PHCallback)&PHBezierPath::luaDestroy);
        p->beginCommitGrouping();
        
        lua_getfield(L, -1, "points");
        if (lua_istable(L, -1))
        {
            int n;
            PHLuaGetNumberField(n, "n");
            
            vector<anchorPoint> v;
            
            for (int i=0; i<n; i++)
            {
                lua_pushnumber(L, i);
                lua_gettable(L, -2);
                if (lua_istable(L, -1))
                {
                    anchorPoint a;
                    PHLuaGetNumberField(a.tag, "tag");
                    PHLuaGetPointField(a.point, "point");
                    v.push_back(a);
                }
                lua_pop(L,1);
            }
            
            p->insertAnchorPoints(v.begin(), v.end(), 0);
        }
        lua_pop(L,1);
        
        lua_getfield(L, -1, "curves");
        if (lua_istable(L, -1))
        {
            int n;
            PHLuaGetNumberField(n, "n");
            
            for (int i=0; i<n; i++)
            {
                lua_pushnumber(L, i);
                lua_gettable(L, -2);
                if (lua_istable(L, -1))
                    p->addCurve(PHRange::fromLua(L, -1));
                lua_pop(L,1);
            }
        }
        lua_pop(L,1);
        
        p->endCommitGrouping();
        return p;
    }
    return NULL;
}
    
void PHBezierPath::luaDestroy(PHObject * sender, void * ud)
{
    if ((lua_State*)ud == L)
        L = NULL;
}

PHBezierPath::~PHBezierPath()
{
    PHMessage::messageWithName("luaDestroy")->removeListener(this);
    if (L)
        PHLuaDeleteHardRef(L, this);
    if (cache)
        delete cache;
}

void PHBezierPath::addCurve(const PHRange & curve)
{
    curves.insert(curve);
    modelChanged();
}

void PHBezierPath::removeCurve(const PHRange & curve)
{
    curves.erase(curve);
    modelChanged();
}

void PHBezierPath::replaceAnchorPoint(const anchorPoint & pnt, int index)
{
    points[index] = pnt;
    modelChanged();
}

void PHBezierPath::insertOffsets(const PHRange & rng)
{
    set<PHRange> l;
    for (set<PHRange>::iterator i = curves.begin(); i!=curves.end(); i++)
    {
        PHRange r = *i;
        if (r.start>=rng.start)
            r.start+=rng.length;
        else
            if (r.start+r.length>rng.start)
                r.length+=rng.length;
        l.insert(r);
    }
    curves = l;
}

void PHBezierPath::removeOffsets(const PHRange & rng)
{
    set<PHRange> l;
    for (set<PHRange>::iterator i = curves.begin(); i!=curves.end(); i++)
    {
        PHRange r = *i;
        if (r.start>=rng.start && r.start+r.length<=rng.start+rng.length)
            continue;
        if (r.start>=rng.start+rng.length)
            r.start-=rng.length;
        else
        if (r.start>=rng.start)
        {
            r.length = r.start+r.length-(rng.start+rng.length);
            r.start = rng.start;
        }
        else
        if (r.start+r.length>rng.start)
            r.length-=min<int>(r.start+r.length-rng.start,rng.length);
        if (r.length<3) continue;
        l.insert(r);
    }
    curves = l;
}

void PHBezierPath::insertAnchorPoints(const anchorPoint * ap, int nr,int index)
{
    points.insert(points.begin()+index, ap, ap+nr);
    insertOffsets(PHRange(index,nr));
    modelChanged();
}

void PHBezierPath::insertAnchorPoints(vector<anchorPoint>::iterator first, vector<anchorPoint>::iterator last, int index)
{
    points.insert(points.begin()+index, first, last);
    insertOffsets(PHRange(index, (int)(last-first)));
    modelChanged();
}

void PHBezierPath::removeAnchorPoints(const PHRange & range)
{
    points.erase(points.begin()+range.start, points.begin()+range.start+range.length);
    removeOffsets(range);
    modelChanged();
}

void PHBezierPath::_modelChanged()
{
    delete cache;
    cache = NULL;
}

bool PHBezierPath::operator == (const PHBezierPath & othr)
{
    if (othr.points.size() != points.size()) return false;
    if (othr.curves.size() != curves.size()) return false;
    return  equal(points.begin(),points.end(),othr.points.begin())&&
            equal(curves.begin(),curves.end(),othr.curves.begin());
}

GLfloat * PHBezierPath::vertexData(size_t & nvertices, const PHRect & texCoord)
{
    const vector<anchorPoint> * a = tesselate(calculatedVertices());
    GLfloat * r = vertexDataFromAnchorList(*a, texCoord, nvertices);
    delete a;
    return r;
}

const vector<PHBezierPath::anchorPoint> & PHBezierPath::calculatedVertices()
{
    if (!cache)
    {
        cache = new vector<anchorPoint>;
        set<PHRange>::iterator j = curves.begin();
        unsigned int i = 0;
        PHRange rng;
        unsigned int n = (unsigned int)points.size();
        set<PHRange>::iterator e = curves.end();
        if (!curves.empty() && ((rng = *(--e)),rng.start+rng.length>n))
            i+=rng.start+rng.length-n-1;
        while (i<n)
        {
            while ((i<n)&&(j==curves.end() || i<j->start))
                cache->push_back(points[i++]);
            if (j!=curves.end() && i==j->start)
            {
                vector<anchorPoint> pnt;
                if (j->start+j->length>n)
                {
                    pnt.insert(pnt.end(), points.begin()+i,points.end());
                    pnt.insert(pnt.end(), points.begin(),points.begin()+(j->start+j->length)-n);
                } else
                    pnt.insert(pnt.end(), points.begin()+i,points.begin()+(j->start+j->length));
                vector<anchorPoint> * bp = bezierPath(pnt);
                cache->insert(cache->end(),bp->begin(),bp->end());
                delete bp;
                i+=j->length-1;
                j++;
            }
        }
    }
    return *cache;
}

vector<PHBezierPath::anchorPoint> * PHBezierPath::bezierPath(vector<anchorPoint> & v)
{
    vector<anchorPoint> * r = new vector<anchorPoint>;
    int count = (int)v.size()*3;
    for (int i=0; i<count; i++)
    {
        if (i==0)
            r->push_back(v[0]);
        else
        {
            vector<PHPoint> vv;
            int n = (int)v.size();
            for (int j = 0; j<n; j++)
                vv.push_back(v[j].point);
            ph_float ammount = (1.0f/count)*i;
            while (n>1)
            {
                for (int i=0; i<n-1; i++)
                    vv[i]=vv[i]*(1-ammount)+vv[i+1]*ammount;
                n--;
            }
            anchorPoint pnt;
            pnt.point = vv[0];
            pnt.tag = -1;
            r->push_back(pnt);
        }
    }
    return r;
}

void PHBezierPath::setBezierCurves(const set<PHRange> & c)
{
    curves = c;
    modelChanged();
}

void PHBezierPath::setAnchorPoints(const vector<anchorPoint> & c)
{
    points = c;
    modelChanged();
}
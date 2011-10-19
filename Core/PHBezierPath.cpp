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

PHBezierPath * PHBezierPath::fromLua(lua_State * L)
{
    if (lua_istable(L, -1))
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
}

void PHBezierPath::beginCommitGrouping()
{
    commitDisableCount++;
}

void PHBezierPath::endCommitGrouping()
{
    if (commitDisableCount>0)
        commitDisableCount--;
    if (!commitDisableCount)
        modelChanged();
}

void PHBezierPath::modelChanged()
{
    if (commitDisableCount) return;
    delete cache;
    cache = NULL;
    for (multimap<PHObject *, PHInvocation>::iterator i = callbacks.begin(); i!=callbacks.end(); i++)
        i->second.call();
}

const vector<PHBezierPath::anchorPoint> * PHBezierPath::tesselate(const vector<anchorPoint> & points)
{
    //dummy implementation
    vector<PHBezierPath::anchorPoint> * l = new vector<PHBezierPath::anchorPoint>(points);
    return l;
}


#pragma mark Tesselation
//----------------------

struct pnt {
    vector<PHBezierPath::anchorPoint>::const_iterator i;
    int p;
    list<pnt*> diagonals;
    pnt * next;
    pnt * prev;
    int type;
    
    enum tp {
        normal = 0,
        merge = 1,
        split = 2
    };
};

struct edge {
    pnt * st, * en;
    int type;
    pnt * helper;
    
    enum tp {
        enter = 0,
        exit = 1
    };
};

class binarySearchTree {
//lăbăreală implementation
    class cmp: public binary_function<edge *, edge *, bool>
    {
    public:
        static bool mode;
        bool operator () (edge * b, edge * a) const
        {
            if (mode)
            {
                PHPoint b1 = b->st->i->point;
                PHPoint b2 = b->en->i->point;
                PHPoint p = a->st->i->point;
                PHLog("cmp: (%d,%d) (%d,%d), [%lf,%lf] [%lf,%lf] [%lf,%lf]",a->st->p,a->en->p,b->st->p,b->en->p,b1.x,b1.y,b2.x,b2.y,p.x,p.y);
                if (b1.x==b2.x) return p>b1;
                if (p.x<b1.x) return false;
                if (b2.x<p.x) return true;
                return p.y>b1.y+(b2.y-b1.y)*(p.x-b1.x)/(b2.x-b1.x);
            }
            else
                return a->st->i->point.x<b->st->i->point.x;
        }
    };
    set<edge*,binarySearchTree::cmp> edges;
public:
    bool addEdge(edge * e)
    {
        PHLog("add edge:(%d %d)",e->st->p,e->en->p);
        return edges.insert(e).second;
    }
    
    bool removeEdge(edge * e)
    {
        PHLog("add edge:(%d %d)",e->st->p,e->en->p);
        return edges.erase(e)!=0;
    }
    
    edge * firstEdgeYLessThanPoint(pnt * p)
    {
        edge e;
        e.st = e.en = p;
        cmp::mode = true;
        set<edge*,binarySearchTree::cmp>::iterator it = edges.lower_bound(&e);
       // set<edge*,binarySearchTree::cmp>::iterator it = edges.begin();
       // binarySearchTree::cmp c;
       // while (it!=edges.end() && !(c(&e,*it)))
       //     it++;
        cmp::mode = false;
        if (it==edges.begin() || it==edges.end()) return NULL;
        PHLog("it before:(%d %d)",(*it)->st->p,(*it)->en->p);
        return *it;
    }
};

bool binarySearchTree::cmp::mode = false;

static bool cmp(pnt * a, pnt * b)
{
    return (a->i->point) < (b->i->point);
}

GLushort * PHBezierPath::triangulate(const vector<anchorPoint> & points, int & n)
{
    int m = points.size();
    
    pnt * a = new pnt[m];
    edge * b = new edge[m];
    pnt ** aa = new pnt * [m];
    vector<PHBezierPath::anchorPoint>::const_iterator it = points.begin();
    binarySearchTree bst;
    for (int i=0; i<m; i++)
    {
        a[i].p = i;
        a[i].i = it++;
        a[i].prev = i?(&a[i-1]):(&a[m-1]);
        a[i].next = i<m-1?&a[i+1]:&a[0];
        a[i].type = pnt::normal;
        b[i].st = &a[i];
        b[i].en = a[i].next;
        b[i].type = edge::enter;
        aa[i] = &a[i];
    }     
    for (int i=0; i<m; i++)
    {
        edge * edg = &b[i];
        PHPoint sp = edg->st->i->point;
        PHPoint ep = edg->en->i->point; 
        if (sp>ep) {
            pnt * tmp = edg->st;
            edg->st = edg->en;
            edg->en = tmp;
        }
        edg->helper = edg->st;
    }
    sort(aa,aa+m,cmp);
    
    int ndiag = 0;
    for (int i=0; i<m; i++)
    {
        pnt * crr = aa[i];
        PHLog("crr: %d",crr->p);
        pnt * pv = crr->prev;
        pnt * nx = crr->next;
        double cx = crr->i->point.x;
        double pvx = pv->i->point.x;
        double nxx = nx->i->point.x;
        
        #define edgenamed(x,y) ((x->p==y->p-1)?&b[x->p]:((x->p-1==y->p)?&b[y->p]:(((x->p==0 && y->p==m-1)||(x->p==m-1 && y->p==0))?&b[m-1]:NULL)))
        edge * nxe = edgenamed(crr, nx);
        edge * pve = edgenamed(crr, pv);
        
#define remedge(x) \
        { \
            edge * edg = (x); \
            if (bst.removeEdge(edg)) \
                if (edg->helper->type == pnt::merge) \
                {   \
                    PHPoint sp = edg->st->i->point; \
                    PHPoint ep = edg->en->i->point; \
                    adddiag(edg->helper,sp>ep?edg->st:edg->en); \
                } \
        }
        
#define adddiag(x,y) \
        { \
            pnt * aa = (x); \
            pnt * bb = (y); \
            if (aa!=bb) {\
                aa->diagonals.push_back(bb); \
                ndiag++; \
            } \
        }
        
        
        if (nxx<cx)
            remedge(nxe);
        if (pvx<cx)
            remedge(pve);
        
        edge * left = bst.firstEdgeYLessThanPoint(crr);
        if (!left)
            PHLog("left edge: NULL");
        else
            PHLog("left edge:(%d %d)",left->st->p,left->en->p);
        bool outside = (!left || left->type == edge::exit);
        
        if (nxx<=cx && pvx<=cx)
        {
            bool inv = (pv->i->point.y>nx->i->point.y) ^ outside;
            if (nxx<cx && inv)
                nxe->type = edge::exit;
            
            if (pvx<cx && !inv)
                pve->type = edge::exit;
            
            if (!outside)
                crr->type = pnt::merge;
        }
        if (nxx>=cx && pvx>=cx)
        {
            bool inv = (pv->i->point.y>nx->i->point.y) ^ outside;
            if (nxx>cx && inv)
                nxe->type = edge::exit;
            
            if (pvx>cx && !inv)
                pve->type = edge::exit;
            
            if (!outside)
            {
                crr->type = pnt::split;
                adddiag(crr,left->helper);
            }
        }
        
        if (!outside)
        {
            if (left->helper->type == pnt::merge)
                adddiag(left->helper,crr);
            left->helper = crr;
        }
        
        if (nxx>=cx)
            bst.addEdge(nxe);
        if (pvx>=cx)
            bst.addEdge(pve);
        
    }
    
    n = ndiag*2+m*2;
    GLushort * v = new GLushort[n];
    for (int i=0; i<m; i++)
    {
        v[i<<1] = i;
        v[(i<<1)+1] = (i==m-1)?0:i+1;
    }
    GLushort * p = v+m*2;
    for (int i=0; i<m; i++)
        for (list<pnt*>::iterator j = a[i].diagonals.begin(); j!=a[i].diagonals.end(); j++)
        {
            *(p++) = i;
            *(p++) = (*j)->p;
        }
    
    delete[] a;
    delete[] b;
    delete[] aa;
    
    return v;
}

const vector<PHBezierPath::anchorPoint> & PHBezierPath::calculatedVertices()
{
    if (!cache)
    {
        cache = new vector<anchorPoint>;
        set<PHRange>::iterator j = curves.begin();
        int i = 0;
        PHRange rng;
        int n = (int)points.size();
        set<PHRange>::iterator e = curves.end();
        if (!curves.empty() && ((rng = *(--e)),rng.start+rng.length>n))
            i+=rng.start+rng.length-n;
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
                    pnt.insert(pnt.end(), points.begin(),points.begin()+n-(j->start+j->length));
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
            double ammount = (1.0f/count)*i;
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
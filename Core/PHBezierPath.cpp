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

bool PHBezierPath::operator == (const PHBezierPath & othr)
{
    if (othr.points.size() != points.size()) return false;
    if (othr.curves.size() != curves.size()) return false;
    return  equal(points.begin(),points.end(),othr.points.begin())&&
            equal(curves.begin(),curves.end(),othr.curves.begin());
}

const vector<PHBezierPath::anchorPoint> * PHBezierPath::tesselate(const vector<anchorPoint> & points)
{
    //dummy implementation
    vector<PHBezierPath::anchorPoint> * l = new vector<PHBezierPath::anchorPoint>(points);
    return l;
}

#pragma mark Triangulation
//----------------------

struct pnt {
    vector<PHBezierPath::anchorPoint>::const_iterator i;
    int p;
    set<pnt*> diagonals;
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
        bool operator () (edge * a, edge * b) const
        {
            if (mode && a->st == a->en)
            {
                PHPoint b1 = b->st->i->point;
                PHPoint b2 = b->en->i->point;
                PHPoint p = a->st->i->point;
                if (b1.x==b2.x) 
                {
                    if (b1.x==p.x)
                        return p.y>b1.y;
                    return false;
                }
                if (p.x<b1.x) return false;
                if (b2.x<p.x) return true;
                return p.y>b1.y+(b2.y-b1.y)*(p.x-b1.x)/(b2.x-b1.x);
            }
            else
            {
                if (a->st->i->point.y == b->st->i->point.y)
                    return a->en->i->point.y > b->en->i->point.y;
                return a->st->i->point.y > b->st->i->point.y;
            }
        }
    };
    set<edge*,binarySearchTree::cmp> edges;
public:
    bool addEdge(edge * e)
    {
        return edges.insert(e).second;
    } 
    
    bool removeEdge(edge * e)
    {
        return edges.erase(e)!=0;
    }
    
    edge * firstEdgeYLessThanPoint(pnt * p)
    {
        edge e;
        e.st = e.en = p;
        cmp::mode = true;
        set<edge*,binarySearchTree::cmp>::iterator it = edges.upper_bound(&e);
        cmp::mode = false;
        if (it==edges.end()) return NULL; 
        return *it;
    }
};

bool binarySearchTree::cmp::mode = false;

static bool cmp(pnt * a, pnt * b)
{
    return (a->i->point) < (b->i->point);
}

static inline int angleof(const PHPoint & a, const PHPoint & b, const PHPoint & c)
{
    double determinant = a.x*b.y-a.y*b.x + b.x*c.y-b.y*c.x + c.x*a.y-c.y*a.x;
    if (determinant==0)
        return 0;
    if (determinant>0)
        return 1;
    return -1;
}

static int PHBezierPath_triangulateMonotone(pnt ** v, int n, GLushort * out)
{
    GLushort * oout = out;
    int * a = new int[n];
    int min = 0;
    for (int i=1; i<n; i++)
        if (v[i]->i->point<v[min]->i->point)
            min = i;
#define next(i) (((i)==n-1)?0:(i)+1)
#define prev(i) ((i)?(i)-1:n-1)
    int up = prev(min);
    int down = next(min);
    int order = angleof(v[up]->i->point,v[min]->i->point,v[down]->i->point);
    
    bool isup = v[up]->i->point<v[down]->i->point;
    int stn = 2;
    a[0] = min;
    a[1] = isup?up:down;
    if (isup)
    {
        up=prev(up);
        if (up==down) 
            up = -1;
    } else {
        down=next(down);
        if (up==down)
            down = -1;
    }
    
    for (int i=2; i<n; i++)
    {
        int cp = -1;
        if (up==-1)
            cp = down;
        else
        if (down==-1)
            cp = up;
        else
        if (v[down]->i->point.x<v[up]->i->point.x)
            cp = down;
        else
            cp = up;
        
        if (cp==down)
        {
            int d = next(down);
            if (v[d]->i->point.x<v[down]->i->point.x)
                down = -1;
            else
                down = d;
        }
        else
        if (cp==up)
        {
            int d = prev(up);
            if (v[d]->i->point.x<v[up]->i->point.x)
                up = -1;
            else
                up = d;
        }
        
#define triangle(a,b,c) \
        { \
            int aa = (a); \
            int bb = (b); \
            int cc = (c); \
            if (angleof(v[aa]->i->point,v[bb]->i->point,v[cc]->i->point)<0) \
            { \
                cc = aa^cc; \
                aa = aa^cc; \
                cc = aa^cc; \
            } \
            (*(out++))=v[aa]->p; \
            (*(out++))=v[bb]->p; \
            (*(out++))=v[cc]->p; \
        }
        
        int stop = a[stn-1];
        bool nxcp = next(stop)==cp;
        bool pvcp = prev(stop)==cp;
        if (nxcp || pvcp)
        {
            int desiredorder;
            if (nxcp)
                desiredorder = order;
            else
                desiredorder = -order;
            while (stn>=2 && (angleof(v[a[stn-2]]->i->point,v[a[stn-1]]->i->point,v[cp]->i->point)!=-desiredorder))
            {
                triangle(cp,a[stn-1],a[stn-2]);
                stn--;
            }
        } else {
            while (stn>=2)
            {
                triangle(cp,a[stn-1],a[stn-2]);
                stn--;
            }
            a[0]=stop;
        }
        a[stn++] = cp;
    }
#undef prev
#undef next
    return (int)(out-oout);
}

GLushort * PHBezierPath::triangulate(const vector<anchorPoint> & points, int & n)
{
    int m = (int)points.size();
    
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
            if (aa!=bb && aa->next!=bb && aa->prev!=bb) {\
                aa->diagonals.insert(bb); \
                bb->diagonals.insert(aa); \
                ndiag++; \
            } \
        }
        
        
        if (nxx<cx)
            remedge(nxe);
        if (pvx<cx)
            remedge(pve);
        
        edge * left = bst.firstEdgeYLessThanPoint(crr);
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
    
    int ntriangles = (m-2);
    n = ntriangles*6;
    GLushort * v = new GLushort[n];
    GLushort * cp = v;
    
    aa[0] = &a[0];
    aa[1] = &a[1];
    int sn = 2;
    a[m-1].diagonals.insert(&a[0]);
    for (int i=2; i<m; i++)
    {
        pnt * crr = &a[i];
        aa[sn++] = crr;
        set<pnt*>::iterator it = crr->diagonals.begin();
        while (it!=crr->diagonals.end() && (*it)->p<crr->p)
        {
            int j = sn-1;
            while (j>=0 && aa[j]->p!=(*it)->p)
                j--;
            if (j>=0)
            {
                pnt * first = aa[j];
                cp+=PHBezierPath_triangulateMonotone(aa+j, sn-j, cp);
                aa[j] = first;
                aa[j+1] = crr;
                sn = j+2;
            }
            crr->diagonals.erase(it);
            it = crr->diagonals.begin();
        }
    }
    
    n = (int)(cp-v);
    
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
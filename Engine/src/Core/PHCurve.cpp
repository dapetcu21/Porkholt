/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */
//  BEWARE!!! THIS IS THE UGLIEST FILE IN THIS WHOLE PROJECT

#include <Porkholt/Core/PHCurve.h>

void PHCurve::modelChanged()
{
    if (commitDisableCount) return;
    _modelChanged();
    for (multimap<PHObject *, PHInvocation>::iterator i = callbacks.begin(); i!=callbacks.end(); i++)
        i->second.call();
}

void PHCurve::beginCommitGrouping()
{
    commitDisableCount++;
}

void PHCurve::endCommitGrouping()
{
    if (commitDisableCount>0)
        commitDisableCount--;
    if (!commitDisableCount)
        modelChanged();
}

GLfloat * PHCurve::vertexDataFromAnchorList(const vector<anchorPoint> & anchors, const PHRect & texCoord, size_t & nvertices)
{
    size_t n = nvertices = anchors.size();
    GLfloat * v = new GLfloat[n*4];
    for (size_t i=0; i<n; i++)
    {
        v[(i<<2)] = anchors[i].point.x;
        v[(i<<2)+1] = anchors[i].point.y;
    }
    textureCoordinatesFromAnchorList(v+2,sizeof(GLfloat)*4,anchors,texCoord);
    return v;
}

void PHCurve::textureCoordinatesFromAnchorList(GLfloat * buffer, size_t stride, const vector<anchorPoint> & anchors, const PHRect & p)
{
    int i,nx;
    nx = i = 0;
    int corners[4] = {0,0,0,0};
    size_t n = anchors.size();
    if (!stride)
        stride = 2*sizeof(GLfloat);
    
    for (size_t i=0; i<n; i++)
        if (anchors[i].tag>0 && anchors[i].tag<=4)
            corners[(anchors[i].tag)-1] = i;
    
    for (int k=0; k<4; k++)
    {
        float len = 0;
        int tag;
        for (i = nx = corners[k];((tag = anchors[i].tag),(tag<=0 || tag>4 || tag==k+1)); i=nx)
        {
            nx++;
            if ((size_t)nx>=n)
                nx-=n;
            len+=(anchors[nx].point-anchors[i].point).length();
        }
        float l = 0;
        for (i = nx = corners[k];((tag = anchors[i].tag),(tag<=0 || tag>4 || tag==k+1)); i=nx)
        {
            nx++;
            if ((size_t)nx>=n)
                nx-=n;
            float d = l/len;
            float x,y;
            switch (k+1) {
                case 1:
                    y = 0;
                    x = d;
                    break;
                case 2:
                    x = 1;
                    y = d;
                    break;
                case 3:
                    x = 1-d;
                    y = 1;
                    break;
                case 4:
                    x = 0;
                    y = 1-d;
                    break;
                default:
                    x = 0;
                    y = 0;
                    break;
            }
            x = p.x+p.width*x;
            y = p.y+p.height*y;
            buffer[0] = x;
            buffer[1] = y;
            buffer = (GLfloat*)(((char*)buffer)+stride);
            l+=(anchors[nx].point-anchors[i].point).length();
        }
    }
}


#pragma mark Triangulation
//----------------------

struct pnt_raw {
    const PHPoint * i;
    const PHPoint * it () { return i; }
    int p;
    set<pnt_raw*> diagonals;
    pnt_raw * next;
    pnt_raw * prev;
    int type;
    
    enum tp {
        normal = 0,
        merge = 1,
        split = 2
    };
};

struct edge_raw {
    pnt_raw * st, * en;
    int type;
    pnt_raw * helper;
    
    enum tp {
        enter = 0,
        exit = 1
    };
};

class binarySearchTree_raw {
//lăbăreală implementation
public:
    class cmp: public binary_function<edge_raw *, edge_raw *, bool>
    {
    public:
        static bool mode;
        bool operator () (edge_raw * a, edge_raw * b) const
        {
            if (mode && a->st == a->en)
            {
                PHPoint b1 = *b->st->i;
                PHPoint b2 = *b->en->i;
                PHPoint p = *a->st->i;
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
                if (a->st->it()->y == b->st->it()->y)
                    return a->en->it()->y > b->en->it()->y;
                return a->st->it()->y > b->st->it()->y;
            }
        }
    };
    set<edge_raw*,binarySearchTree_raw::cmp> edges;
    bool addEdge(edge_raw * e)
    {
        return edges.insert(e).second;
    } 
    
    bool removeEdge(edge_raw * e)
    {
        return edges.erase(e)!=0;
    }
    
    edge_raw * firstEdgeYLessThanPoint(pnt_raw * p)
    {
        edge_raw e;
        e.st = e.en = p;
        cmp::mode = true;
        set<edge_raw*,binarySearchTree_raw::cmp>::iterator it = edges.upper_bound(&e);
        cmp::mode = false;
        if (it==edges.end()) return NULL; 
        return *it;
    }
};

bool binarySearchTree_raw::cmp::mode = false;

static bool cmp_raw(pnt_raw * a, pnt_raw * b)
{
    return (*a->i) < (*b->i);
}

static inline int angleof(const PHPoint & a, const PHPoint & b, const PHPoint & c)
{
    float determinant = a.x*b.y-a.y*b.x + b.x*c.y-b.y*c.x + c.x*a.y-c.y*a.x;
    if (determinant==0)
        return 0;
    if (determinant>0)
        return 1;
    return -1;
}

static int PHBezierPath_triangulateMonotone(pnt_raw ** v, int n, GLushort * out)
{
    GLushort * oout = out;
    int * a = new int[n];
    int min = 0;
    for (int i=1; i<n; i++)
        if (*v[i]->i<*v[min]->i)
            min = i;
#define next(i) (((i)==n-1)?0:(i)+1)
#define prev(i) ((i)?(i)-1:n-1)
    int up = prev(min);
    int down = next(min);
    int order = angleof(*v[up]->i,*v[min]->i,*v[down]->i);
    
    bool isup = *v[up]->i<*v[down]->i;
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
        if (v[down]->i->x<v[up]->i->x)
            cp = down;
        else
            cp = up;
        
        if (cp==down)
        {
            int d = next(down);
            if (v[d]->i->x<v[down]->i->x)
                down = -1;
            else
                down = d;
        }
        else
        if (cp==up)
        {
            int d = prev(up);
            if (v[d]->i->x<v[up]->i->x)
                up = -1;
            else
                up = d;
        }
        
#define triangle(a,b,c) \
        { \
            int aa = (a); \
            int bb = (b); \
            int cc = (c); \
            if (angleof(*v[aa]->i,*v[bb]->i,*v[cc]->i)<0) \
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
            while (stn>=2 && (angleof(*v[a[stn-2]]->i,*v[a[stn-1]]->i,*v[cp]->i)!=-desiredorder))
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

GLushort * PHCurve::triangulatePolygon(const GLfloat vertices[],size_t stride, size_t nvertices, size_t & n)
{
    int m = nvertices;
    
    pnt_raw * a = new pnt_raw[m];
    edge_raw * b = new edge_raw[m];
    pnt_raw ** aa = new pnt_raw * [m];
    const GLfloat * it = vertices;
    binarySearchTree_raw bst;
    for (int i=0; i<m; i++)
    {
        a[i].p = i;
        a[i].i = (const PHPoint*)it;
        it += stride;
        a[i].prev = i?(&a[i-1]):(&a[m-1]);
        a[i].next = i<m-1?&a[i+1]:&a[0];
        a[i].type = pnt_raw::normal;
        b[i].st = &a[i];
        b[i].en = a[i].next;
        b[i].type = edge_raw::enter;
        aa[i] = &a[i];
    }     
    for (int i=0; i<m; i++)
    {
        edge_raw * edg = &b[i];
        PHPoint sp = *edg->st->i;
        PHPoint ep = *edg->en->i;
        if (sp>ep) {
            pnt_raw * tmp = edg->st;
            edg->st = edg->en;
            edg->en = tmp;
        }
        edg->helper = edg->st;
    }
    sort(aa,aa+m,cmp_raw);
    
    int ndiag = 0;
    for (int i=0; i<m; i++)
    {
        pnt_raw * crr = aa[i];
        pnt_raw * pv = crr->prev;
        pnt_raw * nx = crr->next;
        float cx = crr->i->x;
        float pvx = pv->i->x;
        float nxx = nx->i->x;
        
        #define edgenamed(x,y) ((x->p==y->p-1)?&b[x->p]:((x->p-1==y->p)?&b[y->p]:(((x->p==0 && y->p==m-1)||(x->p==m-1 && y->p==0))?&b[m-1]:NULL)))
        edge_raw * nxe = edgenamed(crr, nx);
        edge_raw * pve = edgenamed(crr, pv);
        
#define remedge(x) \
        { \
            edge_raw * edg = (x); \
            if (bst.removeEdge(edg)) \
                if (edg->helper->type == pnt_raw::merge) \
                {   \
                    PHPoint sp = *edg->st->i; \
                    PHPoint ep = *edg->en->i; \
                    adddiag(edg->helper,sp>ep?edg->st:edg->en); \
                } \
        }
        
#define adddiag(x,y) \
        { \
            pnt_raw * aa = (x); \
            pnt_raw * bb = (y); \
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
        
        edge_raw * left = bst.firstEdgeYLessThanPoint(crr);
        bool outside = (!left || left->type == edge_raw::exit);
        
        if (nxx<=cx && pvx<=cx)
        {
            bool inv = (pv->i->y>nx->i->y) ^ outside;
            if (nxx<cx && inv)
                nxe->type = edge_raw::exit;
            
            if (pvx<cx && !inv)
                pve->type = edge_raw::exit;
            
            if (!outside)
                crr->type = pnt_raw::merge;
        }
        if (nxx>=cx && pvx>=cx)
        {
            bool inv = (pv->i->y>nx->i->y) ^ outside;
            if (nxx>cx && inv)
                nxe->type = edge_raw::exit;
            
            if (pvx>cx && !inv)
                pve->type = edge_raw::exit;
            
            if (!outside)
            {
                crr->type = pnt_raw::split;
                adddiag(crr,left->helper);
            }
        }
        
        if (!outside)
        {
            if (left->helper->type == pnt_raw::merge)
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
        pnt_raw * crr = &a[i];
        aa[sn++] = crr;
        set<pnt_raw*>::iterator it = crr->diagonals.begin();
        while (it!=crr->diagonals.end() && (*it)->p<crr->p)
        {
            int j = sn-1;
            while (j>=0 && aa[j]->p!=(*it)->p)
                j--;
            if (j>=0)
            {
                pnt_raw * first = aa[j];
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


//--------------------------

struct pnt {
    vector<PHCurve::anchorPoint>::const_iterator i;
    list<PHCurve::anchorPoint>::const_iterator il;
    const PHCurve::anchorPoint * it () { if (p>=0) return &*i; return &*il; }
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
public:
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
                if (a->st->it()->point.y == b->st->it()->point.y)
                    return a->en->it()->point.y > b->en->it()->point.y;
                return a->st->it()->point.y > b->st->it()->point.y;
            }
        }
    };
    set<edge*,binarySearchTree::cmp> edges;
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

const vector<PHCurve::anchorPoint> * PHCurve::tesselate(const vector<anchorPoint> & points)
{
    
    int m = (int)points.size();
    
    pnt * a = new pnt[m];
    edge * b = new edge[m];
    pnt ** aa = new pnt * [m];
    vector<PHCurve::anchorPoint>::const_iterator it = points.begin();
    
    list<pnt*> additions;
    list<PHCurve::anchorPoint> addp;
    
    binarySearchTree bst;
    for (int i=0; i<m; i++)
    {
        a[i].p = i;
        a[i].i = it++;
        a[i].prev = i?(&a[i-1]):(&a[m-1]);
        a[i].next = i<m-1?&a[i+1]:&a[0];
        b[i].st = &a[i];
        b[i].en = a[i].next;
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
    
    for (int i=0; i<m; i++)
    {
        pnt * crr = aa[i];
        pnt * pv = crr->prev;
        pnt * nx = crr->next;
        float cx = crr->it()->point.x;
        float pvx = pv->it()->point.x;
        float nxx = nx->it()->point.x;
        
        int j = crr->p;
        edge * nxe = &b[j];
        edge * pve = &b[j?(j-1):(m-1)];
        
        if (nxx<=cx)
            bst.removeEdge(nxe);
        if (pvx<=cx)
            bst.removeEdge(pve);
        
        set<edge*> rm;
        for (set<edge*,binarySearchTree::cmp>::iterator i = bst.edges.begin(); i!=bst.edges.end(); i++)
        {
            edge * e = *i;
            PHPoint p1 = e->st->it()->point;
            PHPoint p2 = e->en->it()->point;
            if (p1.x<=cx && p2.x<=cx)
            {
                rm.insert(e);
                continue;
            }
            if (p1.x==p2.x || p1.x==cx || p2.x==cx)
                continue;
            PHPoint p = PHPoint(cx,p1.y+(cx-p1.x)/(p2.x-p1.x)*(p2.y-p1.y));
            
            pnt * pt = new pnt;
            pt->p = -1;
            pt->il = addp.insert(addp.end(), anchorPoint(p, -1));
            pt->prev = (e->st->next==e->en)?e->st:((e->en->next==e->st)?e->en:NULL);
            pt->next = (e->st->next==e->en)?e->en:((e->en->next==e->st)?e->st:NULL);
            if (pt->prev) pt->prev->next = pt;
            if (pt->next) pt->next->prev = pt;
            additions.push_back(pt);
            
            if (p1.x<=cx)
                e->st = pt;
            else
                e->en = pt;
        }
        
        for (set<edge*>::iterator i = rm.begin(); i!=rm.end(); i++)
            bst.removeEdge(*i);
        rm.clear();
        
        if (nxx>cx)
            bst.addEdge(nxe);
        if (pvx>cx)
            bst.addEdge(pve);
        
    }
    
    vector<PHCurve::anchorPoint> * l = new vector<PHCurve::anchorPoint>;
    
    pnt * pp;
    pp = a;
    do {
        if (!pp) break;
        l->push_back(*(pp->it()));
        pp = pp->next;
    } while (a!=pp);
    
    delete[] a;
    delete[] b;
    delete[] aa;
    
    for (list<pnt*>::iterator i = additions.begin(); i!=additions.end(); i++)
        delete *i;
    
    return l;
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
        
#undef triangle
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

GLushort * PHCurve::triangulatePolygon(const vector<anchorPoint> & points, size_t & n)
{
    int m = (int)points.size();
    
    pnt * a = new pnt[m];
    edge * b = new edge[m];
    pnt ** aa = new pnt * [m];
    vector<PHCurve::anchorPoint>::const_iterator it = points.begin();
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
        float cx = crr->i->point.x;
        float pvx = pv->i->point.x;
        float nxx = nx->i->point.x;
        
#define edgenamed(x,y) ((x->p==y->p-1)?&b[x->p]:((x->p-1==y->p)?&b[y->p]:(((x->p==0 && y->p==m-1)||(x->p==m-1 && y->p==0))?&b[m-1]:NULL)))
        edge * nxe = edgenamed(crr, nx);
        edge * pve = edgenamed(crr, pv);
        
#undef remedge
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
       
#undef adddiag
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

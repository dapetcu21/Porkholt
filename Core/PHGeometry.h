/*
 *  PHGeometry.h
 *  Porkholt
 *
 *  Created by Marius Petcu on 2/9/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHGEOMETRY_H
#define PHGEOMETRY_H

#include "PHMain.h"

struct PHRect;
struct PHPoint
{
    union {
        double x;
        double width;
    };
    union {
        double y;
        double height;
    };
    PHPoint() {};
    PHPoint(double xx,double yy) : x(xx), y(yy) {};
    PHPoint(const PHPoint & o) : x(o.x), y(o.y) {};
    PHPoint(const PHRect & o);
    static PHPoint fromLua(lua_State * L, int index);
    PHPoint & operator += (const PHPoint & othr)
    {
        x+=othr.x;
        y+=othr.y;
        return *this;
    }
    PHPoint operator + (const PHPoint & o) const
    {
        PHPoint res(x+o.x,y+o.y);
        return res;
    }
    PHPoint & operator -= (const PHPoint & othr)
    {
        x-=othr.x;
        y-=othr.y;
        return *this;
    }
    PHPoint operator - (const PHPoint & o) const
    {
        PHPoint res(x-o.x,y-o.y);
        return res;
    }
    const PHPoint & operator *= (double d)
    {
        x*=d;
        y*=d;
        return * this;
    }
    PHPoint operator * (double d) const
    {
        PHPoint res(x*d,y*d);
        return res;
    }
    
    PHPoint operator * (const PHPoint & p) const
    {
        PHPoint res(x*p.x,y*p.y);
        return res;
    }
    
    PHPoint operator / (const PHPoint & p) const
    {
        PHPoint res(x/p.x,y/p.y);
        return res;
    }
    
    const PHPoint & operator /= (double d)
    {
        x/=d;
        y/=d;
        return * this;
    }
    PHPoint operator / (double d) const
    {
        PHPoint res(x/d,y/d);
        return res;
    }
    void rotate(double angle)
    {
        double ox=x, oy=y, sinv = sin(angle), cosv = cos(angle);
        x = cosv*ox-sinv*oy;
        y = sinv*ox+cosv*oy;
    }
    double length() { return sqrt(x*x+y*y); } 
    double squaredLength() { return x*x+y*y; }
    void normalize() { (*this)/=length(); }
    PHPoint rotated(double angle) const
    {
        PHPoint p;
        double sinv = sin(angle), cosv = cos(angle);
        p.x = cosv*x-sinv*y;
        p.y = sinv*x+cosv*y;
        return p;
    }
    
    bool operator < (const PHPoint & o) const
    {
        if (x==o.x) 
            return y<o.y;
        return x<o.x;
    }
    
    bool operator > (const PHPoint & o) const
    {
        if (x==o.x) 
            return y>o.y;
        return x>o.x;
    }
    
    bool operator == (const PHPoint & o) const
    {
        return (x==o.x && y==o.y);
    }
    
    void saveToLua(lua_State * L) const;
};

typedef PHPoint PHSize;
#define PHNullSize PHOriginPoint

struct PHRect
{
	PHRect(double _x, double _y, double w, double h) : x(_x),y(_y),width(w),height(h) {};
	PHRect(const PHRect & o) : x(o.x),y(o.y),width(o.width),height(o.height) {};
	PHRect() {};
	double x,y;
	double width,height;
    static PHRect fromLua(lua_State * L, int index);
    void saveToLua(lua_State * L) const;
    
    PHPoint origin() const { return PHPoint(x,y); }
    PHSize size() const { return PHSize(width,height); }
    PHPoint center() const { return PHPoint(x+width/2,y+height/2); }
    void setOrigin(const PHPoint & o) { x = o.x; y = o.y; }
    void setSize(const PHSize & o) { width = o.width; y = o.height; }
    void setCenter(const PHPoint & o) { x = o.x-width/2; y = o.y-height/2; }
    
    PHPoint corner(int index) const { return PHPoint(x+((index&1)?width:0),y+((index&2)?height:0)); }
    
    PHRect & operator += (const PHPoint & othr)
    {
        x+=othr.x;
        y+=othr.y;
        return *this;
    }
    PHRect operator + (const PHPoint & o) const
    {
        return PHRect(x+o.x,y+o.y,width,height);
    }
    PHRect & operator -= (const PHPoint & othr)
    {
        x-=othr.x;
        y-=othr.y;
        return *this;
    }
    PHRect operator - (const PHPoint & o) const
    {
        return PHRect(x-o.x,y-o.y,width,height);
    }
    const PHRect & operator *= (double d)
    {
        width*=d;
        height*=d;
        return * this;
    }
    PHRect operator * (double d) const
    {
        return PHRect(x,y,width*d,height*d);
    }
    const PHRect & operator /= (double d)
    {
        width/=d;
        height/=d;
        return * this;
    }
    PHRect operator / (double d) const
    {
        return PHRect(x,y,width/d,height/d);
    }
};

struct PHColor
{
	double r,g,b,a;
    bool operator == (const PHColor & o) const {
        return (r==o.r)&&(g==o.g)&&(b==o.b)&&(a==o.a);
    }
    bool operator != (const PHColor & o) const {
        return (r!=o.r)||(g!=o.g)||(b!=o.b)||(a!=o.a);
    }
    const PHColor & operator *= (double d)
    {
        if (a>0) a*=d;
        return * this;
    }
    PHColor operator * (double d) const
    {
        if (a<0) return *this;
        PHColor res(r,g,b,a*d);
        return res;
    }
    const PHColor & operator *= (const PHColor & d)
    {
        a*=d.a; r*=d.r; g*=d.g; b*=d.b;
        return * this;
    }
    PHColor operator * (const PHColor & d) const
    {
        PHColor res(r*d.r,g*d.g,b*d.b,a*d.a);
        return res;
    }
    PHColor(double red, double green, double blue, double alpha) : r(red), g(green), b(blue), a(alpha) {};
    PHColor(double red, double green, double blue) : r(red), g(green), b(blue), a(1.0f) {};
    PHColor() {};
    bool isValid() { return (r>=0 && r<=1 && g>=0 && g<=1 && b>=0 && b<=1 && a>=0 && a<=1); }
    static PHColor fromLua(lua_State * L, int index);
    void saveToLua(lua_State * L) const;
};

struct PH24BitColor
{
    uint8_t r,g,b,a;
    bool operator == (const PH24BitColor & o) const {
        return (r==o.r)&&(g==o.g)&&(b==o.b)&&(a==o.a);
    }
    bool operator != (const PH24BitColor & o) const {
        return (r!=o.r)||(g!=o.g)||(b!=o.b)||(a!=o.a);
    }
    PH24BitColor() {};
    PH24BitColor(const PHColor & o) : r(o.r*255), g(o.g*255), b(o.b*255), a(o.a*255) {}
};

struct PHRange
{
    //represents [start, length)
    unsigned int start, length;
    PHRange() : start(0), length(0) {}
    PHRange(int s, int l) : start(s), length(l) {}
    PHRange(const PHRange & o) : start(o.start), length(o.length) {}
    int end() const { return start+length; }
        
    static int wrap(int index, int n) {
        if (index<0)
            index+=((-index)/n + 1)*n;
        return index%n;
    }
    
    bool intersects(int index) const
    {
        return (index>=start && index<start+length);
    }
    
    bool intersects(const PHRange & range) const
    {
        return intersects(range.start) || intersects(range.end()-1) || range.intersects(start) || range.intersects(end()-1);
    }
    
    bool intersectsCircularily(int index, int n) const
    {
        if (intersects(n))
            return PHRange(start,n-start).intersects(index) || PHRange(0,wrap(end(),n)).intersects(index);
        return intersects(index);
    }
    
    bool intersectsCircularily(const PHRange & range, int n) const
    {
        if (intersects(n))
            return PHRange(start,n-start).intersects(range) || PHRange(0,wrap(end(),n)).intersects(range);
        return intersects(range);
    }
    
    PHRange & operator += (const int offset)
    {
        start+=offset;
        return *this;
    }
    
    PHRange operator + (const int offset) const
    {
        return PHRange(start+offset,length);
    }
    
    bool operator < (const PHRange & o) const
    {
        if (start == o.start) return length<o.length;
        return start<o.start;
    }
    
    bool operator <= (const PHRange & o) const
    {
        if (start == o.start) return length<=o.length;
        return start<=o.start;
    }
    
    bool operator > (const PHRange & o) const
    {
        if (start == o.start) return length>o.length;
        return start>o.start;
    }
    
    bool operator >= (const PHRange & o) const
    {
        if (start == o.start) return length>=o.length;
        return start>=o.start;
    }
    
    bool operator == (const PHRange & o) const
    {
        return (o.start == start && o.length == length);
    }
    
    static PHRange fromLua(lua_State * L, int index);
    void saveToLua(lua_State * L) const;
};

extern const PHColor PHClearColor;
extern const PHColor PHBlackColor;
extern const PHColor PHGrayColor;
extern const PHColor PHWhiteColor;
extern const PHColor PHInvalidColor;

extern const PHRect PHWholeRect;
extern const PHRect PHInvalidRect;
extern const PHRect PHNullRect;

extern const PHRange PHInvalidRange;

extern const PHSize PHUnitSize;
extern const PHPoint PHOriginPoint;

void PHInvertMatrix(const GLfloat * m, GLfloat * inverse);
PHPoint PHTransformPointMatrix(const GLfloat * m,const PHPoint & pnt);
PHPoint PHUnTransformPointMatrix(const GLfloat * m, const PHPoint & pnt);
PHPoint PHTransformedPoint(const PHPoint & pnt);
PHPoint PHUnTransformedPoint(const PHPoint & pnt);
bool PHPointInRect(const PHPoint & pnt, const PHRect & rect);
bool PHRectIntersectsRect(const PHRect & r1, const PHRect & r2);
bool PHPointInCircle(const PHPoint & pnt, const PHPoint & origin, double radius);

void PHLowPassFilter(double & var, double newval, double period, double cutoff);

#define toRad(x) ((x)/180.0f*M_PI)
#define toDeg(x) ((x)/M_PI*180.0f)
inline double PHWarp(double v, double f)
{
    if (v<0)
        v-=((int)(v/f)-1)*f;
    if (v>=f)
        v-=((int)(v/f))*f;
    return v;
}

void PHGLRotate(double angle);
void PHGLFlip(PHPoint center, bool horiz, bool vert);
enum PHGLStates
{
    PHGLVertexArray = 1<<0,
    PHGLColorArray = 1<<1,
    PHGLTextureCoordArray = 1<<2,
    PHGLTexture = 1<<3
};
void PHGLSetStates(int states);

extern PHColor PHGLCurrentColor;

inline void PHGLSetColor(PHColor clr)
{
    if (clr.a<0)
        clr = PHWhiteColor;
    if (clr != PHGLCurrentColor)
    {
        PHGLCurrentColor = clr;
        PH24BitColor t(clr);
        glColor4ub(t.r,t.g,t.b,t.a);
    }
}

#endif
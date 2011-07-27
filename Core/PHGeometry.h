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

struct PHRect
{
	PHRect(double _x, double _y, double w, double h) : x(_x),y(_y),width(w),height(h) {};
	PHRect(const PHRect & o) : x(o.x),y(o.y),width(o.width),height(o.height) {};
	PHRect() {};
	double x,y;
	double width,height;
    static PHRect fromLua(lua_State * L, int index);
    void saveToLua(lua_State * L) const;
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
    PHColor() {};
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


inline PHColor PHMakeColor(double red, double green, double blue, double alpha)
{
	PHColor tmp;
	tmp.r = red;
	tmp.g = green;
	tmp.b = blue;
	tmp.a = alpha;
	return tmp;
}

inline PHColor PHMakeColor(double red, double green, double blue)
{
	PHColor tmp;
	tmp.r = red;
	tmp.g = green;
	tmp.b = blue;
	tmp.a = 1.0f;
	return tmp;
}

extern PHColor PHClearColor;
extern PHColor PHBlackColor;
extern PHColor PHGrayColor;
extern PHColor PHWhiteColor;
extern PHColor PHInvalidColor;

extern PHRect PHWholeRect;

inline PHRect PHMakeRect(double x, double y, double width, double height)
{
	PHRect tmp;
	tmp.x = x;
	tmp.y= y;
	tmp.width = width;
	tmp.height = height;
	return tmp;
}

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
    void saveToLua(lua_State * L) const;
};

typedef PHPoint PHSize;
#define PHMakeSize PHMakePoint
#define PHNullSize PHOriginPoint

extern PHPoint PHOriginPoint;

inline PHPoint PHMakePoint(double x, double y)
{
	PHPoint tmp;
	tmp.x = x;
	tmp.y = y;
	return tmp;
}

void PHInvertMatrix(const GLfloat * m, GLfloat * inverse);
PHPoint PHTransformPointMatrix(const GLfloat * m,const PHPoint & pnt);
PHPoint PHUnTransformPointMatrix(const GLfloat * m, const PHPoint & pnt);
PHPoint PHTransformedPoint(const PHPoint & pnt);
PHPoint PHUnTransformedPoint(const PHPoint & pnt);
bool PHPointInRect(const PHPoint & pnt, const PHRect & rect);


void PHLowPassFilter(double & var, double newval, double period, double cutoff);
struct b2Vec2;
void b2RotatePoint(b2Vec2 & p, double angle, b2Vec2 around);

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

#endif
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
//	PHRect(double _x, double _y, double w, double h) : x(_x),y(_y),width(w),height(h) {};
//	PHRect(const PHRect & o) : x(o.x),y(o.y),width(o.width),height(o.height) {};
//	PHRect() : x(0), y(0), width(0), height(0) {};
	double x,y;
	double width,height;
    static PHRect rectFromLua(lua_State * L, int index);
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
    static PHColor colorFromLua(lua_State * L, int index);
    void saveToLua(lua_State * L) const;
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
	double x,y;
    PHPoint() {};
    PHPoint(double xx,double yy) : x(xx), y(yy) {};
    PHPoint(const PHPoint & o) : x(o.x), y(o.y) {};
    static PHPoint pointFromLua(lua_State * L, int index);
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
        angle = -angle/180 * M_PI;
        double ox=x, oy=y, sinv = sin(angle), cosv = cos(angle);
        x = cosv*ox-sinv*oy;
        y = sinv*ox+cosv*oy;
    }
    PHPoint rotated(double angle) const
    {
        angle = -angle/180 * M_PI;
        PHPoint p;
        double sinv = sin(angle), cosv = cos(angle);
        p.x = cosv*x-sinv*y;
        p.y = sinv*x+cosv*y;
        return p;
    }
    void saveToLua(lua_State * L) const;
};

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

#endif
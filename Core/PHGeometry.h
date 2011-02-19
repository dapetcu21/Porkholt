/*
 *  PHGeometry.h
 *  Porkholt
 *
 *  Created by Marius Petcu on 2/9/11.
 *  Copyright 2011 Home. All rights reserved.
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
};

struct PHColor
{
	double r,g,b,a;
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

inline PHColor PHClearColor()
{
	PHColor tmp;
	tmp.r = 0.0f;
	tmp.g = 0.0f;
	tmp.b = 0.0f;
	tmp.a = 0.0f;
	return tmp;
}
inline PHColor PHBlackColor()
{
	PHColor tmp;
	tmp.r = 0.0f;
	tmp.g = 0.0f;
	tmp.b = 0.0f;
	tmp.a = 1.0f;
	return tmp;
}
inline PHColor PHWhiteColor()
{
	PHColor tmp;
	tmp.r = 1.0f;
	tmp.g = 1.0f;
	tmp.b = 1.0f;
	tmp.a = 1.0f;
	return tmp;
}
inline PHColor PHGrayColor()
{
	PHColor tmp;
	tmp.r = 0.5f;
	tmp.g = 0.5f;
	tmp.b = 0.5f;
	tmp.a = 1.0f;
	return tmp;
}

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
};

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

#endif
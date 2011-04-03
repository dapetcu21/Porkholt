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

void PHLowPassFilter(double & var, double newval, double period, double cutoff);

#endif
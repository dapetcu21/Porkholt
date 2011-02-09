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

struct PHRect
{
	double x,y;
	double width,height;
};

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

#endif
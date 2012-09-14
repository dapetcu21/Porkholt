
/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHLua.h>


PHColor PHGLCurrentColor(-1,-1,-1,-1);

void PHLowPassFilter(ph_float & var, ph_float newval, ph_float period, ph_float cutoff)
{
	ph_float RC=1.0/cutoff;
	ph_float alpha=period/(period+RC);
	var = newval * alpha + var * (1.0 - alpha);
}

void PHLowPassFilter(double & var, double newval, double period, double cutoff)
{
	double RC=1.0/cutoff;
	double alpha=period/(period+RC);
	var = newval * alpha + var * (1.0 - alpha);
}

void PHLowPassFilter(ph_float * var, ph_float newval, ph_float period, ph_float cutoff)
{
	ph_float RC=1.0/cutoff;
	ph_float alpha=period/(period+RC);
	(*var) = newval * alpha + (*var) * (1.0 - alpha);
}

void PHLowPassFilter(double * var, double newval, double period, double cutoff)
{
	double RC=1.0/cutoff;
	double alpha=period/(period+RC);
	(*var) = newval * alpha + (*var) * (1.0 - alpha);
}

bool PHPointInRect(const PHPoint & pnt, const PHRect & rect)
{
    if (pnt.x<=rect.x || pnt.x>=rect.x+rect.width) return false;
    if (pnt.y<=rect.y || pnt.y>=rect.y+rect.height) return false;
    return true;
}

bool PHPointInCircle(const PHPoint & pnt, const PHPoint & origin, ph_float radius)
{
    return (pnt-origin).length()<=radius;
}

bool PHRectIntersectsRect(const PHRect & r1, const PHRect & r2)
{
    return (PHPointInRect(r1.corner(0), r2)||
            PHPointInRect(r1.corner(1), r2)||
            PHPointInRect(r1.corner(2), r2)||
            PHPointInRect(r1.corner(3), r2)||
            PHPointInRect(r2.corner(0), r1)||
            PHPointInRect(r2.corner(1), r1)||
            PHPointInRect(r2.corner(2), r1)||
            PHPointInRect(r2.corner(3), r1));
}

ph_float PHAngleFromNormalizedVector(const PHPoint & vec)
{
    ph_float ang;
    if (abs(vec.x)<0.5)
    {
        ang = acos(vec.x);
        if (vec.y<0)
            ang = M_PI*2-ang;
    } else {
        ang = asin(vec.y);
        if (vec.x<0)
            ang = M_PI-ang;
    }
    if (ang>=M_PI*2)
        ang-=M_PI*2;
    if (ang<0)
        ang+=M_PI*2;
    return ang;
}

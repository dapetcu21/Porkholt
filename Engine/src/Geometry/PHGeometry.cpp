
/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHLua.h>


PHColor PHGLCurrentColor(-1,-1,-1,-1);

void PHLowPassFilter(float & var, float newval, float period, float cutoff)
{
	float RC=1.0/cutoff;
	float alpha=period/(period+RC);
	var = newval * alpha + var * (1.0 - alpha);
}

void PHLowPassFilter(double & var, double newval, double period, double cutoff)
{
	double RC=1.0/cutoff;
	double alpha=period/(period+RC);
	var = newval * alpha + var * (1.0 - alpha);
}

void PHLowPassFilter(float * var, float newval, float period, float cutoff)
{
	float RC=1.0/cutoff;
	float alpha=period/(period+RC);
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

bool PHPointInCircle(const PHPoint & pnt, const PHPoint & origin, float radius)
{
    return (pnt-origin).length()<=radius;
}

#define pil(x, a, b) ((x>=a) && (x<=b))
#define lil(x, y, a, b) (pil(x, a, b) || pil(y, a, b) || pil(a, x, y) || pil(b, x, y))
bool PHRectIntersectsRect(const PHRect & r1, const PHRect & r2)
{
    float xb1 = r1.x + r1.width;
    float xb2 = r2.x + r2.width;
    float yb1 = r1.y + r1.height;
    float yb2 = r2.y + r2.height;
    return lil(r1.x, xb1, r2.x, xb2) && lil(r1.y, yb1, r2.y, yb2);
}

float PHAngleFromNormalizedVector(const PHPoint & vec)
{
    float ang;
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

#define threehalfs 1.5f

float PHInvSqrt(float number)
{
    union {
        float f;
        int32_t i;
    } v = { .f = number };

    float half = number * 0.5f;
	v.i = 0x5f3759df - ( v.i >> 1 );
	v.f  = v.f * ( threehalfs - ( half * v.f * v.f ) );
	return v.f;
}

float PHSqrt(float number)
{
    union {
        float f;
        int32_t i;
    } v = { .f = number };

    float half = number * 0.5f;
	v.i = 0x5f3759df - ( v.i >> 1 );
	v.f  = v.f * ( threehalfs - ( half * v.f * v.f ) );
	return v.f * number;
}

#define threehalfs_d 1.5f

double PHInvSqrt(double number)
{
    union {
        double f;
        int64_t i;
    } v = { .f = number };

    double half = number * 0.5f;
	v.i = 0x5fe6eb50c7b537a9 - ( v.i >> 1 );
	v.f  = v.f * ( threehalfs_d - ( half * v.f * v.f ) );
	return v.f;
}

double PHSqrt(double number)
{
    union {
        double f;
        int64_t i;
    } v = { .f = number };

    double half = number * 0.5f;
	v.i = 0x5fe6eb50c7b537a9 - ( v.i >> 1 );
	v.f  = v.f * ( threehalfs_d - ( half * v.f * v.f ) );
	return v.f * number;
}


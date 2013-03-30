/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGEOMETRY_H
#define PHGEOMETRY_H

#include <Porkholt/Core/PHUtilities.h>

#if defined(__ARM_NEON__)
    #include "math_neon.h"
    #define PH_MATRIX_NEON
#elif defined(__ARM_VFPV2__)
    #include "matrix_impl.h"
    #define PH_MATRIX_VFP
#else
    #define PH_MATRIX_C
#endif

float PHInvSqrt(float);
float PHSqrt(float);
double PHSqrt(double);
double PHInvSqrt(double);

#include <Porkholt/Geometry/PHPoint.h>
#include <Porkholt/Geometry/PHRect.h>
#include <Porkholt/Geometry/PHColor.h>
#include <Porkholt/Geometry/PHRange.h>
#include <Porkholt/Geometry/PHMatrix.h>
#include <Porkholt/Geometry/PHQuaternion.h>

bool PHPointInRect(const PHPoint & pnt, const PHRect & rect);
bool PHRectIntersectsRect(const PHRect & r1, const PHRect & r2);
bool PHPointInCircle(const PHPoint & pnt, const PHPoint & origin, float radius);

#define toRad(x) ((x)/180.0f*M_PI)
#define toDeg(x) ((x)/M_PI*180.0f)

void PHLowPassFilter(float & var, float newval, float period, float cutoff);
void PHLowPassFilter(double & var, double newval, double period, double cutoff);
void PHLowPassFilter(float * var, float newval, float period, float cutoff);
void PHLowPassFilter(double * var, double newval, double period, double cutoff);

float PHAngleFromNormalizedVector(const PHPoint & vec);
inline float PHAngleFromVector(PHPoint vec)
{
    vec.normalize();
    return PHAngleFromNormalizedVector(vec);
}

inline float PHWarp(float v, float f)
{
    if (v<0)
        v-=((int)(v/f)-1)*f;
    if (v>=f)
        v-=((int)(v/f))*f;
    return v;
}

inline float PHClamp(float v, float l, float h)
{
    if (v < l)
        v = l;
    if (v > h)
        v = h;
    return v;
}

inline float PHNormalizeAngle(float v)
{
    if (v>M_PI)
        v-=((int)(v+M_PI)/(M_PI*2))*(M_PI*2);
    if (v<-M_PI)
        v-=((int)(v-M_PI)/(M_PI*2))*(M_PI*2);
    return v;
}
#endif

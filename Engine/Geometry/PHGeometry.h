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

#if !defined(__i386__) && defined(__arm__)
//#   ifdef __ARM_NEON__
//#       include "math_neon.h"
//#       define PH_MATRIX_NEON
//#   else
//#       include "matrix_impl.h"
//#       define PH_MATRIX_VFP
#           define PH_MATRIX_C
//#   endif
#else
#   define PH_MATRIX_C
#endif

#include "PHPoint.h"
#include "PHRect.h"
#include "PHColor.h"
#include "PHRange.h"
#include "PHMatrix.h"

bool PHPointInRect(const PHPoint & pnt, const PHRect & rect);
bool PHRectIntersectsRect(const PHRect & r1, const PHRect & r2);
bool PHPointInCircle(const PHPoint & pnt, const PHPoint & origin, ph_float radius);

#define toRad(x) ((x)/180.0f*M_PI)
#define toDeg(x) ((x)/M_PI*180.0f)

void PHLowPassFilter(ph_float & var, ph_float newval, ph_float period, ph_float cutoff);
ph_float PHAngleFromNormalizedVector(const PHPoint & vec);
inline ph_float PHAngleFromVector(PHPoint vec)
{
    vec.normalize();
    return PHAngleFromNormalizedVector(vec);
}

inline ph_float PHWarp(ph_float v, ph_float f)
{
    if (v<0)
        v-=((int)(v/f)-1)*f;
    if (v>=f)
        v-=((int)(v/f))*f;
    return v;
}

#endif
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

//move this to PHGameManager

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

enum PHGLCapabilities
{
    PHGLCapabilityNPOT = 0,
    PHGLCapabilityAppleLimitedNPOT,
    PHGLCapabilityOpenGLES,
    PHGLNumberCapabilities
};

bool PHGLHasCapability(int cap);

#endif
//
//  PHMatrix.cpp
//  Porkholt
//
//  Created by Marius Petcu on 2/1/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHMain.h"
#include "PHMatrix.h"

void PHInvertMatrix(const GLfloat * m, GLfloat * inverse)
{
	ph_float a0 = m[ 0]*m[ 5] - m[ 1]*m[ 4];
    ph_float a1 = m[ 0]*m[ 6] - m[ 2]*m[ 4];
    ph_float a2 = m[ 0]*m[ 7] - m[ 3]*m[ 4];
    ph_float a3 = m[ 1]*m[ 6] - m[ 2]*m[ 5];
    ph_float a4 = m[ 1]*m[ 7] - m[ 3]*m[ 5];
    ph_float a5 = m[ 2]*m[ 7] - m[ 3]*m[ 6];
    ph_float b0 = m[ 8]*m[13] - m[ 9]*m[12];
    ph_float b1 = m[ 8]*m[14] - m[10]*m[12];
    ph_float b2 = m[ 8]*m[15] - m[11]*m[12];
    ph_float b3 = m[ 9]*m[14] - m[10]*m[13];
    ph_float b4 = m[ 9]*m[15] - m[11]*m[13];
    ph_float b5 = m[10]*m[15] - m[11]*m[14];
	
    ph_float det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
	
	inverse[ 0] = + m[ 5]*b5 - m[ 6]*b4 + m[ 7]*b3;
	inverse[ 4] = - m[ 4]*b5 + m[ 6]*b2 - m[ 7]*b1;
	inverse[ 8] = + m[ 4]*b4 - m[ 5]*b2 + m[ 7]*b0;
	inverse[12] = - m[ 4]*b3 + m[ 5]*b1 - m[ 6]*b0;
	inverse[ 1] = - m[ 1]*b5 + m[ 2]*b4 - m[ 3]*b3;
	inverse[ 5] = + m[ 0]*b5 - m[ 2]*b2 + m[ 3]*b1;
	inverse[ 9] = - m[ 0]*b4 + m[ 1]*b2 - m[ 3]*b0;
	inverse[13] = + m[ 0]*b3 - m[ 1]*b1 + m[ 2]*b0;
	inverse[ 2] = + m[13]*a5 - m[14]*a4 + m[15]*a3;
	inverse[ 6] = - m[12]*a5 + m[14]*a2 - m[15]*a1;
	inverse[10] = + m[12]*a4 - m[13]*a2 + m[15]*a0;
	inverse[14] = - m[12]*a3 + m[13]*a1 - m[14]*a0;
	inverse[ 3] = - m[ 9]*a5 + m[10]*a4 - m[11]*a3;
	inverse[ 7] = + m[ 8]*a5 - m[10]*a2 + m[11]*a1;
	inverse[11] = - m[ 8]*a4 + m[ 9]*a2 - m[11]*a0;
	inverse[15] = + m[ 8]*a3 - m[ 9]*a1 + m[10]*a0;
	
	ph_float invDet = (1.0f)/det;
	inverse[ 0] *= invDet;
	inverse[ 1] *= invDet;
	inverse[ 2] *= invDet;
	inverse[ 3] *= invDet;
	inverse[ 4] *= invDet;
	inverse[ 5] *= invDet;
	inverse[ 6] *= invDet;
	inverse[ 7] *= invDet;
	inverse[ 8] *= invDet;
	inverse[ 9] *= invDet;
	inverse[10] *= invDet;
	inverse[11] *= invDet;
	inverse[12] *= invDet;
	inverse[13] *= invDet;
	inverse[14] *= invDet;
	inverse[15] *= invDet;	
}

PHPoint PHTransformPointMatrix(const GLfloat * m,const PHPoint & pnt)
{
	ph_float x,y,w;
	x = pnt.x * m[0] + pnt.y * m[4] /*+ 0 * m[8] */ + 1 * m[12];
	y = pnt.x * m[1] + pnt.y * m[5] /*+ 0 * m[9] */ + 1 * m[13];
	w = pnt.x * m[3] + pnt.y * m[7] /*+ 0 * m[11]*/ + 1 * m[15];        
	
	PHPoint npnt;
	npnt.x = x/w;
	npnt.y = y/w;
	return npnt;
}

PHPoint PHUnTransformPointMatrix(const GLfloat * m, const PHPoint & pnt)
{
	GLfloat inverted[16];
	PHInvertMatrix(m, inverted);
	//return PHTransformPointMatrix(inverted, pnt);
	ph_float x,y,w;
	x = pnt.x * inverted[0] + pnt.y * inverted[4] /*+ 0 * inverted[8] */ + 1 * inverted[12];
	y = pnt.x * inverted[1] + pnt.y * inverted[5] /*+ 0 * inverted[9] */ + 1 * inverted[13];
	w = pnt.x * inverted[3] + pnt.y * inverted[7] /*+ 0 * inverted[11]*/ + 1 * inverted[15];        
	
	PHPoint npnt;
	npnt.x = x/w;
	npnt.y = y/w;
	return npnt;
}

PHPoint PHTransformedPoint(const PHPoint & pnt)
{
	GLfloat m[16]; 
	glGetFloatv(GL_MODELVIEW, m);
	return PHTransformPointMatrix(m, pnt);
}

PHPoint PHUnTransformedPoint(const PHPoint & pnt)
{
	GLfloat m[16]; 
	glGetFloatv(GL_MODELVIEW, m);
	return PHUnTransformPointMatrix(m, pnt);
}

void PHGLRotate(ph_float angle)
{
    ph_float sinv = sin(angle), cosv = cos(angle);
    GLfloat m[16] = {
        cosv, sinv, 0, 0,
        -sinv,  cosv, 0, 0,
        0,     0, 1, 0,
        0,     0, 0, 1
    };
    glMultMatrixf(m);
}

void PHGLFlip(PHPoint center, bool horiz, bool vert)
{
    if (!horiz && !vert) return;
    ph_float px = horiz?-1:1;
    ph_float py = vert?-1:1;
    GLfloat m[16] = {
        px                    , 0                    , 0, 0,
        0                     , py                   , 0, 0,
        0                     , 0                    , 1, 0,
        -center.x*px+center.x , -center.y*py+center.y, 0, 1
    };
    glMultMatrixf(m);
}

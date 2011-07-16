/*
 *  PHGeometry.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHGeometry.h"
#include "PHLua.h"
#include <Box2D/Box2D.h>

PHPoint PHOriginPoint(0,0);
PHRect PHWholeRect = {0,0,1,1};
PHColor PHClearColor = {0,0,0,0};
PHColor PHBlackColor = {0,0,0,1};
PHColor PHWhiteColor = {1,1,1,1};
PHColor PHGrayColor = {0.5,0.5,0.5,1};
PHColor PHInvalidColor = {-1,-1,-1,-1};

PHColor PHColor::colorFromLua(lua_State * L, int index)
{
    if (!lua_istable(L, index)) return PHInvalidColor;
    PHColor color = PHWhiteColor;

    lua_getfield(L, index, "r");
    if (lua_isnumber(L, -1))
        color.r = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "g");
    if (lua_isnumber(L, -1))
        color.g = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "b");
    if (lua_isnumber(L, -1))
        color.b = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "a");
    lua_gettable(L, -2);
    if (lua_isnumber(L, -1))
        color.a = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return color;
}

PHPoint PHPoint::pointFromLua(lua_State * L, int index)
{
    if (!lua_istable(L, index)) return PHOriginPoint;
    PHPoint pnt = PHOriginPoint;
    
    lua_getfield(L, index, "x");
    if (lua_isnumber(L, -1))
        pnt.x = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "y");
    if (lua_isnumber(L, -1))
        pnt.y = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    return pnt;
}

PHRect PHRect::rectFromLua(lua_State * L, int index)
{
    if (!lua_istable(L, index)) return PHWholeRect;
    PHRect pnt = PHWholeRect;
    
    lua_getfield(L, index, "x");
    if (lua_isnumber(L, -1))
        pnt.x = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "y");
    if (lua_isnumber(L, -1))
        pnt.y = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "height");
    if (lua_isnumber(L, -1))
        pnt.height = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "width");
    if (lua_isnumber(L, -1))
        pnt.width = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    return pnt;
}

void PHPoint::saveToLua(lua_State * L) const
{
    lua_newtable(L);    
    lua_pushnumber(L, x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, y);
    lua_setfield(L, -2, "y");
}
void PHRect::saveToLua(lua_State * L) const
{
    lua_newtable(L);    
    lua_pushnumber(L, x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, width);
    lua_setfield(L, -2, "width");
    lua_pushnumber(L, height);
    lua_setfield(L, -2, "height");
}
void PHColor::saveToLua(lua_State * L) const
{
    lua_newtable(L);
    lua_pushnumber(L, r);
    lua_setfield(L, -2, "r");
    lua_pushnumber(L, g);
    lua_setfield(L, -2, "g");
    lua_pushnumber(L, b);
    lua_setfield(L, -2, "b");
    lua_pushnumber(L, a);
    lua_setfield(L, -2, "a");
}

void PHInvertMatrix(const GLfloat * m, GLfloat * inverse)
{
	double a0 = m[ 0]*m[ 5] - m[ 1]*m[ 4];
    double a1 = m[ 0]*m[ 6] - m[ 2]*m[ 4];
    double a2 = m[ 0]*m[ 7] - m[ 3]*m[ 4];
    double a3 = m[ 1]*m[ 6] - m[ 2]*m[ 5];
    double a4 = m[ 1]*m[ 7] - m[ 3]*m[ 5];
    double a5 = m[ 2]*m[ 7] - m[ 3]*m[ 6];
    double b0 = m[ 8]*m[13] - m[ 9]*m[12];
    double b1 = m[ 8]*m[14] - m[10]*m[12];
    double b2 = m[ 8]*m[15] - m[11]*m[12];
    double b3 = m[ 9]*m[14] - m[10]*m[13];
    double b4 = m[ 9]*m[15] - m[11]*m[13];
    double b5 = m[10]*m[15] - m[11]*m[14];
	
    double det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
	
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
	
	double invDet = (1.0f)/det;
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
	double x,y,w;
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
	double x,y,w;
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

void PHLowPassFilter(double & var, double newval, double period, double cutoff)
{
	double RC=1.0/cutoff;
	double alpha=period/(period+RC);
	//alpha = 0.000005;
	var = newval * alpha + var * (1.0 - alpha);
}

void b2RotatePoint(b2Vec2 & p, double angle, b2Vec2 around)
{
    p = p-around;
    b2Vec2 pp = p;
    pp.x = cos(angle)*p.x-sin(angle)*p.y;
    pp.y = sin(angle)*p.x+cos(angle)*p.y;
    p = pp;
    p = p+around;
}

bool PHPointInRect(const PHPoint & pnt, const PHRect & rect)
{
    if (pnt.x<rect.x || pnt.x>rect.x+rect.width) return false;
    if (pnt.y<rect.y || pnt.y>rect.y+rect.height) return false;
    return true;
}

void PHGLRotate(double angle)
{
    double sinv = sin(angle), cosv = cos(angle);
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
    double px = horiz?-1:1;
    double py = vert?-1:1;
    GLfloat m[16] = {
        px                    , 0                    , 0, 0,
        0                     , py                   , 0, 0,
        0                     , 0                    , 1, 0,
        -center.x*px+center.x , -center.y*py+center.y, 0, 1
    };
    glMultMatrixf(m);
}
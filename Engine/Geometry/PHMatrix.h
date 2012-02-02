//
//  PHMatrix.h
//  Porkholt
//
//  Created by Marius Petcu on 2/1/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHMATRIX_H
#define PHMATRIX_H

#include "PHMain.h"
#include "PHPoint.h"

class PHMatrix
{
private:
    GLfloat m[16];
    
public:
    PHMatrix() 
    {}
    
    PHMatrix(const GLfloat * mx)
    {
        setToMatrix(mx);
    }
    
    PHMatrix(const PHMatrix & mx)
    {
        setToMatrix(mx);
    }
    
    PHMatrix(GLfloat a0 , GLfloat a1 , GLfloat a2 , GLfloat a3 , 
             GLfloat a4 , GLfloat a5 , GLfloat a6 , GLfloat a7 , 
             GLfloat a8 , GLfloat a9 , GLfloat a10, GLfloat a11, 
             GLfloat a12, GLfloat a13, GLfloat a14, GLfloat a15)
    {
        m[0]  = a0 ; m[1]  = a1 ; m[2]  = a2 ; m[3] =  a3 ;
        m[4]  = a1 ; m[5]  = a5 ; m[6]  = a6 ; m[7] =  a7 ;
        m[8]  = a1 ; m[9]  = a9 ; m[10] = a10; m[11] = a11;
        m[12] = a1 ; m[13] = a13; m[14] = a14; m[15] = a15;
    }
    
    static PHMatrix identity() { PHMatrix m; m.setToIdentity(); return m; }
    void setToIdentity()
    {
        m[ 0] = 1; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
        m[ 4] = 0; m[ 5] = 1; m[ 6] = 0; m[ 7] = 0;
        m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;
        m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
    }
    
    static PHMatrix rotation(ph_float angle) { PHMatrix m; m.setToZRotation(angle); return m; }
    void rotate(ph_float angle) { zRotate(angle); }
    void preRotate(ph_float angle) { preZRotate(angle); }
    void setToRotation(ph_float angle) { setToZRotation(angle); }
    
    static PHMatrix zRotation(ph_float angle) { PHMatrix m; m.setToZRotation(angle); return m;}
    void zRotate(ph_float angle) { PHMatrix m; m.setToZRotation(angle); (*this)=(*this)*m; }
    void preZRotate(ph_float angle) { PHMatrix m; m.setToZRotation(angle); (*this)=m*(*this); }
    void setToZRotation(ph_float angle)
    {
        ph_float s = sin(angle), c = cos(angle);
        m[ 0] = c; m[ 1] = s; m[ 2] = 0; m[ 3] = 0;
        m[ 4] =-s; m[ 5] = c; m[ 6] = 0; m[ 7] = 0;
        m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;
        m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
    }

    static PHMatrix scaling(const PH3DSize & sz) { PHMatrix m; m.setToScaling(sz); return m;}
    void scale(const PH3DSize & sz) { PHMatrix m; m.setToScaling(sz); (*this)=(*this)*m; }
    void preScale(const PH3DSize & sz) { PHMatrix m; m.setToScaling(sz); (*this)=m*(*this); }
    void setToScaling(const PH3DSize & sz)
    {
        m[ 0] = sz.x; m[ 1] = 0;    m[ 2] = 0;    m[ 3] = 0;
        m[ 4] = 0;    m[ 5] = sz.y; m[ 6] = 0;    m[ 7] = 0;
        m[ 8] = 0;    m[ 9] = 0;    m[10] = sz.z; m[11] = 0;
        m[12] = 0;    m[13] = 0;    m[14] = 0;    m[15] = 1;
    }
    
    static PHMatrix scaling(const PHSize & sz) { PHMatrix m; m.setToScaling(sz); return m;}
    void scale(const PHSize & sz) { PHMatrix m; m.setToScaling(sz); (*this)=(*this)*m; }
    void preScale(const PHSize & sz) { PHMatrix m; m.setToScaling(sz); (*this)=m*(*this); }
    void setToScaling(const PHSize & sz)
    {
        m[ 0] = sz.x; m[ 1] = 0;    m[ 2] = 0;    m[ 3] = 0;
        m[ 4] = 0;    m[ 5] = sz.y; m[ 6] = 0;    m[ 7] = 0;
        m[ 8] = 0;    m[ 9] = 0;    m[10] = 1;    m[11] = 0;
        m[12] = 0;    m[13] = 0;    m[14] = 0;    m[15] = 1;
    }
    
    static PHMatrix translation(const PHPoint & p) { PHMatrix m; m.setToTranslation(p); return m; };
    void translate(const PHPoint & p) { PHMatrix m; m.setToTranslation(p); (*this)=(*this)*m; }
    void preTranslate(const PHPoint & p) { m[12]+=p.x; m[13]+=p.y; }
    void setToTranslation(const PHPoint & p)
    {
        m[ 0] = 1;   m[ 1] = 0;   m[ 2] = 0;   m[ 3] = 0;
        m[ 4] = 0;   m[ 5] = 1;   m[ 6] = 0;   m[ 7] = 0;
        m[ 8] = 0;   m[ 9] = 0;   m[10] = 1;   m[11] = 0;
        m[12] = p.x; m[13] = p.y; m[14] = 0;   m[15] = 1;
    }
    
    static PHMatrix translation(const PH3DPoint & p) { PHMatrix m; m.setToTranslation(p); return m; };
    void translate(const PH3DPoint & p) { PHMatrix m; m.setToTranslation(p); (*this)=(*this)*m; }
    void preTranslate(const PH3DPoint & p) { m[12]+=p.x; m[13]+=p.y; m[14]+=p.z; }
    void setToTranslation(const PH3DPoint & p)
    {
        m[ 0] = 1;   m[ 1] = 0;   m[ 2] = 0;   m[ 3] = 0;
        m[ 4] = 0;   m[ 5] = 1;   m[ 6] = 0;   m[ 7] = 0;
        m[ 8] = 0;   m[ 9] = 0;   m[10] = 1;   m[11] = 0;
        m[12] = p.x; m[13] = p.y; m[14] = p.z; m[15] = 1;
    }
    
    static PHMatrix flipping(const PH3DPoint & origin, bool flipX, bool flipY, bool flipZ) { PHMatrix m; m.setToFlipping(origin,flipX,flipY,flipZ); return m; };
    void translate(const PH3DPoint & origin, bool flipX, bool flipY, bool flipZ) { PHMatrix m; m.setToFlipping(origin,flipX,flipY,flipZ); (*this)=(*this)*m; }
    void preScale(const PH3DPoint & origin, bool flipX, bool flipY, bool flipZ) { PHMatrix m; m.setToFlipping(origin,flipX,flipY,flipZ); (*this)=m*(*this); }
    void setToFlipping(const PH3DPoint & o, bool flipX, bool flipY, bool flipZ)
    {
        ph_float px = flipX?2:0;
        ph_float py = flipY?2:0;
        ph_float pz = flipZ?2:0;
        m[ 0] = px;     m[ 1] = 0;      m[ 2] = 0;      m[ 3] = 0;
        m[ 4] = 0;      m[ 5] = py;     m[ 6] = 0;      m[ 7] = 0;
        m[ 8] = 0;      m[ 9] = 0;      m[10] = pz;     m[11] = 0;
        m[12] = o.x*px; m[13] = o.y*py; m[14] = o.z*pz; m[15] = 1;
    }
    
    void setToMatrix(const PHMatrix & mx)
    {
        setToMatrix(mx.m);
    }
        
    void setToMatrix(const GLfloat * mx)
    {
        memcpy(m, mx, sizeof(GLfloat)*16);
    }
    
    const GLfloat * floats() const { return m; } 
    GLfloat * floats() { return m; } 
    
    PHMatrix & operator *= (const  PHMatrix & o) { (*this) = (*this) * o; return (*this); }
    
    PHMatrix inverse() const;
    PHMatrix operator * (const PHMatrix & b) const;
    PH3DPoint transformPoint(const PH3DPoint & p) const;
    PH3DPoint untransformPoint(const PH3DPoint & p) const { return inverse().transformPoint(p); }
    PHPoint transformPoint(const PHPoint & p) const;
    PHPoint untransformPoint(const PHPoint & p) const { return inverse().transformPoint(p); }
};

extern const PHMatrix PHIdentityMatrix;

#endif

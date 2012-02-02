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
        loadMatrix(mx);
    }
    
    PHMatrix(const PHMatrix & mx)
    {
        loadMatrix(mx);
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
    
    static PHMatrix identity() { PHMatrix m; m.loadIdentity(); return m; }
    void loadIdentity()
    {
        m[ 0] = 1; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
        m[ 4] = 0; m[ 5] = 1; m[ 6] = 0; m[ 7] = 0;
        m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;
        m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
    }
    
    static PHMatrix rotation(ph_float angle) { PHMatrix m; m.loadZRotation(angle); return m; }
    void rotate(ph_float angle) { zRotate(angle); }
    void preRotate(ph_float angle) { preZRotate(angle); }
    void loadRotation(ph_float angle) { loadZRotation(angle); }
    
    static PHMatrix zRotation(ph_float angle) { PHMatrix m; m.loadZRotation(angle); return m;}
    void zRotate(ph_float angle) { PHMatrix m; m.loadZRotation(angle); (*this)=(*this)*m; }
    void preZRotate(ph_float angle) { PHMatrix m; m.loadZRotation(angle); (*this)=m*(*this); }
    void loadZRotation(ph_float angle)
    {
        ph_float s = sin(angle), c = cos(angle);
        m[ 0] = c; m[ 1] = s; m[ 2] = 0; m[ 3] = 0;
        m[ 4] =-s; m[ 5] = c; m[ 6] = 0; m[ 7] = 0;
        m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;
        m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
    }

    static PHMatrix scaling(const PH3DSize & sz) { PHMatrix m; m.loadScaling(sz.x,sz.y,sz.z); return m;}
    void scale(const PH3DSize & sz) { PHMatrix m; m.loadScaling(sz.x,sz.y,sz.z); (*this)=(*this)*m; }
    void preScale(const PH3DSize & sz) { PHMatrix m; m.loadScaling(sz.x,sz.y,sz.z); (*this)=m*(*this); }
    void loadScaling(const PH3DSize & sz) { loadScaling(sz.x,sz.y,sz.z); }
    
    static PHMatrix scaling(const PHSize & sz) { PHMatrix m; m.loadScaling(sz.x,sz.y); return m;}
    void scale(const PHSize & sz) { PHMatrix m; m.loadScaling(sz.x,sz.y); (*this)=(*this)*m; }
    void preScale(const PHSize & sz) { PHMatrix m; m.loadScaling(sz.x,sz.y); (*this)=m*(*this); }
    void loadScaling(const PHSize & sz) { loadScaling(sz.x,sz.y); }
    
    static PHMatrix scaling(GLfloat x, GLfloat y, GLfloat z) { PHMatrix m; m.loadScaling(x,y,z); return m;}
    void scale(GLfloat x, GLfloat y, GLfloat z) { PHMatrix m; m.loadScaling(x,y,z); (*this)=(*this)*m; }
    void preScale(GLfloat x, GLfloat y, GLfloat z) { PHMatrix m; m.loadScaling(x,y,z); (*this)=m*(*this); }
    void loadScaling(GLfloat x, GLfloat y, GLfloat z)
    {
        m[ 0] = x; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
        m[ 4] = 0; m[ 5] = y; m[ 6] = 0; m[ 7] = 0;
        m[ 8] = 0; m[ 9] = 0; m[10] = z; m[11] = 0;
        m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
    }
    
    static PHMatrix scaling(GLfloat x, GLfloat y) { PHMatrix m; m.loadScaling(x,y); return m;}
    void scale(GLfloat x, GLfloat y) { PHMatrix m; m.loadScaling(x,y); (*this)=(*this)*m; }
    void preScale(GLfloat x, GLfloat y) { PHMatrix m; m.loadScaling(x,y); (*this)=m*(*this); }
    void loadScaling(GLfloat x, GLfloat y)
    {
        m[ 0] = x; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
        m[ 4] = 0; m[ 5] = y; m[ 6] = 0; m[ 7] = 0;
        m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;
        m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
    }
    
    static PHMatrix translation(const PHPoint & p) { PHMatrix m; m.loadTranslation(p.x,p.y); return m; };
    void translate(const PHPoint & p) { PHMatrix m; m.loadTranslation(p.x,p.y); (*this)=(*this)*m; }
    void preTranslate(const PHPoint & p) { m[12]+=p.x; m[13]+=p.y; }
    void loadTranslation(const PHPoint & p) { loadTranslation(p.x,p.y); }
    
    static PHMatrix translation(const PH3DPoint & p) { PHMatrix m; m.loadTranslation(p.x,p.y,p.z); return m; };
    void translate(const PH3DPoint & p) { PHMatrix m; m.loadTranslation(p.x,p.y,p.z); (*this)=(*this)*m; }
    void preTranslate(const PH3DPoint & p) { m[12]+=p.x; m[13]+=p.y; m[14]+=p.z; }
    void loadTranslation(const PH3DPoint & p) { loadTranslation(p.x,p.y,p.z); }
    
    static PHMatrix translation(GLfloat x, GLfloat y) { PHMatrix m; m.loadTranslation(x,y); return m; };
    void translate(GLfloat x, GLfloat y) { PHMatrix m; m.loadTranslation(x,y); (*this)=(*this)*m; }
    void preTranslate(GLfloat x, GLfloat y) { m[12]+=x; m[13]+=y; }
    void loadTranslation(GLfloat x, GLfloat y)
    {
        m[ 0] = 1; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
        m[ 4] = 0; m[ 5] = 1; m[ 6] = 0; m[ 7] = 0;
        m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;
        m[12] = x; m[13] = y; m[14] = 0; m[15] = 1;
    }
    
    static PHMatrix translation(GLfloat x, GLfloat y, GLfloat z) { PHMatrix m; m.loadTranslation(x,y,z); return m; };
    void translate(GLfloat x, GLfloat y, GLfloat z) { PHMatrix m; m.loadTranslation(x,y,z); (*this)=(*this)*m; }
    void preTranslate(GLfloat x, GLfloat y, GLfloat z) { m[12]+=x; m[13]+=y; m[14]+=z; }
    void loadTranslation(GLfloat x, GLfloat y, GLfloat z)
    {
        m[ 0] = 1; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
        m[ 4] = 0; m[ 5] = 1; m[ 6] = 0; m[ 7] = 0;
        m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;
        m[12] = x; m[13] = y; m[14] = z; m[15] = 1;
    }
    
    static PHMatrix flipping(const PH3DPoint & origin, bool flipX, bool flipY, bool flipZ) { PHMatrix m; m.loadFlipping(origin,flipX,flipY,flipZ); return m; };
    void translate(const PH3DPoint & origin, bool flipX, bool flipY, bool flipZ) { PHMatrix m; m.loadFlipping(origin,flipX,flipY,flipZ); (*this)=(*this)*m; }
    void preScale(const PH3DPoint & origin, bool flipX, bool flipY, bool flipZ) { PHMatrix m; m.loadFlipping(origin,flipX,flipY,flipZ); (*this)=m*(*this); }
    void loadFlipping(const PH3DPoint & o, bool flipX, bool flipY, bool flipZ)
    {
        m[ 0] = flipX?-1:1; m[ 1] = 0;          m[ 2] = 0;          m[ 3] = 0;
        m[ 4] = 0;          m[ 5] = flipY?-1:1; m[ 6] = 0;          m[ 7] = 0;
        m[ 8] = 0;          m[ 9] = 0;          m[10] = flipZ?-1:1; m[11] = 0;
        m[12] = flipX?o.x*2:0; 
        m[13] = flipY?o.y*2:0; 
        m[14] = flipZ?o.z*2:0; m[15] = 1;
    }
    
    void loadMatrix(const PHMatrix & mx)
    {
        loadMatrix(mx.m);
    }
        
    void loadMatrix(const GLfloat * mx)
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

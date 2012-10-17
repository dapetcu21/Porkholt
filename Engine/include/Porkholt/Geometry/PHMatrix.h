/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHMATRIX_H
#define PHMATRIX_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Geometry/PHPoint.h>


class PHMatrix;
PHPoint PHTransformPointMatrix(const GLfloat * PH_RESTRICT m, const PHPoint & pnt);
PH3DPoint PHTransformPointMatrix(const GLfloat * PH_RESTRICT m, const PH3DPoint & pnt);
void PHMultiplyMatrix(const GLfloat m0[16], const GLfloat m1[16], GLfloat d[16]);
void PHInvertMatrix(const GLfloat * PH_RESTRICT m, GLfloat * PH_RESTRICT inverse);


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
        m[4]  = a4 ; m[5]  = a5 ; m[6]  = a6 ; m[7] =  a7 ;
        m[8]  = a8 ; m[9]  = a9 ; m[10] = a10; m[11] = a11;
        m[12] = a12 ; m[13] = a13; m[14] = a14; m[15] = a15;
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
    
    static PHMatrix perspective(double fovy, double aspect, double zNear, double zFar) { PHMatrix m; m.loadPerspective(fovy, aspect, zNear, zFar); return m;}
    void loadPerspective(double fovy, double aspect, double zNear, double zFar)
    {
        double f = 1/tan(fovy/2);
        double nf = zNear - zFar;
        m[ 0] = f/aspect; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
        m[ 4] = 0; m[ 5] = f; m[ 6] = 0; m[ 7] = 0;
        m[ 8] = 0; m[ 9] = 0; m[10] = (zFar+zNear)/nf; m[11] = -1;
        m[12] = 0; m[13] = 0; m[14] = (2*zFar*zNear)/nf; m[15] = 0;
    }
    
    static PHMatrix frustum(double left,
                            double right,
                            double bottom,
                            double top,
                            double nearVal,
                            double farVal)
    { 
        PHMatrix m;
        m.loadFrustum(left, right, bottom, top, nearVal, farVal);
        return m;
    }
    void loadFrustum(double left,
                     double right,
                     double bottom,
                     double top,
                     double nearVal,
                     double farVal)
    {
        double rl = right - left;
        double tb = top - bottom;
        double fn = farVal - nearVal;
        m[ 0] = (2*nearVal)/rl;  m[ 1] = 0;               m[ 2] = 0;                      m[ 3] = 0;
        m[ 4] = 0;               m[ 5] = (2*nearVal)/tb;  m[ 6] = 0;                      m[ 7] = 0;
        m[ 8] = (right+left)/rl; m[ 9] = (top+bottom)/tb; m[10] = -(farVal+nearVal)/fn;   m[11] = -1;
        m[12] = 0;               m[13] = 0;               m[14] = -(2*farVal*nearVal)/fn; m[15] = 0;
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
    
    PHMatrix transposed() const
    {
        return PHMatrix(m[0], m[4], m[8], m[12],
                        m[1], m[5], m[9], m[13],
                        m[2], m[6], m[10],m[14],
                        m[3], m[7], m[11],m[15]);
    }
    
    PHMatrix inverse() const
    {
        PHMatrix d;
#ifdef PH_MATRIX_VFP
        Matrix4Invert(m,d.m);
#else
        PHInvertMatrix(m,d.m);
#endif
        return d;
    }
    PHMatrix operator * (const PHMatrix & b) const
    {
        PHMatrix d;
#ifdef PH_MATRIX_NEON
        matmul4_neon(m,b.m,d.m);
#else
#ifdef PH_MATRIX_VFP
        Matrix4Mul(m,b.m,d.m);
#else
        PHMultiplyMatrix(m, b.m, d.m);
#endif
#endif
        return d;
    }
    
    PH3DPoint transformPoint(const PH3DPoint & p) const
    {
#ifdef PH_MATRIX_VFP
        GLfloat v[3] = {p.x,p.y,p.z};
        GLfloat r[4];
        Matrix4Vector3Mul(m,v,r);
        return PH3DPoint(r[0]/r[3],r[1]/r[3],r[2]/r[3]);
#else
        return PHTransformPointMatrix(m,p);
#endif
    }

    PH3DPoint untransformPoint(const PH3DPoint & p) const { return inverse().transformPoint(p); }
    PHPoint transformPoint(const PHPoint & p) const
    {
#ifdef PH_MATRIX_VFP
        GLfloat v[3] = {p.x,p.y,1};
        GLfloat r[4];
        Matrix4Vector3Mul(m,v,r);
        return PHPoint(r[0]/r[3],r[1]/r[3]);
#else
        return PHTransformPointMatrix(m,p);
#endif
    }
    PHPoint untransformPoint(const PHPoint & p) const { return inverse().transformPoint(p); }

    PHPoint operator * (const PHPoint & p) const
    {
        return transformPoint(p);
    }

    PH3DPoint operator * (const PH3DPoint & p) const
    {
        return transformPoint(p);
    }

    PHAABox operator * (const PHAABox & p) const
    {
        return PHAABox(transformPoint(p.start), transformPoint(p.end));
    }

    bool operator == (const PHMatrix & o) const
    {
        for (int i = 0; i < 16; i++)
            if (m[i] != o.m[i])
                return false;
        return true;
    }
};

extern const PHMatrix PHIdentityMatrix;

#endif

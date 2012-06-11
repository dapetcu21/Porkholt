/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHPOINT_H
#define PHPOINT_H

#include <Porkholt/Core/PHMain.h>

#ifdef PH_PACKED_STRUCT_PRAGMA
#pragma pack(1)
#endif

struct PHRect;
struct PHPoint
{
    union {
        ph_float x;
        ph_float width;
    };
    union {
        ph_float y;
        ph_float height;
    };
    PHPoint() {};
    PHPoint(ph_float xx,ph_float yy) : x(xx), y(yy) {};
    PHPoint(const PHPoint & o) : x(o.x), y(o.y) {};
    PHPoint(const PHRect & o);
    static PHPoint fromLua(lua_State * L, int index);
    PHPoint & operator += (const PHPoint & othr)
    {
        x+=othr.x;
        y+=othr.y;
        return *this;
    }
    PHPoint operator + (const PHPoint & o) const
    {
        PHPoint res(x+o.x,y+o.y);
        return res;
    }
    PHPoint operator - () const
    {
        PHPoint res(-x,-y);
        return res;
    }
    PHPoint & operator -= (const PHPoint & othr)
    {
        x-=othr.x;
        y-=othr.y;
        return *this;
    }
    PHPoint operator - (const PHPoint & o) const
    {
        PHPoint res(x-o.x,y-o.y);
        return res;
    }
    const PHPoint & operator *= (ph_float d)
    {
        x*=d;
        y*=d;
        return * this;
    }
    PHPoint operator * (ph_float d) const
    {
        PHPoint res(x*d,y*d);
        return res;
    }
    
    PHPoint operator * (const PHPoint & p) const
    {
        PHPoint res(x*p.x,y*p.y);
        return res;
    }
    
    PHPoint operator / (const PHPoint & p) const
    {
        PHPoint res(x/p.x,y/p.y);
        return res;
    }
    
    const PHPoint & operator /= (ph_float d)
    {
        x/=d;
        y/=d;
        return * this;
    }
    PHPoint operator / (ph_float d) const
    {
        PHPoint res(x/d,y/d);
        return res;
    }
    void rotate(ph_float angle)
    {
        ph_float ox=x, oy=y, sinv = sin(angle), cosv = cos(angle);
        x = cosv*ox-sinv*oy;
        y = sinv*ox+cosv*oy;
    }
    ph_float length() const { return sqrt(x*x+y*y); } 
    ph_float squaredLength() const { return x*x+y*y; }
    PHPoint rotated(ph_float angle) const
    {
        PHPoint p;
        ph_float sinv = sin(angle), cosv = cos(angle);
        p.x = cosv*x-sinv*y;
        p.y = sinv*x+cosv*y;
        return p;
    }
    
    bool operator < (const PHPoint & o) const
    {
        if (x==o.x) 
            return y<o.y;
        return x<o.x;
    }
    
    bool operator > (const PHPoint & o) const
    {
        if (x==o.x) 
            return y>o.y;
        return x>o.x;
    }
    
    bool operator == (const PHPoint & o) const
    {
        return (x==o.x && y==o.y);
    }
    
    void saveToLua(lua_State * L) const;
    
    void normalize() { 
#ifdef PH_MATRIX_NEON
        (*this) = normalized();
#else
        (*this)/=length();
#endif
    }
    PHPoint normalized() const { 
#ifdef PH_MATRIX_NEON
        PHPoint p;
        normalize2_neon((const float*)this, (float*)&p);
        return p;
#else
        return (*this) / length();
#endif
    }
    
    ph_float dot(const PHPoint & v) { return dot(*this,v); }
    static ph_float dot(const PHPoint & v1, const PHPoint & v2)
    {
#ifdef PH_MATRIX_NEON
        return dot2_neon((const float *)&v1,(const float *)&v2);
#else
        return v1.x*v2.x+v1.y*v2.y;
#endif
    }
} PH_PACKED_STRUCT;

typedef PHPoint PHSize;
typedef PHPoint PHVector2;
#define PHNullSize PHOriginPoint
#define PHNullVector2 PHOriginPoint
extern const PHSize PHUnitSize;
extern const PHPoint PHOriginPoint;

struct PH3DPoint
{
    union {
        ph_float x;
        ph_float width;
    };
    union {
        ph_float y;
        ph_float height;
    };
    union {
        ph_float z;
        ph_float depth;
    };
    PH3DPoint() {};
    PH3DPoint(const PHPoint & o, ph_float zz) : x(o.x), y(o.y), z(zz) {};
    PH3DPoint(ph_float xx, const PHPoint & o) : x(xx), y(o.x), z(o.y) {};
    PH3DPoint(ph_float xx, ph_float yy) : x(xx), y(yy), z(0) {};
    PH3DPoint(ph_float xx, ph_float yy, ph_float zz) : x(xx), y(yy), z(zz) {};
    PH3DPoint(const PHPoint & o) : x(o.x), y(o.y), z(0) {};
    PH3DPoint(const PH3DPoint & o) : x(o.x), y(o.y), z(o.z) {};
    static PH3DPoint fromLua(lua_State * L, int index);
    PH3DPoint & operator += (const PH3DPoint & othr)
    {
        x+=othr.x;
        y+=othr.y;
        z+=othr.z;
        return *this;
    }
    PH3DPoint operator + (const PH3DPoint & o) const
    {
        PH3DPoint res(x+o.x,y+o.y,z+o.z);
        return res;
    }
    PH3DPoint & operator -= (const PH3DPoint & othr)
    {
        x-=othr.x;
        y-=othr.y;
        z-=othr.z;
        return *this;
    }
    PH3DPoint operator - (const PH3DPoint & o) const
    {
        PH3DPoint res(x-o.x,y-o.y);
        return res;
    }
    PH3DPoint operator - () const
    {
        PH3DPoint res(-x,-y,-z);
        return res;
    }
    const PH3DPoint & operator *= (ph_float d)
    {
        x*=d;
        y*=d;
        z*=d;
        return * this;
    }
    PH3DPoint operator * (ph_float d) const
    {
        PH3DPoint res(x*d,y*d,z*d);
        return res;
    }
    
    PH3DPoint operator * (const PH3DPoint & p) const
    {
        PH3DPoint res(x*p.x,y*p.y,z*p.z);
        return res;
    }
    
    PH3DPoint operator / (const PH3DPoint & p) const
    {
        PH3DPoint res(x/p.x,y/p.y,z/p.z);
        return res;
    }
    
    const PH3DPoint & operator /= (ph_float d)
    {
        x/=d;
        y/=d;
        z/=d;
        return * this;
    }
    PH3DPoint operator / (ph_float d) const
    {
        PH3DPoint res(x/d,y/d,z/d);
        return res;
    }
    ph_float length() const { return sqrt(x*x+y*y+z*z); } 
    ph_float squaredLength() const { return x*x+y*y+z*z; }
    
    bool operator < (const PH3DPoint & o) const
    {
        if (x==o.x) 
        {
            if (y==o.y)
                return z<o.z;
            return y<o.y;
        }
        return x<o.x;
    }
    
    bool operator > (const PH3DPoint & o) const
    {
        if (x==o.x) 
        {
            if (y==o.y)
                return z>o.z;
            return y>o.y;
        }
        return x>o.x;
    }
    
    bool operator == (const PH3DPoint & o) const
    {
        return (x==o.x && y==o.y && z==o.z);
    }
    
    void saveToLua(lua_State * L) const;
    
    void normalize() { 
#ifdef PH_MATRIX_NEON
        (*this) = normalized();
#else
        (*this)/=length();
#endif
    }
    PH3DPoint normalized() const { 
#ifdef PH_MATRIX_NEON
        PH3DPoint p;
        normalize3_neon((const float*)this, (float*)&p);
        return p;
#else
        return (*this) / length();
#endif
    }
    
    ph_float dot(const PH3DPoint & v) { return dot(*this,v); }
    static ph_float dot(const PH3DPoint & v1, const PH3DPoint & v2)
    {
#ifdef PH_MATRIX_NEON
        return dot3_neon((const float *)&v1,(const float *)&v2);
#else
        return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
#endif
    }
    
    PH3DPoint cross(const PH3DPoint & v) { return cross((*this),v); }
    static PH3DPoint cross(const PH3DPoint & v1, const PH3DPoint & v2)
    {
#ifdef PH_MATRIX_NEON
        PH3DPoint d;
        cross3_neon((const float *)&v1,(const float *)&v2,(float*)&d);
        return d;
#else
        return PH3DPoint(v1.y*v2.z-v1.z*v2.y,v1.z*v2.x-v1.x*v2.z,v1.x*v2.y-v1.y*v2.x);
#endif
    }
    
} PH_PACKED_STRUCT;

#ifdef PH_PACKED_STRUCT_PRAGMA
#pragma pack pop
#endif

typedef PH3DPoint PH3DSize;
typedef PH3DPoint PHVector3;
#define PH3DNullSize PH3DOriginPoint
#define PHNullVector3 PH3DOriginPoint 
extern const PH3DSize PH3DUnitSize;
extern const PH3DPoint PH3DOriginPoint;

PH_STATIC_ASSERT(sizeof(PHVector3) == sizeof(ph_float)*3);
PH_STATIC_ASSERT(sizeof(PHVector2) == sizeof(ph_float)*2);

#endif

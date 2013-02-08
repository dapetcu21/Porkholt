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

    void saveToLua(lua_State * L) const;
    static PHPoint fromLua(lua_State * L, int index);

    PHPoint operator - () const
    {
        return PHPoint(-x,-y);
    }


    PHPoint operator + (const PHPoint & o) const
    {
        return PHPoint(x+o.x,y+o.y);
    }
    PHPoint & operator += (const PHPoint & othr)
    {
        x+=othr.x;
        y+=othr.y;
        return *this;
    }

    PHPoint operator - (const PHPoint & o) const
    {
        return PHPoint(x-o.x,y-o.y);
    }
    PHPoint & operator -= (const PHPoint & othr)
    {
        x-=othr.x;
        y-=othr.y;
        return *this;
    }

    PHPoint operator * (ph_float d) const
    {
        return PHPoint(x*d,y*d);
    }
    PHPoint & operator *= (ph_float d)
    {
        x*=d;
        y*=d;
        return * this;
    }

    PHPoint operator * (const PHPoint & p) const
    {
        return PHPoint(x*p.x,y*p.y);
    }

    PHPoint & operator *= (const PHPoint & p)
    {
        x*=p.x;
        y*=p.y;
        return *this;
    }
    
    PHPoint operator / (ph_float d) const
    {
        return PHPoint(x/d,y/d);
    }
    PHPoint & operator /= (ph_float d)
    {
        x/=d;
        y/=d;
        return * this;
    }

    PHPoint operator / (const PHPoint & p) const
    {
        return PHPoint(x/p.x,y/p.y);
    }

    PHPoint & operator /= (const PHPoint & p)
    {
        x/=p.x;
        y/=p.y;
        return *this;
    }

    ph_float length() const { return PHSqrt(x*x+y*y); } 
    ph_float squaredLength() const { return x*x+y*y; }
    ph_float inverseLength() const { return PHInvSqrt(x*x+y*y); }
    PHPoint rotated(ph_float angle) const;
    void rotate(ph_float angle);
   
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

    void normalize() { 
#ifdef PH_MATRIX_NEON
        (*this) = normalized();
#else
        (*this) *= inverseLength();
#endif
    }
    PHPoint normalized() const { 
#ifdef PH_MATRIX_NEON
        PHPoint p;
        normalize2_neon((const float*)this, (float*)&p);
        return p;
#else
        return (*this) * inverseLength();
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

#ifdef BOX2D_H
    PHPoint(const b2Vec2 & o) : x(o.x), y(o.y) {};
    b2Vec2 b2d() const { return b2Vec2(x, y); }
    PHPoint & operator = (const b2Vec2 & o) { x = o.x; y = o.y; return (*this); }
#endif
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
    void saveToLua(lua_State * L) const;

    PH3DPoint operator - () const
    {
        return PH3DPoint(-x,-y,-z);
    }

    PH3DPoint operator + (const PH3DPoint & o) const
    {
        return PH3DPoint(x+o.x,y+o.y,z+o.z);
    }
    PH3DPoint & operator += (const PH3DPoint & othr)
    {
        x+=othr.x;
        y+=othr.y;
        z+=othr.z;
        return *this;
    }

    PH3DPoint operator - (const PH3DPoint & o) const
    {
        return PH3DPoint(x-o.x,y-o.y);
    }
    PH3DPoint & operator -= (const PH3DPoint & othr)
    {
        x-=othr.x;
        y-=othr.y;
        z-=othr.z;
        return *this;
    }

    PH3DPoint operator * (ph_float d) const
    {
        return PH3DPoint(x*d,y*d,z*d);
    }
    PH3DPoint & operator *= (ph_float d)
    {
        x*=d;
        y*=d;
        z*=d;
        return * this;
    }

    PH3DPoint operator * (const PH3DPoint & p) const
    {
        return PH3DPoint(x*p.x,y*p.y,z*p.z);
    }
    PH3DPoint & operator *= (const PH3DPoint & p)
    {
        x*=p.x;
        y*=p.y;
        z*=p.z;
        return * this;
    }

    PH3DPoint operator / (ph_float d) const
    {
        return PH3DPoint(x/d,y/d,z/d);
    }
    PH3DPoint & operator /= (ph_float d)
    {
        x/=d;
        y/=d;
        z/=d;
        return * this;
    }

    PH3DPoint operator / (const PH3DPoint & p) const
    {
        return PH3DPoint(x/p.x,y/p.y,z/p.z);
    }
    
    PH3DPoint & operator /= (const PH3DPoint & p)
    {
        x/=p.x;
        y/=p.y;
        z/=p.z;
        return * this;
    }
    
    ph_float length() const { return PHSqrt(x*x+y*y+z*z); } 
    ph_float squaredLength() const { return x*x+y*y+z*z; }
    ph_float inverseLength() const { return PHInvSqrt(x*x+y*y+z*z); }
    void rotate(ph_float angle);
    PH3DPoint rotated(ph_float angle) const;
    
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
    
    
    void normalize() { 
#ifdef PH_MATRIX_NEON
        (*this) = normalized();
#else
        (*this) *= inverseLength();
#endif
    }
    PH3DPoint normalized() const { 
#ifdef PH_MATRIX_NEON
        PH3DPoint p;
        normalize3_neon((const float*)this, (float*)&p);
        return p;
#else
        return (*this) * inverseLength();
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
    
    PHPoint xy() { return PHPoint(x, y); }
    PHPoint yz() { return PHPoint(y, z); }

#ifdef BOX2D_H
    PH3DPoint(const b2Vec3 & o) : x(o.x), y(o.y), z(o.z) {}
    b2Vec3 b2d() const { return b2Vec3(x, y, z); } 
    PH3DPoint & operator = (const b2Vec3 & o) { x = o.x; y = o.y; return (*this); }
#endif
} PH_PACKED_STRUCT;

struct PHAABox
{
    PH3DPoint start, end;

    PHAABox() {}
    PHAABox(const PH3DPoint & s, const PH3DPoint & e) : start(s), end(e) {}
    PHAABox(const PHPoint & p) : start(p.x, p.y, -1), end(p.x, p.y, 1) {}
    PHAABox(const PHAABox & o) : start(o.start), end(o.end) {};

    PHAABox & operator = (const PHAABox & o) { 
        start = o.start;
        end = o. end;
        return *this;
    }
} PH_PACKED_STRUCT;

#ifdef PH_PACKED_STRUCT_PRAGMA
#pragma pack pop
#endif

typedef PH3DPoint PH3DSize;
typedef PH3DPoint PHVector3;
typedef PHAABox PHPositionalVector;
#define PH3DNullSize PH3DOriginPoint
#define PHNullVector3 PH3DOriginPoint 
extern const PH3DSize PH3DUnitSize;
extern const PH3DPoint PH3DOriginPoint;

PH_STATIC_ASSERT(sizeof(PHVector3) == sizeof(ph_float)*3);
PH_STATIC_ASSERT(sizeof(PHVector2) == sizeof(ph_float)*2);

#endif

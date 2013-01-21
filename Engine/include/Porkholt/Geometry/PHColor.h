/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHCOLOR_H
#define PHCOLOR_H

#ifdef PH_PACKED_STRUCT_PRAGMA
#pragma pack(1)
#endif

struct PHQuaternion;
struct PHColor
{
    union
    {
        ph_float r;
        ph_float x;
    };
    union
    {
        ph_float g;
        ph_float y;
    };
    union
    {
        ph_float b;
        ph_float z;
    };
    union
    {
        ph_float a;
        ph_float t;
    };
    bool operator == (const PHColor & o) const {
        return (r==o.r)&&(g==o.g)&&(b==o.b)&&(a==o.a);
    }
    bool operator != (const PHColor & o) const {
        return (r!=o.r)||(g!=o.g)||(b!=o.b)||(a!=o.a);
    }
    const PHColor & multiplyAlpha(ph_float d)
    {
        if (a>0) a*=d;
        return * this;
    }
    PHColor multipliedAlpha(ph_float d) const
    {
        if (a<0) return *this;
        PHColor res(r,g,b,a*d);
        return res;
    }
    const PHColor & operator *= (ph_float d)
    {
        a*=d; r*=d; g*=d; b*=d;
        return * this;
    }
    PHColor operator * (ph_float d) const
    {
        PHColor res(r*d,g*d,b*d,a*d);
        return res;
    }
    const PHColor & operator *= (const PHColor & d)
    {
        a*=d.a; r*=d.r; g*=d.g; b*=d.b;
        return * this;
    }
    PHColor operator * (const PHColor & d) const
    {
        PHColor res(r*d.r,g*d.g,b*d.b,a*d.a);
        return res;
    } 
    const PHColor & operator /= (ph_float d)
    {
        d = 1.0f/d;
        a*=d; r*=d; g*=d; b*=d;
        return * this;
    }
    PHColor operator / (ph_float d) const
    {
        d = 1.0f/d;
        PHColor res(r*d,g*d,b*d,a*d);
        return res;
    }
    const PHColor & operator /= (const PHColor & d)
    {
        a/=d.a; r/=d.r; g/=d.g; b/=d.b;
        return * this;
    }
    PHColor operator / (const PHColor & d) const
    {
        PHColor res(r/d.r,g/d.g,b/d.b,a/d.a);
        return res;
    } 
    const PHColor & operator += (const PHColor & d)
    {
        a+=d.a; r+=d.r; g+=d.g; b+=d.b;
        return * this;
    }
    PHColor operator + (const PHColor & d) const
    {
        PHColor res(r+d.r,g+d.g,b+d.b,a+d.a);
        return res;
    } 

    const PHColor & operator -= (const PHColor & d)
    {
        a-=d.a; r-=d.r; g-=d.g; b-=d.b;
        return * this;
    }
    PHColor operator - (const PHColor & d) const
    {
        PHColor res(r-d.r,g-d.g,b-d.b,a-d.a);
        return res;
    } 

    PHColor(ph_float red, ph_float green, ph_float blue, ph_float alpha) : r(red), g(green), b(blue), a(alpha) {};
    PHColor(ph_float red, ph_float green, ph_float blue) : r(red), g(green), b(blue), a(1.0f) {};
    PHColor(const PHQuaternion & q);
    PHColor() {};
    bool isValid() { return (r>=0 && r<=1 && g>=0 && g<=1 && b>=0 && b<=1 && a>=0 && a<=1); }
    bool isNull() { return !(r || g || b || a); }
    bool isBlack() { return !(r || g || b); }
    static PHColor fromLua(lua_State * L, int index);
    void saveToLua(lua_State * L) const;

    ph_float length() const { return sqrt(x*x+y*y+z*z+t*t); } 
    ph_float squaredLength() const { return x*x+y*y+z*z+t*t; }

    void normalize() { 
#ifdef PH_MATRIX_NEON
        (*this) = normalized();
#else
        (*this) /= length();
#endif
    }
    PHColor normalized() const { 
#ifdef PH_MATRIX_NEON
        PHColor p;
        normalize4_neon((const float*)this, (float*)&p);
        return p;
#else
        return (*this) / length();
#endif
    }

    ph_float dot(const PHColor & v) { return dot(*this,v); }
    static ph_float dot(const PHColor & v1, const PHColor & v2)
    {
#ifdef PH_MATRIX_NEON
        return dot4_neon((const float *)&v1,(const float *)&v2);
#else
        return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.a*v2.a;
#endif
    }


} PH_PACKED_STRUCT;

struct PH24BitColor
{
    uint8_t r,g,b,a;
    bool operator == (const PH24BitColor & o) const {
        return (r==o.r)&&(g==o.g)&&(b==o.b)&&(a==o.a);
    }
    bool operator != (const PH24BitColor & o) const {
        return (r!=o.r)||(g!=o.g)||(b!=o.b)||(a!=o.a);
    }
    PH24BitColor() {};
    PH24BitColor(const PHColor & o) : r(o.r*255), g(o.g*255), b(o.b*255), a(o.a*255) {}
} PH_PACKED_STRUCT;

#ifdef PH_PACKED_STRUCT_PRAGMA
#pragma pack pop
#endif

typedef PHColor PHVector4;

extern const PHColor PHClearColor;
extern const PHColor PHBlackColor;
extern const PHColor PHGrayColor;
extern const PHColor PHWhiteColor;
extern const PHColor PHInvalidColor;

namespace PHColor_conditions {
    PH_STATIC_ASSERT(sizeof(PHVector4) == sizeof(ph_float)*4);
    PH_STATIC_ASSERT(sizeof(PH24BitColor) == 4);
}

#endif

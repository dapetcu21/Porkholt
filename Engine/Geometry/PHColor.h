//
//  PHColor.h
//  Porkholt
//
//  Created by Marius Petcu on 2/1/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHCOLOR_H
#define PHCOLOR_H

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
    const PHColor & operator *= (ph_float d)
    {
        if (a>0) a*=d;
        return * this;
    }
    PHColor operator * (ph_float d) const
    {
        if (a<0) return *this;
        PHColor res(r,g,b,a*d);
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
    PHColor(ph_float red, ph_float green, ph_float blue, ph_float alpha) : r(red), g(green), b(blue), a(alpha) {};
    PHColor(ph_float red, ph_float green, ph_float blue) : r(red), g(green), b(blue), a(1.0f) {};
    PHColor() {};
    bool isValid() { return (r>=0 && r<=1 && g>=0 && g<=1 && b>=0 && b<=1 && a>=0 && a<=1); }
    static PHColor fromLua(lua_State * L, int index);
    void saveToLua(lua_State * L) const;
};

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
};

typedef PHColor PHVector4;

extern const PHColor PHClearColor;
extern const PHColor PHBlackColor;
extern const PHColor PHGrayColor;
extern const PHColor PHWhiteColor;
extern const PHColor PHInvalidColor;

#endif

//
//  PHPoint.h
//  Porkholt
//
//  Created by Marius Petcu on 2/1/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHPOINT_H
#define PHPOINT_H

#include "PHMain.h"

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
    ph_float length() { return sqrt(x*x+y*y); } 
    ph_float squaredLength() { return x*x+y*y; }
    void normalize() { (*this)/=length(); }
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
};

typedef PHPoint PHSize;
#define PHNullSize PHOriginPoint
extern const PHSize PHUnitSize;
extern const PHPoint PHOriginPoint;

#endif

/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHRECT_H
#define PHRECT_H

//#include <Porkholt/Geometry/PHPoint.h>

struct PHRect
{
	PHRect(float _x, float _y, float w, float h) : x(_x),y(_y),width(w),height(h) {};
    PHRect(const PHVector2 & o, float w, float h) : x(o.x), y(o.y), width(w), height(h) {};
    PHRect(const PHVector2 & o, const PHVector2 & s) : x(o.x), y(o.y), width(s.width), height(s.height) {};
    PHRect(float _x, float _y, const PHVector2 & s) : x(_x), y(_y), width(s.width), height(s.height) {};
	PHRect(const PHRect & o) : x(o.x),y(o.y),width(o.width),height(o.height) {};
	PHRect() {};
	float x,y;
	float width,height;
    static PHRect fromLua(lua_State * L, int index);
    void saveToLua(lua_State * L) const;
    
    PHPoint origin() const { return PHPoint(x,y); }
    PHSize size() const { return PHSize(width,height); }
    PHPoint center() const { return PHPoint(x+width/2,y+height/2); }
    void setOrigin(const PHPoint & o) { x = o.x; y = o.y; }
    void setSize(const PHSize & o) { width = o.width; y = o.height; }
    void setCenter(const PHPoint & o) { x = o.x-width/2; y = o.y-height/2; }
    
    PHPoint corner(int index) const { return PHPoint(x+((index&1)?width:0),y+((index&2)?height:0)); }
    
    PHRect portionOf(const PHRect & o) const
    {
        return PHRect(  o.x + o.width * x, 
                        o.y + o.height * y,
                        o.x + o.width * width, 
                        o.y + o.height * height);
    }

    PHRect & operator += (const PHPoint & othr)
    {
        x+=othr.x;
        y+=othr.y;
        return *this;
    }
    PHRect operator + (const PHPoint & o) const
    {
        return PHRect(x+o.x,y+o.y,width,height);
    }
    PHRect & operator += (const PHRect & othr)
    {
        x+=othr.x;
        y+=othr.y;
        width+=othr.width;
        height+=othr.height;
        return *this;
    }
    PHRect operator + (const PHRect & o) const
    {
        return PHRect(x+o.x,y+o.y,width+o.width,height+o.height);
    }
    PHRect & operator -= (const PHPoint & othr)
    {
        x-=othr.x;
        y-=othr.y;
        return *this;
    }
    PHRect operator - (const PHPoint & o) const
    {
        return PHRect(x-o.x,y-o.y,width,height);
    }
    PHRect & operator -= (const PHRect & othr)
    {
        x-=othr.x;
        y-=othr.y;
        width-=othr.width;
        height-=othr.height;
        return *this;
    }
    PHRect operator - (const PHRect & o) const
    {
        return PHRect(x-o.x,y-o.y,width-o.width,height-o.height);
    }
    const PHRect & operator *= (float d)
    {
        width*=d;
        height*=d;
        return * this;
    }
    PHRect operator * (float d) const
    {
        return PHRect(x,y,width*d,height*d);
    }
    const PHRect & operator *= (const PHSize & d)
    {
        width*=d.x;
        height*=d.y;
        return * this;
    }
    PHRect operator * (const PHSize & d) const
    {
        return PHRect(x,y,width*d.x,height*d.y);
    }
    const PHRect & operator /= (float d)
    {
        width/=d;
        height/=d;
        return * this;
    }
    PHRect operator / (float d) const
    {
        return PHRect(x,y,width/d,height/d);
    }
};

extern const PHRect PHWholeRect;
extern const PHRect PHInvalidRect;
extern const PHRect PHNullRect;

#endif

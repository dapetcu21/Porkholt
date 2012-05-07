//
//  PHRect.h
//  Porkholt
//
//  Created by Marius Petcu on 2/1/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHRECT_H
#define PHRECT_H

//#include "PHPoint.h"

struct PHRect
{
	PHRect(ph_float _x, ph_float _y, ph_float w, ph_float h) : x(_x),y(_y),width(w),height(h) {};
	PHRect(const PHRect & o) : x(o.x),y(o.y),width(o.width),height(o.height) {};
	PHRect() {};
	ph_float x,y;
	ph_float width,height;
    static PHRect fromLua(lua_State * L, int index);
    void saveToLua(lua_State * L) const;
    
    PHPoint origin() const { return PHPoint(x,y); }
    PHSize size() const { return PHSize(width,height); }
    PHPoint center() const { return PHPoint(x+width/2,y+height/2); }
    void setOrigin(const PHPoint & o) { x = o.x; y = o.y; }
    void setSize(const PHSize & o) { width = o.width; y = o.height; }
    void setCenter(const PHPoint & o) { x = o.x-width/2; y = o.y-height/2; }
    
    PHPoint corner(int index) const { return PHPoint(x+((index&1)?width:0),y+((index&2)?height:0)); }
    
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
    const PHRect & operator *= (ph_float d)
    {
        width*=d;
        height*=d;
        return * this;
    }
    PHRect operator * (ph_float d) const
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
    const PHRect & operator /= (ph_float d)
    {
        width/=d;
        height/=d;
        return * this;
    }
    PHRect operator / (ph_float d) const
    {
        return PHRect(x,y,width/d,height/d);
    }
};

extern const PHRect PHWholeRect;
extern const PHRect PHInvalidRect;
extern const PHRect PHNullRect;

#endif

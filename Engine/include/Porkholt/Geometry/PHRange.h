/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHRANGE_H
#define PHRANGE_H

struct PHRange
{
    //represents [start, length)
    unsigned int start, length;
    PHRange() : start(0), length(0) {}
    PHRange(int s, int l) : start(s), length(l) {}
    PHRange(const PHRange & o) : start(o.start), length(o.length) {}
    int end() const { return start+length; }
    
    static int wrap(int index, int n) {
        if (index<0)
            index+=((-index)/n + 1)*n;
        return index%n;
    }
    
    bool intersects(unsigned int index) const
    {
        return (index>=start && index<start+length);
    }
    
    bool intersects(const PHRange & range) const
    {
        return intersects(range.start) || intersects(range.end()-1) || range.intersects(start) || range.intersects(end()-1);
    }
    
    bool intersectsCircularily(unsigned int index, unsigned int n) const
    {
        if (intersects(n))
            return PHRange(start,n-start).intersects(index) || PHRange(0,wrap(end(),n)).intersects(index);
        return intersects(index);
    }
    
    bool intersectsCircularily(const PHRange & range, unsigned int n) const
    {
        if (intersects(n))
            return PHRange(start,n-start).intersects(range) || PHRange(0,wrap(end(),n)).intersects(range);
        return intersects(range);
    }
    
    PHRange & operator += (unsigned int offset)
    {
        start+=offset;
        return *this;
    }
    
    PHRange operator + (unsigned int offset) const
    {
        return PHRange(start+offset,length);
    }
    
    bool operator < (const PHRange & o) const
    {
        if (start == o.start) return length<o.length;
        return start<o.start;
    }
    
    bool operator <= (const PHRange & o) const
    {
        if (start == o.start) return length<=o.length;
        return start<=o.start;
    }
    
    bool operator > (const PHRange & o) const
    {
        if (start == o.start) return length>o.length;
        return start>o.start;
    }
    
    bool operator >= (const PHRange & o) const
    {
        if (start == o.start) return length>=o.length;
        return start>=o.start;
    }
    
    bool operator == (const PHRange & o) const
    {
        return (o.start == start && o.length == length);
    }
    
    static PHRange fromLua(lua_State * L, int index);
    void saveToLua(lua_State * L) const;
    void toLua(lua_State * L) const { saveToLua(L); }
};

extern const PHRange PHInvalidRange;

#endif

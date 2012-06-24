/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHUTILITIES_H
#define PHUTILITIES_H

#include <Porkholt/Core/PHMain.h>

class PHHashedString
{
public:
    string str;
private:
    uint32_t h;
public:
    PHHashedString() : str(), h(0) {}
    PHHashedString(const PHHashedString & o) : str(o.str), h(o.h) {}
    PHHashedString(const string & s) : str(s) { recomputeHash(); }
    
    void recomputeHash();

    void setString(const string & s) 
    {
        str = s;
        recomputeHash();
    }

    const string & getString() { return str; }

    bool operator == (const PHHashedString & o) const
    {
        return ((h == o.h) && (str == o.str));
    }

    bool operator != (const PHHashedString & o) const
    {
        return ((h != o.h) || (str != o.str));
    }

    bool operator < (const PHHashedString & o) const
    {
        if (h == o.h)
            return str < o.str;
        return h < o.h;
    }
    
    bool operator > (const PHHashedString & o) const
    {
        if (h == o.h)
            return str > o.str;
        return h > o.h;
    }
    
    bool operator <= (const PHHashedString & o) const
    {
        if (h == o.h)
            return str <= o.str;
        return h <= o.h;
    }
    
    bool operator >= (const PHHashedString & o) const
    {
        if (h == o.h)
            return str >= o.str;
        return h >= o.h;
    }

    PHHashedString & operator = (const PHHashedString & o)
    {
        h = o.h;
        str = o.str;
        return (*this);
    }
};

uint32_t PHCRC32(uint32_t crc, const uint8_t * buf, size_t size);

#endif

/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHUTILITIES_H
#define PHUTILITIES_H

#include <Porkholt/Core/PHMain.h>

template <class T>
class PHHashedStr
{
public:
    T str;
private:
    uint32_t h;
public:
    PHHashedStr<T>() : str(), h(0) {}
    PHHashedStr<T>(const PHHashedStr<T> & o) : str(o.str), h(o.h) {}
    PHHashedStr<T>(const T & s) : str(s) { recomputeHash(); }
    
    void recomputeHash();

    void setString(const T & s) 
    {
        str = s;
        recomputeHash();
    }

    PHHashedStr<T> & operator = (const PHHashedStr<T> & o)
    {
        h = o.h;
        str = o.str;
        return (*this);
    }

    const string & getString() { return str; }
    int compare(const PHHashedStr<T> & o) const
    {
        if (h == o.h)
            return str-o.str;
        else
        return (int32_t)h-(int32_t)o.h;
    }
    
    bool operator == (const PHHashedStr<T> & o) const
    {
        return compare(o) == 0;
    }

    bool operator != (const PHHashedStr<T> & o) const
    {
        return compare(o) != 0;
    }

    bool operator < (const PHHashedStr<T> & o) const
    {
        return compare(o) < 0;
    }
    
    bool operator > (const PHHashedStr<T> & o) const
    {
        return compare(o) > 0;
    }
    
    bool operator <= (const PHHashedStr<T> & o) const
    {
        return compare(o) <= 0;
    }
    
    bool operator >= (const PHHashedStr<T> & o) const
    {
        return compare(o) >= 0;
    }
};

template <>
inline int PHHashedStr<string>::compare(const PHHashedStr<string> & o) const
{
    if (h == o.h)
        return str.compare(o.str);
    else
    return (int32_t)h-(int32_t)o.h;
}

template <>
inline int PHHashedStr<const char *>::compare(const PHHashedStr<const char*> & o) const
{
    if (h == o.h)
        return strcmp(str, o.str);
    else
    return (int32_t)h-(int32_t)o.h;
}

typedef PHHashedStr<string> PHHashedString;
typedef PHHashedStr<const char *> PHHashedCString;

uint32_t PHCRC32(uint32_t crc, const uint8_t * buf, size_t size);
uint32_t PHCRC32Until(uint32_t crc, const uint8_t * p, uint8_t until = 0);

#endif

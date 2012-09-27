/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */
 
#ifndef PHFILE_H
#define PHFILE_H

#include <Porkholt/IO/PHStream.h>

class PHFile : public PHStream
{
public:
    PHFile(const string & p) : PHStream(p) {}

    virtual size_t pointer() = 0;
    virtual void setPointer(size_t p) = 0;
    virtual size_t size() = 0;
    virtual void seek(ssize_t off) { setPointer(pointer()+off); }
    
    bool isFile() { return true; }

    uint8_t * loadToBuffer(size_t & sz)
    {
        sz = size();
        uint8_t * buf = new uint8_t[sz+1];
        buf[sz] = 0;
        try {
            open(Read);
            read(buf, sz);
            close();
        } catch (...)
        {
            delete[] buf;
            throw;
        }
        return buf;
    }

    uint8_t * loadToBuffer()
    {
        size_t sz;
        return loadToBuffer(sz);
    }

    string loadToString()
    {
        size_t sz;
        uint8_t * buf = loadToBuffer(sz);
        string s((char*)buf, sz);
        delete[] buf;
        return s;
    }

    void loadToString(string & s)
    {
        size_t sz;
        uint8_t * buf = loadToBuffer(sz);
        s.assign((char*)buf, sz);
        delete[] buf;
    }
};

#endif

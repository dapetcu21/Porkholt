/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSTREAM_H
#define PHSTREAM_H
#include "PHInode.h"

class PHStream: public PHInode
{
public:
    enum openflags {
        Read =   1<<0,
        Write =  1<<1,
        Create = 1<<2,
        Append = 1<<3
    };
    virtual void open(int flags) = 0;
    virtual size_t read(uint8_t * buf, size_t size) = 0;
    virtual size_t write(const uint8_t * buf, size_t size) = 0;
    virtual void close() = 0;
    
    bool isStream() { return true; }
};

#endif

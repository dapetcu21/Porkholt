/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSTREAM_H
#define PHSTREAM_H
#include <Porkholt/IO/PHInode.h>

class PHStream: public PHInode
{
protected:
    int coflags;
public:
    enum openflags {
        Read =   1<<0,
        Write =  1<<1,
        Create = 1<<2,
        Append = 1<<3,
        Trunc = 1<<4,
    };
    PHStream(const string & p) : PHInode(p), coflags(0) {};

    virtual void open(int flags) { coflags = flags; } 
    int currentOpenFlags() { return coflags; }
    bool openedForReading() { return (coflags & Read)!=0; }
    bool openedForWriting() { return (coflags & Write)!=0; }
    bool opened() { return coflags!=0; }
    virtual size_t read(uint8_t * buf, size_t size) = 0;
    virtual size_t write(const uint8_t * buf, size_t size) = 0;
    size_t write(const string & s) { return write((uint8_t*)s.c_str(), s.size()); }
    virtual void close() { coflags = 0; }
    
    bool isStream() { return true; }
};

#endif

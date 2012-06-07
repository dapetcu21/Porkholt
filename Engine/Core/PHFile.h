/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */
 
#ifndef PHFILE_H
#define PHFILE_H

#include "PHStream.h"

class PHFile : public PHStream
{
    virtual size_t pointer() = 0;
    virtual void setPointer(size_t p) = 0;
    virtual size_t size() = 0;
    virtual void seek(ssize_t off) { setPointer(pointer()+off); }
    
    bool isFile() { return true; }
};

#endif

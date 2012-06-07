/*
 *  PHFile.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */
 
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

/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHINODE_H
#define PHINODE_H
#include <Porkholt/Core/PHMain.h>

class PHInode : public PHObject
{
public:
    virtual bool isStream() { return false; }
    virtual bool isFile() { return false; }
    virtual bool isDirectory() { return false; }
    static PHInode * itemAtFSPath(const string & p);
};

#endif

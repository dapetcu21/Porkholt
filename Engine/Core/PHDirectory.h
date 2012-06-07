/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHDIRECTORY_H
#define PHDIRECTORY_H
#include "PHMain.h"

class PHDirectory: public PHInode
{
public:
    bool isDirectory() { return true; }
    virtual PHInode * itemAtPath(const string & s) throw = 0;
};

#endif

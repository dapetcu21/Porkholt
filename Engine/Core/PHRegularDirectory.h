/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHREGULARDIRECTORY_H
#define PHREGULARDIRECTORY_H
#include "PHInode.h"

class PHRegularDirectory: public PHInode
{
    string path;
    PHRegularDirectory(const string & dpath, int dummy);
    friend class PHInode;
public:
    PHRegularDirectory(const string & dpath);
    ~PHRegularDirectory();
    PHInode * itemAtPath(const string & s);
};

#endif

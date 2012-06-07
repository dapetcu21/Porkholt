/*
 *  PHRegularDirectory.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

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

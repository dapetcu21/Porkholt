/*
 *  PHDirectory.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

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

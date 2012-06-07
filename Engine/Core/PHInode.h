/*
 *  PHInode.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHINODE_H
#define PHINODE_H
#include "PHMain.h"

class PHInode : public PHObject
{
public:
    virtual bool isStream() { return false; }
    virtual bool isFile() { return false; }
    virtual bool isDirectory() { return false; }
    static PHInode * itemAtFSPath(const string & p);
};

#endif

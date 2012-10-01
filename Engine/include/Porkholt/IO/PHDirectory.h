/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHDIRECTORY_H
#define PHDIRECTORY_H
#include <Porkholt/IO/PHInode.h>

class PHDirectory: public PHInode
{
public:
    PHDirectory(const string & s) : PHInode(s) {}

    bool isDirectory() { return true; }
    virtual PHInode * itemAtPath(const string & s) = 0;
    virtual PHFile * fileAtPath(const string & s) = 0;
    virtual PHDirectory * directoryAtPath(const string & s) = 0;

    virtual bool itemExists(const string & path) = 0;
    virtual bool fileExists(const string & path) = 0;
    virtual bool directoryExists(const string & path) = 0;

    virtual void stat(const string & path, PHInode::stat_t & s) = 0;
};

#endif

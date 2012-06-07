/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHRegularDirectory.h"
#include <sys/errno.h>

PHRegularDirectory::PHRegularDirectory(const string & p) : path (p) 
{
    struct stat results;
    if (stat(path.c_str(), &results) != 0)
        throw string(strerror(errno));
    if (!(results.st_mode & S_IFDIR))
        throw string("not a directory");
}

PHRegularDirectory::PHRegularDirectory(const string & p, int dummy) : path (p){}
PHRegularDirectory::~PHRegularDirectory() {}

PHInode * PHRegularDirectory::itemAtPath(const string & p) 
{
    return PHInode::itemAtFSPath(path + "/" + p);
}


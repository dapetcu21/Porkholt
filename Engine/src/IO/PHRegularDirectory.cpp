/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/IO/PHRegularDirectory.h>
#include <sys/errno.h>

PHRegularDirectory::PHRegularDirectory(const string & p) : PHDirectory(p) 
{
    struct stat results;
    if (stat(_path.c_str(), &results) != 0)
        throw string(strerror(errno));
    if (!(results.st_mode & S_IFDIR))
        throw string("not a directory");
}

PHRegularDirectory::PHRegularDirectory(const string & p, int dummy) : PHDirectory(p) {}
PHRegularDirectory::~PHRegularDirectory() {}

PHInode * PHRegularDirectory::itemAtPath(const string & p) 
{
    return PHInode::itemAtFSPath(_path + "/" + p);
}

PHFile * PHRegularDirectory::fileAtPath(const string & p) 
{
    return PHInode::fileAtFSPath(_path + "/" + p);
}

PHDirectory * PHRegularDirectory::directoryAtPath(const string & p) 
{
    return PHInode::directoryAtFSPath(_path + "/" + p);
}

bool PHRegularDirectory::itemExists(const string & p)
{
    return PHInode::itemExistsOnFS(_path + "/" + p);
}

bool PHRegularDirectory::directoryExists(const string & p)
{
    return PHInode::directoryExistsOnFS(_path + "/" + p);
}

bool PHRegularDirectory::fileExists(const string & p)
{
    return PHInode::fileExistsOnFS(_path + "/" + p);
}

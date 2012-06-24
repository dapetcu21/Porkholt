/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/IO/PHInode.h>
#include <Porkholt/IO/PHRegularFile.h>
#include <Porkholt/IO/PHRegularDirectory.h>
#include <sys/stat.h>
#include <sys/errno.h>

PHInode * PHInode::itemAtFSPath(const string & p)
{
    struct stat results;
    if (stat(p.c_str(), &results) != 0)
        throw string(strerror(errno));
    if (results.st_mode & S_IFDIR)
        return new PHRegularDirectory(p, false);
    if (results.st_mode & S_IFREG)
        return new PHRegularFile(p, false);
}

PHFile * PHInode::fileAtFSPath(const string & p)
{
    struct stat results;
    if (stat(p.c_str(), &results) != 0)
        throw string(strerror(errno));
    if (results.st_mode & S_IFREG)
        return new PHRegularFile(p ,false);
    throw string("not a file");
}

PHDirectory * PHInode::directoryAtFSPath(const string & p)
{
    struct stat results;
    if (stat(p.c_str(), &results) != 0)
        throw string(strerror(errno));
    if (results.st_mode & S_IFDIR)
        return new PHRegularDirectory(p ,false);
    throw string("not a directory");
}

bool PHInode::itemExistsOnFS(const string & p)
{
    struct stat results;
    if (stat(p.c_str(), &results) != 0)
    {
        if ((errno != ENOENT) && (errno != ENOTDIR))
            PHLog("stat() failed on \"%s\": %s", p.c_str(), strerror(errno));
        return false;
    }
    return true;
}

bool PHInode::directoryExistsOnFS(const string & p)
{
    struct stat results;
    if (stat(p.c_str(), &results) != 0)
    {
        if ((errno != ENOENT) && (errno != ENOTDIR))
            PHLog("stat() failed on \"%s\": %s", p.c_str(), strerror(errno));
        return false;
    }
    return ((results.st_mode & S_IFDIR) != 0);
}

bool PHInode::fileExistsOnFS(const string & p)
{
    struct stat results;
    if (stat(p.c_str(), &results) != 0)
    {
        if ((errno != ENOENT) && (errno != ENOTDIR))
            PHLog("stat() failed on \"%s\": %s", p.c_str(), strerror(errno));
        return false;
    }
    return ((results.st_mode & S_IFREG) != 0);
}

PHInode::PHInode(const string & path) : _path(path)
{
    size_t n = path.size();
    while (n && (path[n-1] == '/')) n--;
    size_t i = n;
    _base = 0;
    _ext = n;
    _base_end = n;
    bool db = true, dx = true;
    while (i && (db || dx)) {
        i--;
        if (dx && (path[i] == '.'))
        {
            _ext = i+1;
            _base_end = i;
            dx = false;
        }
        if (db && (path[i] == '/'))
        {
            _base = i+1;
            db = false;
        }
    }
}

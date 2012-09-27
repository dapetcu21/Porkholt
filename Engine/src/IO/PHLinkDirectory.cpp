/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/IO/PHLinkDirectory.h>
#include <Porkholt/IO/PHFile.h>
#include <Porkholt/Core/PHAutoreleasePool.h>

#define INIT root(dir), PHDirectory(dir->path())

PHLinkDirectory::PHLinkDirectory(PHDirectory * dir, PHFile * symlinks) : INIT
{
    init(symlinks);
}

PHLinkDirectory::PHLinkDirectory(PHDirectory * dir) : INIT
{
    PHAutoreleasePool ap;
    init(dir->fileAtPath("_symlinks"));
}

void PHLinkDirectory::init(PHFile * symlinks)
{
    root->retain();
    //TODO:read file;
}

PHLinkDirectory::~PHLinkDirectory()
{
    if (root) 
        root->release();
}

const string & PHLinkDirectory::_resolvedPath(const string & p)
{
    map<PHHashedString, string>::iterator i = links.find(PHHashedString(p));
    if (i != links.end())
        return i->second;
        
    return p;
}

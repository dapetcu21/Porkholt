/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/IO/PHLinkDirectory.h>
#include <Porkholt/IO/PHFile.h>
#include <Porkholt/Core/PHAutoreleasePool.h>

struct PHLinkDirectory_data
{
    int ref;
    map<PHHashedString, string> links;
};


#define INIT root(dir), PHDirectory(dir->path())

PHLinkDirectory::PHLinkDirectory(PHDirectory * dir, PHFile * symlinks) : root(dir), PHDirectory(dir->path())
{
    init(symlinks);
}

PHLinkDirectory::PHLinkDirectory(PHDirectory * dir) : root(dir), PHDirectory(dir->path())
{
    PHAutoreleasePool ap;
    init(dir->fileAtPath("_symlinks"));
}

PHLinkDirectory::PHLinkDirectory(PHDirectory * dir, PHLinkDirectory_data * _d, const string & _prefix): PHDirectory(dir->path() + "/" + _prefix), root(dir), d(_d), prefix(_prefix + "/")
{
    d->ref++;
    dir->retain();
}

void PHLinkDirectory::init(PHFile * symlinks)
{
    root->retain();
    uint8_t * data = symlinks->loadToBuffer();
    char * s = (char*)data;
    char * p = s;
    char * v[2] = {NULL, NULL};
    int i = 0;
    d = new PHLinkDirectory_data;
    d->ref = 1;

    while(1)
    {
        for(; (*p) && ((*p) != '\n')&&((*p) != '\r'); p++);
        if (!*p) break;
        (*p)='\0';
        if (*s)
        {
            v[i] = s;
            i = !i;
            if (!i)
                d->links.insert(make_pair(PHHashedString(v[0]), string(v[1])));
        }
        s = ++p;
    }
}

PHLinkDirectory::~PHLinkDirectory()
{
    if (root) 
        root->release();
    if (!--d->ref)
        delete d;
}

const string & PHLinkDirectory::_resolvedPath(const string & p)
{
    map<PHHashedString, string>::iterator i = d->links.find(PHHashedString(p));
    if (i != d->links.end())
        return i->second;
    return p;
}

PHInode * PHLinkDirectory::itemAtPath(const string & s) 
{ 
    string ss = _resolvedPath(prefix+s);
    stat_t st;
    root->stat(ss, st);
    if (st.type == PHInode::stat_t::Directory)
    {
        PHLinkDirectory * dir = new PHLinkDirectory(root, d, ss);
        dir->autorelease();
        return dir;
    }
    return root->itemAtPath(ss);
}

PHDirectory * PHLinkDirectory::directoryAtPath(const string & s)
{
   PHLinkDirectory * dir = new PHLinkDirectory(root, d, _resolvedPath(prefix+s));
   dir->autorelease();
   return dir;
}


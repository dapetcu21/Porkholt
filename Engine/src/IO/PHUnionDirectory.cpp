/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/IO/PHUnionDirectory.h>

#define iter vector<PHDirectory*>::iterator
#define foreach for (iter i = items.begin(); i != items.end(); i++)

PHUnionDirectory::PHUnionDirectory(const string & s, const vector<PHDirectory*> & _items) : PHDirectory(s), items(_items)
{
    foreach
        (*i)->retain();
}

PHUnionDirectory::PHUnionDirectory(const string & s) : PHDirectory(s)
{
}

PHUnionDirectory::PHUnionDirectory() : PHDirectory("${union_fs}")
{
}

PHUnionDirectory::~PHUnionDirectory()
{
    foreach
        (*i)->release();
}

void PHUnionDirectory::addDirectory(PHDirectory * dir)
{
    if (!dir) return;
    dir->retain();
    items.push_back(dir);
}

PHInode * PHUnionDirectory::itemAtPath(const string & s)
{
    bool file = true;
    vector<PHDirectory*> v;
    foreach
    {
        PHInode * in = NULL;
        try { in = (*i)->itemAtPath(s); }
        catch(...) {}
        if (in)
        {
            if (file)
            {
                if (in->isFile())
                    return in;
                else
                    file = false;
            } else {
                if (in->isDirectory())
                    v.push_back((PHDirectory*)in);
            }
        }
    }
    switch (v.size())
    {
        case 0:
            throw string("no such file or directory");
        case 1:
            return v[0];
        default:
            PHUnionDirectory * dir = new PHUnionDirectory(path() + "/" + s, v);
            dir->autorelease();
            return dir;
    }
}

PHFile * PHUnionDirectory::fileAtPath(const string & s)
{
    foreach
    {
        PHFile * f = NULL;
        try { f = (*i)->fileAtPath(s); }
        catch(...) {}
        if (f)
            return f;
    }
    throw string("no such file");
}

PHDirectory * PHUnionDirectory::directoryAtPath(const string & s)
{
    vector<PHDirectory*> v;
    foreach
        try { v.push_back((*i)->directoryAtPath(s)); }
        catch(...) {}
    switch (v.size())
    {
        case 0:
            throw string("no such directory");
        case 1:
            return v[0];
        default:
            PHUnionDirectory * dir = new PHUnionDirectory(path() + "/" + s, v);
            dir->autorelease();
            return dir;
    }
}

bool PHUnionDirectory::itemExists(const string & path)
{
    foreach
        if ((*i)->itemExists(path))
            return true;
    return false;
}

bool PHUnionDirectory::fileExists(const string & path)
{
    foreach
        if ((*i)->fileExists(path))
            return true;
    return false;
}

bool PHUnionDirectory::directoryExists(const string & path)
{
    foreach
        if ((*i)->directoryExists(path))
            return true;
    return false;
}

void PHUnionDirectory::stat(const string & path, PHInode::stat_t & s)
{
    s.type = stat_t::None; 
    s.size = 0;

    foreach
    {
        (*i)->stat(path, s);
        if (s.type != stat_t::None)
            return;
    }
}


/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/IO/PHEmbeddedDirectory.h>
#include <Porkholt/IO/PHEmbeddedFile.h>
#include <Porkholt/Core/PHEmbeddedData.h>

PHEmbeddedDirectory::PHEmbeddedDirectory(const string & path) : PHDirectory(path)
{
    map<PHHashedString, pair<const char*, size_t> >::iterator i = PHEmbedFiles->find(PHHashedString(path));
    if (i == PHEmbedFiles->end())
        throw string("Embedded directory doesn't exist: " + path);
    if (i->second.first)
        throw string("Embedded item is not a directory: " + path);
}

PHInode * PHEmbeddedDirectory::itemAtPath(const string & s)
{
    string p = _path + "/" + s;
    map<PHHashedString, pair<const char*, size_t> >::iterator i = PHEmbedFiles->find(PHHashedString(p));
    if (i == PHEmbedFiles->end())
        throw string("no such embedded file or directory");
    PHInode * r;
    if (i->second.first)
        r = new PHEmbeddedFile(p);
    else
        r = new PHEmbeddedDirectory(p);
    r->autorelease();
    return r;
}

PHFile * PHEmbeddedDirectory::fileAtPath(const string & s)
{
    string p = _path + "/" + s;
    map<PHHashedString, pair<const char*, size_t> >::iterator i = PHEmbedFiles->find(PHHashedString(p));
    if (i == PHEmbedFiles->end())
        throw string("no such embedded file or directory");
    PHFile * r;
    if (i->second.first)
        r = new PHEmbeddedFile(p);
    else
        throw string("this embedded item is not a file");
    r->autorelease();
    return r;
}

PHDirectory * PHEmbeddedDirectory::directoryAtPath(const string & s)
{
    string p = _path + "/" + s;
    map<PHHashedString, pair<const char*, size_t> >::iterator i = PHEmbedFiles->find(PHHashedString(p));
    if (i == PHEmbedFiles->end())
        throw string("no such embedded file or directory");
    PHDirectory * r;
    if (i->second.first)
        throw string("this embedded item is not a directory");
    else
        r = new PHEmbeddedDirectory(p);
    r->autorelease();
    return r;
}

bool PHEmbeddedDirectory::itemExists(const string & s)
{
    string p = _path + "/" + s;
    map<PHHashedString, pair<const char*, size_t> >::iterator i = PHEmbedFiles->find(PHHashedString(p));
    return (i != PHEmbedFiles->end());
}

bool PHEmbeddedDirectory::fileExists(const string & s)
{
    string p = _path + "/" + s;
    map<PHHashedString, pair<const char*, size_t> >::iterator i = PHEmbedFiles->find(PHHashedString(p));
    return (i != PHEmbedFiles->end()) && i->second.first;
}

bool PHEmbeddedDirectory::directoryExists(const string & path)
{
    string p = _path + "/" + path;
    map<PHHashedString, pair<const char*, size_t> >::iterator i = PHEmbedFiles->find(PHHashedString(p));
    return (i != PHEmbedFiles->end()) && !i->second.first;
}

void PHEmbeddedDirectory::stat(const string & path, PHInode::stat_t & s)
{
    string p = _path + "/" + path;
    map<PHHashedString, pair<const char*, size_t> >::iterator i = PHEmbedFiles->find(PHHashedString(p));
    if (i == PHEmbedFiles->end())
    {
        s.type = stat_t::None;
        s.size = 0;
    } else {
        s.type = i->second.first ? stat_t::File : stat_t::Directory;
        s.size = i->second.second;
    }
}


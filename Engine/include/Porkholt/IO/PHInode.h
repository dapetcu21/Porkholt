/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHINODE_H
#define PHINODE_H
#include <Porkholt/Core/PHMain.h>

class PHDirectory;
class PHFile;
class PHInode : public PHObject
{
protected:
    string _path;
    size_t _base, _base_end, _ext;
public:
    const string & path() { return _path; }
    string name() { return _path.substr(_base); }
    string extension() { return _path.substr(_ext); }
    string basename() { return _path.substr(_base, _base_end-_base); }
    PHInode(const string & path);

    virtual bool isStream() { return false; }
    virtual bool isFile() { return false; }
    virtual bool isDirectory() { return false; }
    static PHInode * itemAtFSPath(const string & p);
    static PHFile * fileAtFSPath(const string & p);
    static PHDirectory * directoryAtFSPath(const string & p);
    static bool itemExistsOnFS(const string & p);
    static bool fileExistsOnFS(const string & p);
    static bool directoryExistsOnFS(const string & p);
};

#endif

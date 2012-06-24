/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHREGULARDIRECTORY_H
#define PHREGULARDIRECTORY_H
#include <Porkholt/IO/PHDirectory.h>

class PHRegularDirectory: public PHDirectory
{
    PHRegularDirectory(const string & dpath, int dummy);
    friend class PHInode;

public:
    PHRegularDirectory(const string & dpath);
    ~PHRegularDirectory();
    PHInode * itemAtPath(const string & s);
    PHFile * fileAtPath(const string & s);
    PHDirectory * directoryAtPath(const string & s);

    bool itemExists(const string & path);
    bool fileExists(const string & path);
    bool directoryExists(const string & path);
};

#endif

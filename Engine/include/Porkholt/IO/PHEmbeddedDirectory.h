/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHEMBEDDEDDIRECTORY_H
#define PHEMBEDDEDDIRECTORY_H

#include <Porkholt/IO/PHDirectory.h>

class PHEmbeddedDirectory : public PHDirectory 
{
    public:
        PHEmbeddedDirectory(const string & path);

        PHInode * itemAtPath(const string & s);
        PHFile * fileAtPath(const string & s);
        PHDirectory * directoryAtPath(const string & s);

        bool itemExists(const string & path);
        bool fileExists(const string & path);
        bool directoryExists(const string & path);

        void stat(const string & path, PHInode::stat_t & s);
};

#endif

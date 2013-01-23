/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHUNIONDIRECTORY_H
#define PHUNIONDIRECTORY_H

#include <Porkholt/IO/PHDirectory.h>

class PHUnionDirectory : public PHDirectory 
{
    protected:
        vector<PHDirectory*> items;
    public:
        PHUnionDirectory(const string & s, const vector<PHDirectory*> & items);
        PHUnionDirectory(const string & s);
        PHUnionDirectory();
        ~PHUnionDirectory();

        void addDirectory(PHDirectory * dir);
        const vector<PHDirectory*> & directories() { return items; }
        
        PHInode * itemAtPath(const string & s);
        PHFile * fileAtPath(const string & s);
        PHDirectory * directoryAtPath(const string & s);

        bool itemExists(const string & path);
        bool fileExists(const string & path);
        bool directoryExists(const string & path);

        void stat(const string & path, PHInode::stat_t & s);
};

#endif

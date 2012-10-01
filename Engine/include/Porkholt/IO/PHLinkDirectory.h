/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHLINKDIRECTORY_H
#define PHLINKDIRECTORY_H

#include <Porkholt/IO/PHDirectory.h>

class PHFile;

struct PHLinkDirectory_data;
class PHLinkDirectory : public PHDirectory
{
    protected:
        PHDirectory * root;
        PHLinkDirectory_data * d;
        string prefix;

        PHLinkDirectory(PHDirectory * dir, PHLinkDirectory_data * d, const string & prefix);
        
        void init(PHFile * symlinks);
        const string & _resolvedPath(const string & p);
    public:
        PHLinkDirectory(PHDirectory * dir, PHFile * symlinks);
        PHLinkDirectory(PHDirectory * dir);
        ~PHLinkDirectory();

        PHFile * fileAtPath(const string & s) { return root->fileAtPath(_resolvedPath(prefix+s)); }
        PHDirectory * directoryAtPath(const string & s);
        PHInode * itemAtPath(const string & s);

        bool itemExists(const string & s) { return root->itemExists(_resolvedPath(prefix+s)); }
        bool fileExists(const string & s) { return root->fileExists(_resolvedPath(prefix+s)); }
        bool directoryExists(const string & s) { return root->directoryExists(_resolvedPath(prefix+s)); }

        void stat(const string & s, PHInode::stat_t & st) { root->stat(_resolvedPath(prefix+s), st); }
};

#endif

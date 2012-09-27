/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHLINKDIRECTORY_H
#define PHLINKDIRECTORY_H

#include <Porkholt/IO/PHDirectory.h>

class PHFile;

class PHLinkDirectory : public PHDirectory
{
    protected:
        PHDirectory * root;
        map<PHHashedString, string> links;
        
        void init(PHFile * symlinks);
        const string & _resolvedPath(const string & p);
    public:
        PHLinkDirectory(PHDirectory * dir, PHFile * symlinks);
        PHLinkDirectory(PHDirectory * dir);
        ~PHLinkDirectory();

        string resolvedPath(const string & p) { return _resolvedPath(p); }

        PHInode * itemAtPath(const string & s) { return root->itemAtPath(_resolvedPath(s)); }
        PHFile * fileAtPath(const string & s) { return root->fileAtPath(_resolvedPath(s)); }
        PHDirectory * directoryAtPath(const string & s) { return root->directoryAtPath(_resolvedPath(s)); }

        bool itemExists(const string & s) { return root->itemExists(_resolvedPath(s)); }
        bool fileExists(const string & s) { return root->fileExists(_resolvedPath(s)); }
        bool directoryExists(const string & s) { return root->directoryExists(_resolvedPath(s)); }

};

#endif

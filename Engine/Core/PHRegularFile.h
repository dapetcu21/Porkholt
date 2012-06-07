/*
 *  PHRegularFile.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHREGULARFILE_H
#define PHREGULARFILE_H
#include "PHFile.h"

class PHRegularFile : public PHFile
{
protected:
    string path;
    int fd;
    PHRegularFile(const string & dpath, int dummy);
    friend class PHInode;

    void init();
public:
    void open(int flags);
    void close();
    
    size_t read(uint8_t * buf, size_t size);
    size_t write(const uint8_t * buf, size_t size);
    
    size_t pointer();
    void setPointer(size_t p);
    size_t size();
    void seek(ssize_t p);
    
    PHRegularFile(const string & dpath);
    ~PHRegularFile();
};

#endif

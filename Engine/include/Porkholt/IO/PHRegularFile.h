/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHREGULARFILE_H
#define PHREGULARFILE_H
#include <Porkholt/IO/PHFile.h>

class PHRegularFile : public PHFile
{
protected:
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

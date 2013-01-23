/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHEMBEDDEDFILE_H
#define PHEMBEDDEDFILE_H

#include <Porkholt/IO/PHFile.h>

class PHEmbeddedFile : public PHFile 
{
    protected:
        size_t _pointer, _size;
        const uint8_t * _data;
    public:
        PHEmbeddedFile(const string & path);

        size_t pointer() { return _pointer; }
        void setPointer(size_t p) { _pointer = p; }
        size_t size() { return _size; }
        size_t read(uint8_t * buf, size_t size);
        size_t write(const uint8_t * buf, size_t size);
        void open(int flags);
};

#endif

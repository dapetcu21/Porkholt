/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/IO/PHEmbeddedFile.h>
#include <Porkholt/Core/PHEmbeddedData.h>

PHEmbeddedFile::PHEmbeddedFile(const string & path) : PHFile(path), _pointer(0)
{
    map<PHHashedString, pair<const char*, size_t> >::iterator i = PHEmbedFiles->find(PHHashedString(path));
    if (i == PHEmbedFiles->end())
        throw string("Embedded file doesn't exist: " + path);
    _size = i->second.second;
    _data = (const uint8_t *) i->second.first;
    if (!_data)
        throw string("Embedded item is not a file: " + path);
}

void PHEmbeddedFile::open(int flags)
{
    PHFile::open(flags);
    _pointer = 0;
}

size_t PHEmbeddedFile::write(const uint8_t * buf, size_t size)
{
    throw string("Can't write to embedded files");
}

size_t PHEmbeddedFile::read(uint8_t * buf, size_t size)
{
    size_t reading_size = min(size, _size - _pointer);
    memcpy(buf, _data + _pointer, reading_size);
    _pointer += reading_size;
    return reading_size;
}

/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHWAVDECODER_H
#define PHWAVDECODER_H

#include <Porkholt/Sound/PHAsyncDecoder.h>

class PHWAVDecoder : public PHAsyncDecoder
{
protected:
    size_t doff;

    void _loadHeader();
    void _releaseFileHandle();
    void _loadData(uint8_t * data, size_t firstSample, size_t length);
};

#endif

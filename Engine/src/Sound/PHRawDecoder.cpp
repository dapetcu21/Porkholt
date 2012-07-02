/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Sound/PHRawDecoder.h>

PHRawDecoder::PHRawDecoder(uint8_t * buffer, size_t length, size_t frequency, int channels, enum bformat format): len(length), buf(buffer), count(1)
{
    freq = frequency;
    chan = channels;
    fmt = format;
}


PHRawDecoder::~PHRawDecoder()
{
    if (count)
        delete[] buf;
}

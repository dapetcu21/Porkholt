/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHRAWDECODER_H
#define PHRAWDECODER_H

#include <Porkholt/Sound/PHDecoder.h>

class PHRawDecoder : public PHDecoder
{
private:
    size_t len;
    uint8_t * buf;
    int count;
public:
    //memory managed by the decoder
    PHRawDecoder(uint8_t * buffer, size_t length, size_t frequency, int channels, enum bformat format);
    ~PHRawDecoder();

    uint8_t * dataForSampleRange(size_t firstSample, size_t length) { count++; return buf+firstSample*channels()*formatSampleLength(); }
    void releaseData(uint8_t * data) { if (!--count) delete[] buf; }
    void releaseStorage() { releaseData(NULL); }

    size_t length() { return len; }
};

#endif

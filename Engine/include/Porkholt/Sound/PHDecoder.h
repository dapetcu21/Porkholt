/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHDECODER_H
#define PHDECODER_H

#include <Porkholt/Core/PHMain.h>

class PHFile;

class PHDecoder : public PHObject
{
protected:
    int chan;
    size_t freq;
    int fmt;
    bool st;
public:
    int channels() { return chan; }
    size_t frequency() { return freq; }

    enum bformat {
        U8,
        U16,
        noFormats
    };
    enum bformat format() { return (enum bformat)fmt; }
    size_t formatSampleLength() { const size_t f[] = {1, 2}; return f[fmt]; }

    void setStatic(bool s) { st = s; }
    bool isStatic() { return st; }

    virtual void loadFromFile(PHFile * file) {};

    virtual size_t length() = 0; //in samples
    virtual size_t chunkLength() { if (st) return length(); return (200<<10)/chan/formatSampleLength(); }
    virtual uint8_t * dataForSampleRange(size_t firstSample, size_t length) = 0;
    virtual void releaseData(uint8_t * data) { delete[] data; }

    virtual void releaseStorage() = 0;
};

#endif

/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHAUDIOBUFFER_H
#define PHAUDIOBUFFER_H

#include <Porkholt/Sound/PHOpenAL.h>

class PHDecoder;

class PHAudioBuffer : public PHObject
{
private:
    vector<ALuint> buffers;
    vector<int> rcb;
    vector<ALuint> trash;
    int n;
    
    void retainBuffer(size_t index) { rcb[index]++; }
    void releaseBuffer(size_t index);

    ALuint dequeueNewBuffer();
    ALuint bufferForPart(size_t index);
    void throwAwayTrash();

    size_t bufferCount() { return n; };
    size_t sampleCount();
    size_t frequency();

    friend class PHSound;

    PHDecoder * decoder;


    void fillBuffer(ALuint b, size_t offset);


public:
    PHAudioBuffer(PHDecoder * dec);
    ~PHAudioBuffer();
};

#endif

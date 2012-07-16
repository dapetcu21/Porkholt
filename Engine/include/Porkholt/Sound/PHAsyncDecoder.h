/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHASYNCDECODER_H
#define PHASYNCDECODER_H

#include <Porkholt/Sound/PHDecoder.h>

class PHAsyncDecoder : public PHDecoder
{
protected:
    PHFile * file;
    size_t bps;
    size_t len;
    bool running;

    pair<size_t,size_t> last;

    PHMutex * mutex;
    
    list<pair<size_t,size_t> > taskQueue;
    map<pair<size_t,size_t>, uint8_t* > tasks;

    void processQueue();

    virtual void _loadHeader() = 0;
    virtual void _loadData(uint8_t * data, size_t firstSample, size_t length) = 0;
    virtual void _releaseFileHandle() = 0;

    PHThread * thread;
    PHSemaphore * sem;

    void threadFunc();

public:
    void loadFromFile(PHFile * file);

    void releaseData(uint8_t * data);
    bool prepareChunk(size_t firstSample, size_t length);
    uint8_t * dataForSampleRange(size_t firstSample, size_t length);

    size_t length() { return len; }
    void releaseStorage();

    PHAsyncDecoder();
    ~PHAsyncDecoder();
};

#endif

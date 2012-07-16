/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Sound/PHAsyncDecoder.h>
#include <Porkholt/IO/PHFile.h>
#include <Porkholt/Core/PHThread.h>

void PHAsyncDecoder::loadFromFile(PHFile * f)
{
    file = f;
    f->retain();
    _loadHeader();
    running = true;
    thread = new PHThread;
    thread->setFunction(PHInv(this, PHAsyncDecoder::threadFunc, NULL));
    thread->start();
}

void PHAsyncDecoder::threadFunc()
{
    do {
        processQueue();
        sem->wait();
    } while (running);
}

void PHAsyncDecoder::releaseData(uint8_t * data)
{
    size_t * v = (size_t*)data;
    size_t firstSample = v[-2], length = v[-1];
    mutex->lock();
    tasks.erase(make_pair(firstSample, length));
    mutex->unlock();
    delete[] (data - sizeof(size_t)*2);
}

void PHAsyncDecoder::releaseStorage()
{
    if (!file) return;
    mutex->lock();
    running = false;
    mutex->unlock();

    sem->signal();
    thread->join();
    _releaseFileHandle();
    taskQueue.clear();
    for(map<pair<size_t,size_t>, uint8_t* >::iterator i = tasks.begin(); i != tasks.end(); i++)
    {
        if (i->second)
        {
            delete[] (i->second - sizeof(size_t)*2);
        }
    }
    tasks.clear();
    file->release();
    file = NULL;
}

void PHAsyncDecoder::processQueue()
{
    mutex->lock();
    while (!taskQueue.empty())
    {
        pair<size_t,size_t> p = taskQueue.front(); 
        taskQueue.pop_front();
        mutex->unlock();
        uint8_t * data = new uint8_t[p.second * formatSampleLength() * channels() + 2*sizeof(size_t)];
        size_t * d = (size_t*)data;
        d[0] = p.first;
        d[1] = p.second;
        _loadData(data + sizeof(size_t)*2, p.first, p.second);
        mutex->lock();
        map<pair<size_t,size_t>, uint8_t* >::iterator i = tasks.find(p); 
        if (i != tasks.end())
            i->second = data + sizeof(size_t)*2;
        else
            delete[] data;
    }
    mutex->unlock();
}

bool PHAsyncDecoder::prepareChunk(size_t firstSample, size_t length)
{
    mutex->lock();
    pair<size_t,size_t> p = make_pair(firstSample, length);
    if (last != p && (p.first!=size_t(-1)))
    {
        map<pair<size_t,size_t>, uint8_t* >::iterator i = tasks.find(last);
        if (i!=tasks.end())
        {
            uint8_t * d = i->second;
            if (d)
                delete[] (d - sizeof(size_t)*2);
            tasks.erase(i);
        }
        last = p;
    }
    map<pair<size_t,size_t>, uint8_t* >::iterator i = tasks.find(p);
    if (i == tasks.end())
    {
        taskQueue.push_back(p);
        tasks.insert(make_pair(p, (uint8_t*)NULL));
        mutex->unlock();
        sem->signal();
        return false;
    }
    bool r = (i->second != NULL);
    mutex->unlock();
    return r;
}

uint8_t * PHAsyncDecoder::dataForSampleRange(size_t firstSample, size_t length)
{
    mutex->lock();
    map<pair<size_t,size_t>, uint8_t* >::iterator i = tasks.find(make_pair(firstSample, length));
    if (i == tasks.end())
    {
        mutex->unlock();
        return NULL;
    } 
    uint8_t * r = i->second;
    mutex->unlock();
    return r;
}

PHAsyncDecoder::PHAsyncDecoder() : mutex(new PHMutex(true)), sem(new PHSemaphore(0)), last(-1, -1)
{
}

PHAsyncDecoder::~PHAsyncDecoder()
{
    releaseStorage();
    sem->release();
}

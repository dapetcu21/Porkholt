/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Sound/PHAudioBuffer.h>
#include <Porkholt/Sound/PHDecoder.h>

PHAudioBuffer::PHAudioBuffer(PHDecoder * dec) : decoder(dec)
{
    n = ((decoder->length()-1)/decoder->chunkLength())+1; 
    buffers.resize(n);
    rcb.resize(n);
    if (n==1) 
    {
        if (decoder->prepareChunk(0, decoder->length()))
            bufferForPart(0);
    }
    decoder->retain();
}

void PHAudioBuffer::fillBuffer(ALuint b, size_t offset)
{
    ALenum format;
    static const ALenum fmts [][PHDecoder::noFormats] = {
        {0, 0},
        {AL_FORMAT_MONO8, AL_FORMAT_MONO16}, 
        {AL_FORMAT_STEREO8, AL_FORMAT_STEREO16}
        };
    size_t samples = min(decoder->chunkLength(), decoder->length() - offset);
    uint8_t * buf = decoder->dataForSampleRange(offset, samples);
    alBufferData(b, fmts[decoder->channels()][decoder->format()], buf, samples*decoder->channels()*decoder->formatSampleLength(), decoder->frequency());
    decoder->releaseData(buf);
}

size_t PHAudioBuffer::sampleCount()
{
    return decoder->length();
}

size_t PHAudioBuffer::frequency()
{
    return decoder->frequency();
}

size_t PHAudioBuffer::bufferLength()
{
    return decoder->chunkLength();
}

bool PHAudioBuffer::prepareBuffer(size_t index)
{
    if (buffers[index]) return true;
    size_t chunk = decoder->chunkLength();
    size_t offset = index * chunk;
    return decoder->prepareChunk(offset, min(chunk, decoder->length() - offset));
}

void PHAudioBuffer::releaseBuffer(size_t index)
{
    if (n==1) return;
    if (!(--rcb[index]))
    {
        trash.push_back(buffers[index]);
        buffers[index] = 0;
    }
}

ALuint PHAudioBuffer::dequeueNewBuffer()
{
    ALuint b;
    if (trash.empty())
        alGenBuffers(1, &b);
    else
    {
        b = trash.back();
        trash.pop_back();
    }
    return b;
}

ALuint PHAudioBuffer::bufferForPart(size_t index)
{
    if (!buffers[index])
    {
        ALuint b = dequeueNewBuffer();
        fillBuffer(b, index * decoder->chunkLength());
        buffers[index] = b;
        rcb[index] = 0;
        if (n==1)
            decoder->releaseStorage();
    }
    rcb[index]++;
    return buffers[index];
}

void PHAudioBuffer::throwAwayTrash()
{
    if (!trash.empty())
    {
        alDeleteBuffers(trash.size(), &trash[0]);
        trash.clear();
    }
}

PHAudioBuffer::~PHAudioBuffer()
{
    trash.insert(trash.end(), buffers.begin(), buffers.end());
    throwAwayTrash();
    decoder->release();
}

/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Sound/PHOpenAL.h>

class PHDecoder;

class PHAudioBuffer : public PHObject
{
private:
    map<size_t, ALuint> buffers;
    list<ALuint> trash;

    void collectTrashBuffers();
    ALuint dequeueNewBuffer();
    ALuint bufferForPart(size_t index);
    void throwAwayTrash();

    size_t bufferCount();

    friend class PHSound;

    PHDecoder * decoder;

public:
    PHAudioBuffer(PHDecoder * dec);
    ~PHAudioBuffer();
};

/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Sound/PHSound.h>
#include <Porkholt/Sound/PHAudioBuffer.h>
#include <Porkholt/Sound/PHSoundManager.h>

PHSound::PHSound(PHSoundManager * mn) : buf(NULL), man(mn), st(false), stack_begin(0), schseek(false), playing(false)
{
    alGenSources(1, &id); 
}

PHSound::~PHSound()
{
    clearBuffer();
    if (buf)
        buf->release();
    alDeleteSources(1, &id);
}

void PHSound::unqueue(size_t size)
{
    if (!size) return;
    ALuint * v = new ALuint[size];
    for (size_t i = 0; i<size; i++)
    {
        v[i] = stack.front();
        stack.pop_front();
    }
    stack_begin += size;
    alSourceUnqueueBuffers(id, size, v);
    delete [] v;
}

void PHSound::update()
{
    if (st)
    {
        if (stack.empty())
        {
            if (buf->prepareBuffer(0))
            {
                ALuint b = buf->bufferForPart(0);
                alSourcei(id, AL_BUFFER, b);
                if (schseek)
                {
                    alSourcef(id, AL_SAMPLE_OFFSET, seekv);
                    schseek = false;
                }
                if (playing)
                    alSourcePlay(id);
            }
        } else
            if (schseek)
            {
                alSourcef(id, AL_SAMPLE_OFFSET, seekv);
            }
    } else {
        size_t chunk = buf->bufferLength();
        if (schseek)
        {
            size_t seekb = seekv / chunk;
            if (seekb>=stack_begin && seekb<stack_begin+stack.size())
            {
                alSourcef(id, AL_SAMPLE_OFFSET, seekv - seekb * chunk);
                schseek = false;
            }
            else
                unqueue(stack.size());
        }
        ALint off;
        if (schseek)
            off = seekv / chunk;
        else 
            alGetSourcei(id, AL_SAMPLE_OFFSET, &off);
        size_t crrb = off / chunk;
        if (crrb>=stack_begin && crrb<stack_begin+stack.size())
        {
            unqueue(crrb-stack_begin);
            if ((crrb != buf->bufferCount()+1) && (crrb = stack_begin+stack.size()-1))
            {
                if (buf->prepareBuffer(crrb+1))
                {
                    ALuint b = buf->bufferForPart(crrb+1);
                    alSourceQueueBuffers(id, 1, &b);
                    stack.push_back(b);
                }
            }
        } else {
            alSourceStop(id);
            unqueue(stack.size());
            if (buf->prepareBuffer(crrb))
            {
                ALuint b = buf->bufferForPart(crrb);
                alSourceQueueBuffers(id, 1, &b);
                stack.push_back(b);
                stack_begin = crrb;

                if (schseek)
                {
                    alSourcef(id, AL_SAMPLE_OFFSET, seekv - stack_begin * chunk);
                    schseek = false;
                }
            }
        }

        if (playing)
        {
            ALint state;
            alGetSourcei(id, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING)
                alSourcePlay(id);
        }
    }
}

void PHSound::clearBuffer()
{
    if (buf)
    {
        alSourceStop(id);
        if (!st)
            unqueue(stack.size());
        alSourcei(id, AL_BUFFER, 0);
    }
    stack_begin = 0;
}

void PHSound::setBuffer(PHAudioBuffer * buff)
{
    clearBuffer();
    buff->retain();
    buf->release();
    buf = buff;
    st = (buf->bufferCount() == 1);
    buf->prepareBuffer(0);
}

void PHSound::play()
{
    if (playing) return;
    playing = true;
    update();
}

void PHSound::pause()
{
    if (!playing) return;
    playing = false;
    alSourcePause(id);
}

void PHSound::stop()
{
    pause();
    seek(0);
}

bool PHSound::isPlaying()
{
    return playing;
}


ph_float PHSound::duration()
{
    if (!buf) return 0;
    return buf->sampleCount()/ph_float(buf->frequency());
}

ph_float PHSound::playPosition()
{
    ALfloat v;
    alGetSourcef(id, AL_SEC_OFFSET, &v);
    return ph_float(v)+stack_begin*buf->bufferLength()/ph_float(buf->frequency());
}

void PHSound::seek(ph_float pos)
{
    schseek = true;
    seekv = pos;
    update();
}


ph_float PHSound::pitch()
{
    ALfloat v;
    alGetSourcef(id, AL_PITCH, &v);
    return ph_float(v);
}

ph_float PHSound::gain()
{
    ALfloat v;
    alGetSourcef(id, AL_GAIN, &v);
    return ph_float(v);
}

ph_float PHSound::minGain()
{
    ALfloat v;
    alGetSourcef(id, AL_MIN_GAIN, &v);
    return ph_float(v);
}

ph_float PHSound::maxGain()
{
    ALfloat v;
    alGetSourcef(id, AL_MAX_GAIN, &v);
    return ph_float(v);
}

ph_float PHSound::maxDistance()
{
    ALfloat v;
    alGetSourcef(id, AL_MAX_DISTANCE, &v);
    return ph_float(v);
}

ph_float PHSound::rolloffFactor()
{
    ALfloat v;
    alGetSourcef(id, AL_ROLLOFF_FACTOR, &v);
    return ph_float(v);
}

ph_float PHSound::coneOuterGain()
{
    ALfloat v;
    alGetSourcef(id, AL_CONE_OUTER_GAIN, &v);
    return ph_float(v);
}

ph_float PHSound::coneInnerAngle()
{
    ALfloat v;
    alGetSourcef(id, AL_CONE_INNER_ANGLE, &v);
    return ph_float(v);
}

ph_float PHSound::coneOuterAngle()
{
    ALfloat v;
    alGetSourcef(id, AL_CONE_OUTER_ANGLE, &v);
    return ph_float(v);
}

ph_float PHSound::referenceDistance()
{
    ALfloat v;
    alGetSourcef(id, AL_REFERENCE_DISTANCE, &v);
    return ph_float(v);
}


void PHSound::setPitch(ph_float val)
{
    alSourcef(id, AL_PITCH, ALfloat(val));
}

void PHSound::setGain(ph_float val)
{
    alSourcef(id, AL_GAIN, ALfloat(val));
}

void PHSound::setMinGain(ph_float val)
{
    alSourcef(id, AL_MIN_GAIN, ALfloat(val));
}

void PHSound::setMaxGain(ph_float val)
{
    alSourcef(id, AL_MAX_GAIN, ALfloat(val));
}

void PHSound::setMaxDistance(ph_float val)
{
    alSourcef(id, AL_MAX_DISTANCE, ALfloat(val));
}

void PHSound::setRolloffFactor(ph_float val)
{
    alSourcef(id, AL_ROLLOFF_FACTOR, ALfloat(val));
}

void PHSound::setConeOuterGain(ph_float val)
{
    alSourcef(id, AL_CONE_OUTER_GAIN, ALfloat(val));
}

void PHSound::setConeInnerAngle(ph_float val)
{
    alSourcef(id, AL_CONE_INNER_ANGLE, ALfloat(val));
}

void PHSound::setConeOuterAngle(ph_float val)
{
    alSourcef(id, AL_CONE_OUTER_ANGLE, ALfloat(val));
}

void PHSound::setReferenceDistance(ph_float val)
{
    alSourcef(id, AL_REFERENCE_DISTANCE, ALfloat(val));
}


PHVector3 PHSound::position()
{
    ALfloat v[3];
    alGetSourcefv(id, AL_POSITION, v);
    return PHVector3(ph_float(v[0]), ph_float(v[1]), ph_float(v[2]));
}

PHVector3 PHSound::velocity()
{
    ALfloat v[3];
    alGetSourcefv(id, AL_VELOCITY, v);
    return PHVector3(ph_float(v[0]), ph_float(v[1]), ph_float(v[2]));
}

PHVector3 PHSound::direction()
{
    ALfloat v[3];
    alGetSourcefv(id, AL_DIRECTION, v);
    return PHVector3(ph_float(v[0]), ph_float(v[1]), ph_float(v[2]));
}


void PHSound::setPosition(const PHVector3 & v)
{
    alSource3f(id, AL_POSITION, ALfloat(v.x), ALfloat(v.y), ALfloat(v.x));
}

void PHSound::setVelocity(const PHVector3 & v)
{
    alSource3f(id, AL_VELOCITY, ALfloat(v.x), ALfloat(v.y), ALfloat(v.x));
}

void PHSound::setDirection(const PHVector3 & v)
{
    alSource3f(id, AL_DIRECTION, ALfloat(v.x), ALfloat(v.y), ALfloat(v.x));
}


bool PHSound::relativePositions()
{
    ALint v;
    alGetSourcei(id, AL_SOURCE_RELATIVE, &v);
    return (v == AL_TRUE);
}

void PHSound::setRelativePositions(bool r)
{
    alSourcei(id, AL_SOURCE_RELATIVE, r?AL_TRUE:AL_FALSE);
}

bool PHSound::looping()
{
    ALint v;
    alGetSourcei(id, AL_LOOPING, &v);
    return (v == AL_TRUE);
}

void PHSound::setLooping(bool l)
{
   alSourcei(id, AL_LOOPING, l?AL_TRUE:AL_FALSE);
}




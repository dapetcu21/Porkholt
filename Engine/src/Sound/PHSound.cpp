/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Sound/PHSound.h>
#include <Porkholt/Sound/PHAudioBuffer.h>
#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/Sound/PHSoundPool.h>

PHSound::PHSound(PHSoundManager * mn) : buf(NULL), man(mn), st(false), stack_begin(0), schseek(false), playing(false), loop(false), seekv(0), initial(true), _tag(0)
{
    alGenSources(1, &id); 
    man->addSound(this);
}

void PHSound::setCallback(const PHInvocation & invo)
{
    inv = invo;
}

void PHSound::playFading()
{
    play();
}

void PHSound::pauseFading()
{
    //this retains
    pause();
}

PHSound * PHSound::copy()
{
    PHSound * snd = new PHSound(man);
    snd->setBuffer(buf);
    return snd;
}

PHSound::~PHSound()
{
    clearBuffer();
    if (buf)
        buf->release();
    alDeleteSources(1, &id);
    man->removeSound(this);
    set<PHSoundPool*> sp = soundPool;
    for (set<PHSoundPool*>::iterator i = sp.begin(); i != sp.end(); i++)
        (*i)->removeSound(this);
}

void PHSound::songEnded()
{
    PHLog("songEnded");
    inv.call(this);
}

void PHSound::addSoundPool(PHSoundPool * sp)
{
    soundPool.insert(sp);
}

void PHSound::removeSoundPool(PHSoundPool * sp)
{
    soundPool.erase(sp);
}

void PHSound::unqueue(size_t size)
{
    if (!size) return;
    ALuint * v = new ALuint[size];
    size_t * b = new size_t[size];
    size_t n = buf->bufferCount();
    for (size_t i = 0; i<size; i++, ( ((++stack_begin) == n)?(stack_begin=0):0 ))
    {
        v[i] = stack.front();
        stack.pop_front();
        b[i] = stack_begin;
    }

    ALint processed;
    alGetSourcei(id, AL_BUFFERS_PROCESSED, &processed);
    if (processed != size)
    {
        PHLog("Something odd here. processed (%d) != size (%d)", int(processed), int(size));
    }

    ALenum err = alGetError();
    alSourceUnqueueBuffers(id, size, v);
    if ((err = alGetError()) != AL_NO_ERROR)
    {
        PHLog("Could not unqueue buffers. Odd: %s", alGetString(err));
    }
    
    for(size_t i = 0; i<size; i++)
    {
        buf->releaseBuffer(b[i]);
        PHLog("release %d", b[i]);
    }

    delete [] v;
    delete [] b;
}

static inline bool _instack(size_t s, size_t n, size_t a, size_t b)
{
    if (b>n)
        return (s>=a || s<b-n);
    else
        return (s>=a && s<b);
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
                stack.push_back(b);
            }
        } else {
            if (schseek)
            {
                alSourcef(id, AL_SAMPLE_OFFSET, seekv);
                schseek = false;
            }
        }
    } else {
        size_t chunk = buf->bufferLength();
        size_t n = buf->bufferCount();
#define instack(x) _instack(x, n, stack_begin, stack_begin+stack.size())
        if (schseek)
        {
            size_t seekb = seekv / chunk;

            if (instack(seekb))
            {
                ssize_t s = ssize_t(seekv) - stack_begin * chunk;
                if (s<0)
                    s += buf->sampleCount();
                alSourcef(id, AL_SAMPLE_OFFSET, s);
                schseek = false;
            }
        }

        size_t off = playPositionSample(); 
        size_t crrb = off / chunk;

        if (instack(crrb))
        {
            ssize_t s = ssize_t(crrb) - stack_begin;
            if (s<0)
                s+=n;
            unqueue(s);
            if (( loop || (crrb != buf->n-1) ) && (crrb == stack_begin+stack.size()-1))
            {
                size_t nb = crrb+1;
                if (nb == n)
                    nb = 0;
                if (buf->prepareBuffer(nb) && !initial)
                {
                    ALuint b = buf->bufferForPart(nb);
                    alSourceQueueBuffers(id, 1, &b);
                    stack.push_back(b);
                    PHLog("buffer %d (+1)", nb);
                }
            }
        } else {
            alSourceStop(id);
            if (!schplaying && (off != 0))
                schplaying = true;
            unqueue(stack.size());
            if (buf->prepareBuffer(crrb) && !initial)
            {
                ALuint b = buf->bufferForPart(crrb);
                alSourceQueueBuffers(id, 1, &b);
                stack.push_back(b);
                stack_begin = crrb;
                PHLog("buffer %d", crrb);

                if (schseek)
                { 
                    alSourcef(id, AL_SAMPLE_OFFSET, seekv - stack_begin * chunk);
                    schseek = false;
                }
            }
        }
    }
    buf->throwAwayTrash();

    if (!stack.empty())
    {
        ALint state;
        alGetSourcei(id, AL_SOURCE_STATE, &state);
        if (schplaying)
        {
            if (playing)
            {
                if (state != AL_PLAYING)
                {
                    alSourcePlay(id);
                    state = AL_PLAYING;
                }
            } else {
                if (state == AL_PLAYING)
                {
                    alSourcePause(id);
                    state = AL_PAUSED;
                }
            }
            schplaying = false;
        }
        if (playing && (state == AL_STOPPED))
        {
            retain();
            songEnded();
            bool p = playing;
            stop();
            if (p && loop)
                play();
            release();
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
    if (buff)
        buff->retain();
    if (buf)
        buf->release();
    buf = buff;
    st = (buf->bufferCount() == 1);
    buf->prepareBuffer(0);
}

void PHSound::play()
{
    if (isPlaying()) return;
    playing = true;
    schplaying = true;
    initial = false;
    update();
}

void PHSound::pause()
{
    playing = false;
    schplaying = false;
    if (!isPlaying()) return;
    alSourcePause(id);
    seekv = playPositionSample();
}

void PHSound::stop()
{
    pause();
    seek(0);
}

bool PHSound::isPlaying()
{
    if (schplaying)
        return playing;
    ALint state;
    alGetSourcei(id, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
}

ph_float PHSound::duration()
{
    if (!buf) return 0;
    return buf->sampleCount()/ph_float(buf->frequency());
}

size_t PHSound::playPositionSample(ALenum state)
{
    if (schseek)
        return seekv;
    if (state == -1)
        alGetSourcei(id, AL_SOURCE_STATE, &state);
    if (state == AL_INITIAL)
        return seekv;
    if (state == AL_STOPPED)
    {
        if (playing)
        {
            if (schplaying)
                return stack_begin * buf->bufferLength();
            size_t n = buf->bufferCount();
            size_t i = stack_begin + stack.size();
            while(i >= n)
                i-= n;
            return i * buf->bufferLength();
        } else 
            return seekv;
    }
    if (schseek || (alGetSourcei(id, AL_SOURCE_STATE, &state), ((state == AL_INITIAL) || (state == AL_INITIAL))))
        return seekv;
    ALint v;
    alGetSourcei(id, AL_SAMPLE_OFFSET, &v);
    size_t chk = buf->bufferLength();
    size_t off = v + stack_begin * chk;
    size_t n = buf->sampleCount();
    if (off >= n)
        off -= n;
    return off;
}

ph_float PHSound::playPosition()
{
       return playPositionSample()/ph_float(buf->frequency());
}

void PHSound::seek(ph_float pos)
{
    schseek = true;
    seekv = pos*buf->frequency();
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
    return loop;
}

void PHSound::setLooping(bool l)
{
    if (st)
        alSourcei(id, AL_LOOPING, l?AL_TRUE:AL_FALSE);
    loop = l;
}




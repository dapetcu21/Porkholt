/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Sound/PHSound.h>
#include <Porkholt/Sound/PHAudioBuffer.h>
#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/Sound/PHSoundPool.h>

PHSound::PHSound(PHSoundManager * mn) : buf(NULL), man(mn), st(false), stack_begin(0), schseek(false), playing(false), loop(false), seekv(0), initial(true), _tag(0), schplaying(false),

_pitch(1.0f), _gain(1.0f), _minGain(0.0f), _maxGain(1.0f), _maxDistance(FLT_MAX), _rolloffFactor(1.0f), _coneOuterGain(0.0f), _coneInnerAngle(360.0f), _coneOuterAngle(360.0f), _position(0,0,0), _velocity(0,0,0), _direction(0,0,0), _relativePositions(false)
{
    alGenSources(1, &id); 
    man->addSound(this);
    actorAttachedToGameManager(man->gameManager());
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


void PHSound::setPitch(ph_float val)
{
    _pitch = val;
    alSourcef(id, AL_PITCH, ALfloat(val));
}

void PHSound::setGain(ph_float val)
{
    _gain = val;
    alSourcef(id, AL_GAIN, ALfloat(val));
}

void PHSound::setMinGain(ph_float val)
{
    _minGain = val;
    alSourcef(id, AL_MIN_GAIN, ALfloat(val));
}

void PHSound::setMaxGain(ph_float val)
{
    _maxGain = val;
    alSourcef(id, AL_MAX_GAIN, ALfloat(val));
}

void PHSound::setMaxDistance(ph_float val)
{
    _maxDistance = val;
    alSourcef(id, AL_MAX_DISTANCE, ALfloat(val));
}

void PHSound::setRolloffFactor(ph_float val)
{
    _rolloffFactor = val;
    alSourcef(id, AL_ROLLOFF_FACTOR, ALfloat(val));
}

void PHSound::setConeOuterGain(ph_float val)
{
    _coneOuterGain = val;
    alSourcef(id, AL_CONE_OUTER_GAIN, ALfloat(val));
}

void PHSound::setConeInnerAngle(ph_float val)
{
    _coneInnerAngle = val;
    alSourcef(id, AL_CONE_INNER_ANGLE, ALfloat(val));
}

void PHSound::setConeOuterAngle(ph_float val)
{
    _coneOuterAngle = val;
    alSourcef(id, AL_CONE_OUTER_ANGLE, ALfloat(val));
}

void PHSound::setReferenceDistance(ph_float val)
{
    _referenceDistance = val;
    alSourcef(id, AL_REFERENCE_DISTANCE, ALfloat(val));
}


void PHSound::setPosition(const PHVector3 & v)
{
    _position = v;
    alSource3f(id, AL_POSITION, ALfloat(v.x), ALfloat(v.y), ALfloat(v.x));
}

void PHSound::setVelocity(const PHVector3 & v)
{
    _velocity = v;
    alSource3f(id, AL_VELOCITY, ALfloat(v.x), ALfloat(v.y), ALfloat(v.x));
}

void PHSound::setDirection(const PHVector3 & v)
{
    _direction = v;
    alSource3f(id, AL_DIRECTION, ALfloat(v.x), ALfloat(v.y), ALfloat(v.x));
}

void PHSound::setRelativePositions(bool r)
{
    _relativePositions = r;
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

void PHSound::setAnimationFieldF(int field, ph_float value)
{
    switch (field)
    {
        case PHCinematicActor::fieldPitch:
            setPitch(value);
            break;
        case PHCinematicActor::fieldGain:
            setGain(value);
            break;
        default:
            break;
    }
}

void PHSound::setAnimationFieldV3(int field, const PHVector3 & value)
{
    switch (field)
    {
        case PHCinematicActor::fieldMove:
            setPosition(value);
            break;
        case PHCinematicActor::fieldVelocity:
            setVelocity(value);
            break;
    }
}

void PHSound::setAnimationFieldQ(int field, const PHQuaternion & value)
{
    switch (field)
    {
        case PHCinematicActor::fieldDirection:
            setDirection(value * PHVector3(0, 0, 1));
            break;
        default:
            break;
    }
}

ph_float PHSound::getAnimationFieldF(int field)
{
    switch (field)
    {
        case PHCinematicActor::fieldPitch:
            return pitch();
        case PHCinematicActor::fieldGain:
            return gain();
        default:
            return 0;
    }
}

PHVector3 PHSound::getAnimationFieldV3(int field)
{
    switch (field)
    {
        case PHCinematicActor::fieldMove:
            return position();
        case PHCinematicActor::fieldVelocity:
            return velocity();
        default:
            return PH3DOriginPoint;
    }
}

PHQuaternion PHSound::getAnimationFieldQ(int field)
{
    switch (field)
    {
        case PHCinematicActor::fieldDirection:
            return PHQuaternion::fromPointsOnSphere(PHVector3(0, 0, 1), direction());
        default:
            return PHIdentityQuaternion;
    }
}




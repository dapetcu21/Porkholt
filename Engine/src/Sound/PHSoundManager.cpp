/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/Sound/PHOpenAL.h>
#include <Porkholt/Sound/PHSound.h>
#include <Porkholt/Sound/PHAudioBuffer.h>
#include <Porkholt/Sound/PHDecoder.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/IO/PHFile.h>
#include <Porkholt/Sound/PHWAVDecoder.h>

map<string, PHAllocator> * PHSoundManager::extensions = NULL;
bool PHSoundManager::plugLoaded = false;

void PHSoundManager::setBackgroundMusic(PHSound * m)
{
    if (m == music) return;
    if (music)
    {
        music->pauseFading();
        music->release();
    }
    music = m;
    if (music)
    {
        music->retain();
        music->stop();
        music->playFading();
    }
}

void PHSoundManager::addSound(PHSound * snd)
{
    if (!inside)
        allsounds.insert(snd);
    else
    {
        insertlist.insert(snd);
        deletelist.erase(snd);
    }
}

void PHSoundManager::removeSound(PHSound * snd)
{
    if (!inside)
        allsounds.erase(snd);
    else
    {
        deletelist.insert(snd);
        insertlist.erase(snd);
    }
}

void PHSoundManager::advanceAnimation(ph_float elapsed)
{
    inside = true;
    set<PHSound*>::iterator i;
    for (i = allsounds.begin(); i!= allsounds.end(); i++)
    {
        PHSound * s = *i;
        if (deletelist.count(s))
            continue;
        s->update();
    }
    for (i = deletelist.begin(); i!= deletelist.end(); i++)
        allsounds.erase(*i);
    allsounds.insert(insertlist.begin(), insertlist.end());
    deletelist.clear();
    insertlist.clear();
    inside = false;
}

void PHSoundManager::loadPlugins()
{
    if (plugLoaded) return;
    plugLoaded = true;

    registerPlugin("wav", PHAlloc<PHWAVDecoder>);
    registerPlugin("wave", PHAlloc<PHWAVDecoder>);
}

void PHSoundManager::registerPlugin(const string & ext, PHAllocator a)
{
    if (!extensions)
        extensions = new map<string, PHAllocator>;

    extensions->insert(make_pair<string, PHAllocator>(ext, a));
}

PHSoundManager::PHSoundManager(PHDirectory * dir) : sndDir(dir), inside(false), music(NULL)
{
    sndDir->retain();
#ifdef PH_DEBUG
    const char * s = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
    PHLog("Available OpenAL devices:");
    while (s && s[0])
    {
        PHLog("\"%s\"", s);
        s += strlen(s)+1;
    }
#endif
    device = (void*)alcOpenDevice(NULL);
    if (device)
    {
        context = (void*)alcCreateContext((ALCdevice*)device, NULL); 
        if (context)
            alcMakeContextCurrent((ALCcontext*)context);
    }
    loadPlugins();
}

PHSound * PHSoundManager::soundNamed(const string & name, PHDirectory * dir)
{
    PHHashedString n(dir->path() + "/" + name);
    map<PHHashedString, PHSound *>::iterator it = sounds.find(n);
    if (it == sounds.end())
    {
        if (!extensions || extensions->empty())
        {
            string err("No decoder plugins loaded");
            PHLog(err);
            throw err;
        }
        for (map<string, PHAllocator>::iterator i = extensions->begin(); i!=extensions->end(); i++)
        {
            string enm = name + "." + i->first;
            if (dir->fileExists(enm))
            {
                PHAutoreleasePool ap;
                PHDecoder * decoder = NULL;
                try {
                    decoder = (PHDecoder*)(i->second)();
                    decoder->loadFromFile(dir->fileAtPath(enm));
                } catch (string ex) {
                    PHLog("Can't load \"%s\": %s",enm.c_str(), ex.c_str());
                    if (decoder)
                        decoder->release();
                    throw;
                }
                PHAudioBuffer * buf = new PHAudioBuffer(decoder);
                decoder->release();
                PHSound * snd = new PHSound(this);
                snd->setBuffer(buf);
                buf->release();
                sounds.insert(make_pair(n, snd));
                return snd;
            }
        }
        return NULL;
    }
    return it->second;
}

PHSoundManager::~PHSoundManager()
{
    sndDir->release();
    if (context)
    {
       alcMakeContextCurrent(NULL);
       alcDestroyContext((ALCcontext*)context);
    }
    if (device)
        alcCloseDevice((ALCdevice*)device);
}

void PHSoundManager::setGain(ph_float gain)
{
    alListenerf(AL_GAIN, ALfloat(gain));  
}

ph_float PHSoundManager::gain()
{
    ALfloat v;
    alGetListenerf(AL_GAIN, &v);
    return ph_float(v);
}

void PHSoundManager::setPositiion(const PHVector3 & p)
{
    alListener3f(AL_POSITION, ALfloat(p.x), ALfloat(p.y), ALfloat(p.z));
}

PHVector3 PHSoundManager::position()
{
    ALfloat v[3];
    alGetListenerfv(AL_POSITION, v);
    return PHVector3(ph_float(v[0]), ph_float(v[1]), ph_float(v[2]));
}

void PHSoundManager::setVelocity(const PHVector3 & p)
{
    alListener3f(AL_VELOCITY, ALfloat(p.x), ALfloat(p.y), ALfloat(p.z));
}

PHVector3 PHSoundManager::velocity()
{
    ALfloat v[3];
    alGetListenerfv(AL_VELOCITY, v);
    return PHVector3(ph_float(v[0]), ph_float(v[1]), ph_float(v[2]));
}

void PHSoundManager::setOrientation(const PHQuaternion & p)
{
    PHMatrix m = p.normalized().rotationMatrix();
    setOrientation(m.transformPoint(PH3DPoint(0,0,-1)),
                   m.transformPoint(PH3DPoint(0,1,0))); 
}

void PHSoundManager::setOrientation(const PHVector3 & at, const PHVector3 & up)
{
    ALfloat v[6] = { at.x, at.y, at.z, up.x, up.y, up.z };
    alListenerfv(AL_ORIENTATION, v);
}

pair<PHVector3, PHVector3> PHSoundManager::orientation()
{
    ALfloat v[6];
    alGetListenerfv(AL_ORIENTATION, v);
    return make_pair(PHVector3(v[0], v[1], v[2]), PHVector3(v[3], v[4], v[5]));
}

ph_float PHSoundManager::dopplerFactor()
{
    return alGetFloat(AL_DOPPLER_FACTOR);
}

void PHSoundManager::setDopplerFactor(ph_float v)
{
    alDopplerFactor(ALfloat(v));
}

ph_float PHSoundManager::speedOfSound()
{
    return alGetFloat(AL_SPEED_OF_SOUND);
}

void PHSoundManager::setSpeedOfSound(ph_float v)
{
    alSpeedOfSound(v);
}

enum PHSoundManager::distModel PHSoundManager::distanceModel()
{
    ALenum m = alGetInteger(AL_DISTANCE_MODEL);
    switch (m)
    {
        case AL_INVERSE_DISTANCE:
            return inverseDistance;
        case AL_INVERSE_DISTANCE_CLAMPED:
            return inverseDistanceClamped;
        case AL_LINEAR_DISTANCE:
            return linearDistance;
        case AL_LINEAR_DISTANCE_CLAMPED:
            return linearDistanceClamped;
        case AL_EXPONENT_DISTANCE:
            return exponentDistance;
        case AL_EXPONENT_DISTANCE_CLAMPED:
            return exponentDistanceClamped;
        default:
            return none;
    }
}
void PHSoundManager::setDistanceModel(enum distModel model)
{
    static const ALenum v[] = {AL_INVERSE_DISTANCE, AL_INVERSE_DISTANCE_CLAMPED, AL_LINEAR_DISTANCE, AL_LINEAR_DISTANCE_CLAMPED, AL_EXPONENT_DISTANCE, AL_EXPONENT_DISTANCE_CLAMPED, AL_NONE};
    alDistanceModel(v[int(model)]);
}


/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

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

PHSoundManager::PHSoundManager(PHDirectory * dir) : sndDir(dir), inside(false)
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
                PHFile * file = NULL;
                PHDecoder * decoder = NULL;
                try {
                    decoder = (PHDecoder*)(i->second)();
                    file = dir->fileAtPath(enm);
                    decoder->loadFromFile(file);
                    file->release();
                    file = NULL;
                } catch (string ex) {
                    PHLog("Can't load \"%s\": %s",enm.c_str(), ex.c_str());
                    if (file)
                        file->release();
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
        throw string("No match found for ") + name;
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

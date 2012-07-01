/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/Sound/PHOpenAL.h>
#include <Porkholt/Sound/PHSound.h>
#include <Porkholt/Sound/PHAudioBuffer.h>
#include <Porkholt/Sound/PHDecoder.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/IO/PHFile.h>

map<string, PHAllocator> * PHSoundManager::extensions = NULL;

PHSoundManager::PHSoundManager(PHDirectory * dir) : sndDir(dir)
{
    sndDir->retain();
    device = alcOpenDevice(NULL);
    if (device)
    {
        context = alcCreateContext(device, NULL); 
    }
}

PHSound * PHSoundManager::soundNamed(const string & name, PHDirectory * dir)
{
    PHHashedString n(dir->path + "/" + name);
    map<PHHashedString, PHSound *>::iterator it = sounds.find(n);
    if (it == sounds.end())
    {
        for (map<string, PHAllocator>::iterator i = extension->begin(); i!=extensions->end(); i++)
        {
            string enm = name + "." + i->first;
            if (dir->fileExists(enm))
            {
                PHFile * file = NULL;
                PHDecoder * decoder = NULL;
                try {
                    decoder = (PHDecoder*)(i->second)();
                    file = dir->fileAtPath(enm);
                    decoder->setFile(file);
                    file->release();
                    file = NULL;
                } catch (...) {
                    if (file)
                        file->release();
                    if (decoder)
                        decoder->release();
                    throw;
                }
                PHAudioBuffer * buf = new PHAudioBuffer(decoder);
                decoder->release();
                PHSound * snd = new PHSound;
                snd->setBuffer(buf);
                buf->release();
                sounds.insert(make_pair(n, snd));
                return snd;
            }
        }
        throw string("No match found for") + name;
    }
    return it->second;
}

PHSoundManager::~PHSoundManager()
{
    sndDir->release();
    if (context)
    {
       alcMakeContextCurrent(NULL);
       alcDestroyContext(context);
    }
    if (device)
        alcCloseDevice(device);
}

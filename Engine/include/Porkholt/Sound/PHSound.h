/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSOUND_H
#define PHSOUND_H

#include <Porkholt/Sound/PHOpenAL.h>
#include <Porkholt/Core/PHCinematicActor.h>

class PHAudioBuffer;
class PHSoundManager;
class PHSoundPool;

class PHSound : public PHObject, public PHCinematicActor
{
private:
    ALuint id;
    PHAudioBuffer * buf;
    PHSoundManager * man;
    bool st;
    int stack_begin;
    list<ALuint> stack;
    size_t seekv;
    bool schseek;
    bool playing;
    bool loop;
    bool schplaying;
    bool initial;

    void clearBuffer();
    void unqueue(size_t size);
    void songEnded();

    size_t _tag;

    PHInvocation inv;

    set<PHSoundPool*> soundPool;

public:
    size_t tag() { return _tag; }
    void setTag(size_t t) { _tag = t; }

    void addSoundPool(PHSoundPool * sp);
    void removeSoundPool(PHSoundPool * sp);
    
    PHSound(PHSoundManager * man);
    ~PHSound();

    void update();

    void setBuffer(PHAudioBuffer * buf);
    PHAudioBuffer * buffer() { return buf; }

    void play();
    void pause();
    void stop();
    bool isPlaying();

    void playFading();
    void pauseFading();

    ph_float duration();
    ph_float playPosition();
    size_t playPositionSample(ALenum state);
    size_t playPositionSample() { return playPositionSample(-1); }
    void seek(ph_float pos);
    
    bool looping();
    void setLooping(bool l);
    
    void setCallback(const PHInvocation & invo);

    PHSound * copy();
    
    void playAndRelease()
    {
        setCallback(PHInv(this, PHSound::release, NULL));
        play();
    }

    PHSound * detachAndPlay()
    {
        PHSound * snd = copy();
        snd->playAndRelease();
        return snd;
    }

protected:
    ph_float _pitch, _gain, _minGain, _maxGain, _maxDistance, _rolloffFactor, _coneOuterGain, _coneInnerAngle, _coneOuterAngle, _referenceDistance;
    PHVector3 _position, _velocity, _direction;
    bool _relativePositions;

public:
    ph_float pitch() { return _pitch; }
    ph_float gain() { return _gain; }
    ph_float minGain() { return _minGain; }
    ph_float maxGain() { return _maxGain; }
    ph_float maxDistance() { return _maxDistance; }
    ph_float rolloffFactor() { return _rolloffFactor; }
    ph_float coneOuterGain() { return _coneOuterGain; }
    ph_float coneInnerAngle() { return _coneInnerAngle; }
    ph_float coneOuterAngle() { return _coneOuterAngle; }
    ph_float referenceDistance() { return _referenceDistance; }

    void setPitch(ph_float val);
    void setGain(ph_float val);
    void setMinGain(ph_float val);
    void setMaxGain(ph_float val);
    void setMaxDistance(ph_float val);
    void setRolloffFactor(ph_float val);
    void setConeOuterGain(ph_float val);
    void setConeInnerAngle(ph_float val);
    void setConeOuterAngle(ph_float val);
    void setReferenceDistance(ph_float val);

    PHVector3 position() { return _position; }
    PHVector3 velocity() { return _velocity; }
    PHVector3 direction() { return _direction; }

    void setPosition(const PHVector3 & v);
    void setVelocity(const PHVector3 & v);
    void setDirection(const PHVector3 & v);

    bool relativePositions() { return _relativePositions; }
    void setRelativePositions(bool r);

protected:
    void setAnimationFieldF(int field, ph_float value);
    void setAnimationFieldV3(int field, const PHVector3 & value);
    void setAnimationFieldQ(int field, const PHQuaternion & value);

    ph_float getAnimationFieldF(int field);
    PHVector3 getAnimationFieldV3(int field);
    PHQuaternion getAnimationFieldQ(int field);

public:
    void animatePitch(ph_float val) { animateField(PHAnimationField(PHCinematicActor::fieldPitch, val)); }
    void animateGain(ph_float val) { animateField(PHAnimationField(PHCinematicActor::fieldGain, val)); }
    void animateVelocity(const PHVector3 & val) { animateField(PHAnimationField(PHCinematicActor::fieldVelocity, val)); }
    void animateDirection(const PHVector3 & val) { animateField(PHAnimationField(PHCinematicActor::fieldDirection, PHQuaternion::fromPointsOnSphere(PHVector3(0, 0, 1), val))); }
    void animateDirection(const PHQuaternion & val) { animateField(PHAnimationField(PHCinematicActor::fieldDirection, val)); }
};

#endif

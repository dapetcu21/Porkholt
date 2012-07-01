/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>

class PHAudioBuffer;

class PHSound : public PHObject
{
private:
    PHAudioBuffer * buf;
public:
    void setBuffer(PHAudioBuffer * buf);
    PHAudioBuffer * buffer() { return buf; }

    void play();
    void pause();
    void stop();
    bool isPlaying();

    ph_float duration();
    ph_float playPosition();
    void seek(ph_float pos);

    ph_float pitch();
    ph_float gain();
    ph_float minGain();
    ph_float maxGain();
    ph_float maxDistance();
    ph_float rolloffFactor();
    ph_float coneOuterGain();
    ph_float coneInnerAngle();
    ph_float coneOuterAngle();
    ph_float referenceDistance();

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

    PHVector3 position();
    PHVector3 velocity();
    PHVector3 direction();

    void setPosition(const PHVector3 & v);
    void setVelocity(const PHVector3 & v);
    void setDirection(const PHVector3 & v);

    bool relativePositions();
    void setRelativePositions(bool r);
    bool looping();
    void setLooping(bool l);

};

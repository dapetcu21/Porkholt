//
//  PHKeyframeAnimator.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 1/8/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHKEYFRAMEANIMATOR_H
#define PHKEYFRAMEANIMATOR_H

#include "PHGenericCinematicAnimator.h"

class PHKeyframeAnimatorGroup;
class PHKeyframeAnimator : public PHGenericCinematicAnimator
{
public:
    class Keyframe
    {
    protected:
        uint8_t flags;
        ph_float time;
        PHPoint position;
        ph_float rotation;
        PHSize scale;
        PHColor bgColor;
        PHColor customColor;
        ph_float customValue;
        
        int jmpSec;
        ph_float jmpTime;
        
        enum {
            hasPosition = 1<<0,
            hasRotation = 1<<1,
            hasScale = 1<<2,
            hasBgColor = 1<<3,
            hasCustomColor = 1<<4,
            hasCustomValue = 1<<5,
            hasJump = 1<<6
        };
        
        friend class PHKeyframeAnimator;
        
    public:
        Keyframe(ph_float tm) : flags(0), time(tm) {};
        Keyframe(const Keyframe & o) : 
            flags(o.flags),
            time(o.time),
            position(o.position),
            rotation(o.rotation),
            scale(o.scale),
            bgColor(o.bgColor),
            customColor(o.customColor),
            customValue(o.customValue),
            jmpSec(o.jmpSec),
            jmpTime(o.jmpTime) 
        {};
        
        void addPosition(const PHPoint & pos);
        void addRotation(ph_float rot);
        void addScale(const PHSize & scale);
        void addBgColor(const PHColor & clr);
        void addCustomColor(const PHColor & clr);
        void addCustomValue(ph_float val);
        void addJump(int sectionIndex, ph_float time);
        
    };
    static Keyframe positionFrame(ph_float tm, const PHPoint & pos);
    static Keyframe rotationFrame(ph_float tm, ph_float rot);
    static Keyframe scaleFrame(ph_float tm, const PHSize & scale);
    static Keyframe bgColorFrame(ph_float tm, const PHColor & clr);
    static Keyframe customColorFrame(ph_float tm, const PHColor & clr);
    static Keyframe customValueFrame(ph_float tm, ph_float val);
    static Keyframe nullFrame(ph_float tm);
    static Keyframe jumpFrame(ph_float tm, int sectionIndex, ph_float time);
    
    struct Section
    {
        string name;
        vector<Keyframe> frames;
        
        Section(const string & nm, const vector<Keyframe> & frm) : name(nm), frames(frm) {};
        Section(const Section & o) : name(o.name), frames(o.frames) {};
    };
    
private:
    int frame;
    bool playing;
    int section;
    ph_float time;
    
    int nextPos;
    int nextRot;
    int nextScale;
    int nextBgColor;
    int nextCColor;
    int nextCValue;
    
    PHInvocation callback;
    
    vector<Section> sections;
    
    void advanceNexts(int frame);
    
public:
    PHKeyframeAnimator(const vector<Section> &);
    
    void pause() { playing = false; }
    void resume() { playing = true; }
    
    int sectionWithName(const string & name);
    
    void playSection(const string & name) {
        playSection(sectionWithName(name), PHInvocation(), 0.0f); }
    void playSection(int section) {
        playSection(section, PHInvocation(), 0.0f); }
    void playSection(const string & name, const PHInvocation & callback) { 
        playSection(sectionWithName(name), callback, 0.0f); }
    void playSection(int section, const PHInvocation & callback) {
        playSection(section, callback, 0.0f); }
    void playSection(const string & name, const PHInvocation & callback, ph_float seek) { 
        playSection(sectionWithName(name), callback, seek); }
    void playSection(int section, const PHInvocation & callback, ph_float seek);
    
    virtual void advanceAnimation(ph_float elapsedTime);
    
    static PHKeyframeAnimator * fromLua(lua_State * L);
    
    virtual ~PHKeyframeAnimator();
    
private:
    friend class PHKeyframeAnimatorGroup;
    
    set<PHKeyframeAnimatorGroup*> groups;
    
    void addIntoGroup(PHKeyframeAnimatorGroup * g);
    void removeFromGroup(PHKeyframeAnimatorGroup * g);
};

#endif

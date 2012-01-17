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

class PHKeyframeAnimator : public PHGenericCinematicAnimator
{
public:
    class Keyframe
    {
    protected:
        uint8_t flags;
        double time;
        PHPoint position;
        double rotation;
        PHSize scale;
        PHColor bgColor;
        PHColor customColor;
        double customValue;
        
        int jmpSec;
        double jmpTime;
        
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
        Keyframe(double tm) : flags(0), time(tm) {};
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
        void addRotation(double rot);
        void addScale(const PHSize & scale);
        void addBgColor(const PHColor & clr);
        void addCustomColor(const PHColor & clr);
        void addCustomValue(double val);
        void addJump(int sectionIndex, double time);
        
    };
    static Keyframe positionFrame(double tm, const PHPoint & pos);
    static Keyframe rotationFrame(double tm, double rot);
    static Keyframe scaleFrame(double tm, const PHSize & scale);
    static Keyframe bgColorFrame(double tm, const PHColor & clr);
    static Keyframe customColorFrame(double tm, const PHColor & clr);
    static Keyframe customValueFrame(double tm, double val);
    static Keyframe nullFrame(double tm);
    static Keyframe jumpFrame(double tm, int sectionIndex, double time);
    
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
    double time;
    
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
    void playSection(const string & name, const PHInvocation & callback, double seek) { 
        playSection(sectionWithName(name), callback, seek); }
    void playSection(int section, const PHInvocation & callback, double seek);
    
    virtual void advanceAnimation(double elapsedTime);
    
    static PHKeyframeAnimator * fromLua(lua_State * L);
};

#endif

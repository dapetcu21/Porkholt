//
//  PHImageAnimator.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/13/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHIMAGEANIMATOR_H
#define PHIMAGEANIMATOR_H

#include "PHMain.h"

class PHAnimatedImage;
class PHAnimatorPool;

class PHImageAnimator : public PHObject
{
private:
    PHAnimatedImage * _image;
    PHImageAnimator(PHAnimatedImage * img);
    PHImageAnimator(PHAnimatedImage * img, PHAnimatorPool * p);
    virtual ~PHImageAnimator();
    
    friend class PHAnimatedImage;
    friend class PHAnimatorPool;
    
    int section;
    int frame;
    int realframe,lastframe;
    double remaining,time;
    bool fade;
    
    PHObject * target;
    PHCallback callback;
    void * userdata;
    
    double timeForFrameInSection(int fr, int sec);
    int realFrame(int fr, int sec);
    
    bool advanceManually;
    
    bool running;
    
    PHAnimatorPool * pool;
    
public:
    PHAnimatedImage * image() { return _image; }
    void animateSection(const string & name) { animateSection(name, NULL, NULL, NULL); }
    void animateSection(int sec) { animateSection(sec, NULL, NULL, NULL); }
    void animateSection(const string & name, PHObject * target, PHCallback callback, void * userdata);
    void animateSection(int sec, PHObject * target, PHCallback callback, void * userdata);
    void reset(PHObject * target, PHCallback callback, void * userdata);
    void reset() { reset(NULL,NULL,NULL); }
    
    bool isPaused() { return !running; }
    void setPaused(bool p) { running=!p; }
    void pause() { running = false; }
    void resume() { running = true; }
    
    bool isAdvancingManually() { return advanceManually; }
    void setAdvanceManually(bool s) { advanceManually = s; }
    void advanceAnimation(double elapsedTime);
    void setAnimatorPool(PHAnimatorPool * p);
    PHAnimatorPool * animatorPool() { return pool; }
    
    void renderInFrame(const PHRect & frm) { renderInFramePortionTint(frm,PHWholeRect,PHInvalidColor); }
	void renderInFramePortion(const PHRect & frm,const PHRect & cnstr) { renderInFramePortionTint(frm, cnstr,PHInvalidColor); }
    void renderInFramePortionTint(const PHRect & frm,const PHRect & cnstr,const PHColor & tint);
    
    
};

#endif
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
#include "PHImage.h"

class PHAnimatedImage;
class PHAnimatorPool;
class PHImageView;

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
    
    PHInvocation invocation;
    
    double timeForFrameInSection(int fr, int sec);
    int realFrame(int fr, int sec);
    
    bool advanceManually;
    
    bool running;
    
    PHAnimatorPool * pool;
    
public:
    PHAnimatedImage * image() { return _image; }
    void animateSection(const string & name) { animateSection(name, PHInvocation()); }
    void animateSection(int sec) { animateSection(sec, PHInvocation()); }
    void animateSection(const string & name, PHInvocation inv);
    void animateSection(int sec, PHInvocation inv);
    void reset(PHInvocation inv);
    void reset() { reset(PHInvocation()); }
    
    bool isPaused() { return !running; }
    void setPaused(bool p) { running=!p; }
    void pause() { running = false; }
    void resume() { running = true; }
    
    bool isAdvancingManually() { return advanceManually; }
    void setAdvanceManually(bool s) { advanceManually = s; }
    void advanceAnimation(double elapsedTime);
    void setAnimatorPool(PHAnimatorPool * p);
    PHAnimatorPool * animatorPool() { return pool; }
    
    double currentFrameTime() { return time; }
    double remainingFrameTime() { return remaining; }
    int currentSection() { return section; }
    int currentFrame() { return frame; }
    int currentRealFrame() { return realframe; }
    int lastRealFrame() { return lastframe; }
    
    //immediate mode
    void renderInFrame(const PHRect & frm) { renderInFramePortionTint(frm,PHWholeRect,PHInvalidColor); }
	void renderInFramePortion(const PHRect & frm,const PHRect & cnstr) { renderInFramePortionTint(frm, cnstr,PHInvalidColor); }
    void renderInFramePortionTint(const PHRect & frm,const PHRect & cnstr,const PHColor & tint);
    
    //VBOs
    void rebuildVBOs(PHImageView * imageView, GLuint & vbo1, PHImage::VBOParams & params1, GLuint & vbo2, PHImage::VBOParams & params2);
    void bindCurrentFrameToTexture();
    void bindLastFrameToTexture();
    
};

#endif
//
//  PHImageAnimator.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/13/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHIMAGEANIMATOR_H
#define PHIMAGEANIMATOR_H

#include "PHAnimator.h"
#include "PHImage.h"

class PHAnimatedImage;
class PHAnimatorPool;
class PHImageView;

class PHImageAnimator : public PHAnimator
{
private:
    PHAnimatedImage * _image;
    PHImageAnimator(PHAnimatedImage * img);
    PHImageAnimator(PHAnimatedImage * img, PHAnimatorPool * p);
    
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
    
    bool running;
    
public:
    PHAnimatedImage * image() { return _image; }
    void animateSection(const string & name) { animateSection(name, PHInvocation()); }
    void animateSection(int sec) { animateSection(sec, PHInvocation()); }
    void animateSection(const string & name, const PHInvocation & inv);
    void animateSection(int sec, const PHInvocation & inv);
    void reset(const PHInvocation & inv);
    void reset() { reset(PHInvocation()); }
    
    bool isPaused() { return !running; }
    void setPaused(bool p) { running=!p; }
    void pause() { running = false; }
    void resume() { running = true; }
    
    bool isFading() { return fade; }
    
    void advanceAnimation(double elapsedTime);
    
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
    
    PHRect currentFrameTextureCoordinates(const PHRect & port);
    PHRect lastFrameTextureCoordinates(const PHRect & port);
};

#endif
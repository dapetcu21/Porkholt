/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHIMAGEANIMATOR_H
#define PHIMAGEANIMATOR_H

#include <Porkholt/Core/PHAnimator.h>
#include <Porkholt/Core/PHImage.h>

class PHAnimatedImage;
class PHAnimatorPool;
class PHImageView;
class PHGameManager;
class PHGLVertexArrayObject;
class PHGLVertexBufferObject;
class PHGLTexture2D;

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
    ph_float remaining,time;
    bool fade;
    
    PHInvocation invocation;
    
    ph_float timeForFrameInSection(int fr, int sec);
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
    
    void advanceAnimation(ph_float elapsedTime);
    
    ph_float currentFrameTime() { return time; }
    ph_float remainingFrameTime() { return remaining; }
    int currentSection() { return section; }
    int currentFrame() { return frame; }
    int currentRealFrame() { return realframe; }
    int lastRealFrame() { return lastframe; }
    
    //immediate mode
    void renderInFrame(PHGameManager * gameManager, const PHRect & frm) { renderInFramePortionTint(gameManager, frm,PHWholeRect,PHInvalidColor); }
	void renderInFramePortion(PHGameManager * gameManager, const PHRect & frm,const PHRect & cnstr) { renderInFramePortionTint(gameManager, frm, cnstr,PHInvalidColor); }
    void renderInFramePortionTint(PHGameManager * gameManager, const PHRect & frm,const PHRect & cnstr,const PHColor & tint);
    
    //VAOs
    void rebuildVAOs(PHImageView * imageView, PHGLVertexArrayObject * & vao1, PHGLVertexArrayObject * & vao2);

    PHGLTexture2D * currentFrameTexture();
    PHGLTexture2D * lastFrameTexture();
    
    PHRect currentFrameTextureCoordinates(const PHRect & port);
    PHRect lastFrameTextureCoordinates(const PHRect & port);
};

#endif

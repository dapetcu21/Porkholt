//
//  main.cpp
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Porkholt Labs!. All rights reserved.
//

#import "PHStartGame.h"
#include "PHViewController.h"
#include "PHImage.h"
#include "PHImageView.h"

class PHSlide : public PHViewController
{
protected:
    virtual void screenTapped(int count) {
        
    }
    
    int cnt;
    
    void _screenTapped(PHObject * sender, void * ud)
    {
        screenTapped(cnt++);
    }
    
public:
    
    PHSlide() : cnt(0) {
        PHMessage::messageWithName("thingTapped")->addListener(this, (PHCallback)&PHSlide::_screenTapped);
    }
    
    ~PHSlide() {
        PHMessage::messageWithName("thingTapped")->removeListener(this);
    }
};

class PHTitle : public PHSlide
{
protected:
    PHView * loadView(const PHRect & r)
    {
        PHView * v = new PHView(r);
        v->setAutoresizeMask(PHView::ResizeAll);
        v->setAutoresizesSubviews(true);
        return v;
    }
    
    void screenTapped(int count)
    {
        if (count == 0)
        {
            PHView * v = getView();
            PHImage * img = gm->imageNamed("porkholt_labs");
            PHRect rr(0, 0, img->width(), img->height());
            PHRect r = v->bounds();
            rr.x = (r.width - rr.width)/2;
            rr.y = (r.height - rr.height)/2;
            PHImageView * vv = new PHImageView(rr);
            vv->setImage(img);
            vv->setAutoresizeMask(PHView::ResizeFlexibleBorders);
            vv->setScaleX(5.0f);
            vv->setScaleY(5.0f);
            vv->setTintColor(PHClearColor);
            vv->beginCinematicAnimation(1.0f, PHCinematicAnimator::FadeOutFunction);
            vv->animateScale(PHSize(1/5.0f, 1/5.0f));
            vv->animateCustomColor(PHWhiteColor);
            vv->commitCinematicAnimation();
            v->addChild(vv);
            vv->release();
        }
    }
};



class PHCaptureView : public PHView
{
public:
    PHCaptureView() {};
    PHCaptureView(const PHRect & r) : PHView(r) {}
    
protected:
    void touchEvent(PHEvent * evt)
    {
        if (((evt->type() == PHEvent::touchDown) || (evt->type() == PHEvent::touchUp)) && PHPointInRect(toMyCoordinates(evt->location()), this->bounds()))
        {
            evt->setHandled(true);
            if (evt->type() == PHEvent::touchUp)
                PHMessage::messageWithName("thingTapped")->broadcast(this, NULL);
        }
    }
};

void PHGameEntryPoint(PHGameManager * gm)
{   
    PHCaptureView * v = new PHCaptureView(gm->navigationController()->getView()->frame());
    gm->navigationController()->getView()->setAutoresizeMask(PHView::ResizeAll);
    gm->navigationController()->getView()->setAutoresizesSubviews(true);
    v->setAutoresizesSubviews(true);
    v->setUserInput(true);
    v->addChild(gm->navigationController()->getView());
    gm->setMainView(v);
    v->release();
    
    PHViewController * vc = new PHTitle();
	vc->init(gm);
    gm->navigationController()->pushViewController(vc);
}

int main(int argc, char *argv[]) {
    return PHStartGame(argc, argv, PHStartGame_GLES1 | PHStartGame_GLES2 | PHStartGame_Resizable | PHStartGame_FullScreen, &PHGameEntryPoint,NULL);
}
 
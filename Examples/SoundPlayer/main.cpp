/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/Sound/PHSound.h>
#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/Sound/PHRawDecoder.h>
#include <Porkholt/Sound/PHAudioBuffer.h>
#include <Porkholt/UI/PHButtonView.h>

class PHSliderView : public PHView
{
    class knob : public PHView
    {
    public:
        PHSliderView * v;
        knob(PHSliderView * sv) : v(sv), PHView(PHRect(0, 0, 10, 10))
        {
            setBackgroundColor(PHWhiteColor);
        }
    };
    knob * k;
public:
    ph_float _pos;

    PHSliderView(const PHRect & frame) : PHView(frame)
    {
        k = new knob(this);
        addChild(k);
        setUserInput(true);
        setBackgroundColor(PHBlackColor);
        setPosition(0);
    }

    void setPosition(ph_float pos)
    {
        _pos = pos;
        PHRect b(bounds());
        PHPoint p(b.x + b.width * pos, b.y + b.height/2);
        k->setCenter(p);
    }

    void layoutSubviews(const PHRect & oldBounds)
    {
        setPosition(_pos);
    }

    virtual void submitPosition(ph_float pos)
    {
    }

    void touchEvent(PHEvent * evt)
    {
        if (evt->type() == PHEvent::touchDown)
        {
            PHPoint p = toMyCoordinates(evt->location());
            ph_float pos = (p.x - bounds().x) / bounds().width;
            setPosition(pos);
            submitPosition(pos);
            evt->setHandled(true);
        }
    }

    ~PHSliderView()
    {
        k->release();
    }
};

class PHSeekSlider : public PHSliderView
{
    PHSound * snd;

public:

    PHSeekSlider(const PHRect & frame, PHSound * sound) : PHSliderView(frame), snd(sound) {
        snd->retain();
    }

    void submitPosition(ph_float pos)
    {
        snd->seek(pos * snd->duration());
    }

    void update()
    {
        setPosition(snd->playPosition() / snd->duration());
    }

    ~PHSeekSlider()
    {
        snd->release();
    }
};

class PHCheckbox : public PHView
{
public:
    bool state;
    PHInvocation inv;

    void setState()
    {
        setBackgroundColor(state?PHWhiteColor:PHBlackColor);
    }

    PHCheckbox(const PHRect & r) : PHView(r), state(false)
    {
        setUserInput(true);
        setState();
    }
    
    PHCheckbox() : PHView(), state(false)
    {
        setUserInput(true);
        setState();
    }

    void touchEvent(PHEvent * event)
    {
        if (event->type() == PHEvent::touchDown)
        {
            event->setHandled(true);
            state = ! state;
            setState();
            PHInvocation invo = inv;
            inv.call((PHObject*)state);
            inv = invo;
        }
    }
};

class PHPlayerController : public PHViewController
{
    PHSound * snd;
    PHSeekSlider * slider;

    ~PHPlayerController()
    {
        slider->release();
        snd->release();
    }

#define buttonborder 10
#define buttonsize 30
#define sliderhb 30
#define sliderh 10
#define sliderb 30

    PHView * playerView(const PHRect & frame)
    {
        PHView * pv = new PHView(frame);
        pv->setUserInput(true);
        pv->setAutoresizesSubviews(true);
        pv->setBackgroundColor(PHColor(1.0, 1.0, 1.0, 0.3));

        PHView * buttoncontainer = new PHView(PHRect(0, 0, buttonsize*3+buttonborder*2, buttonsize));
        buttoncontainer->setUserInput(true);
        buttoncontainer->setCenter(pv->boundsCenter() - PHPoint(0,sliderhb / 2));

        PHButtonView * playb = new PHButtonView(PHRect(0, 0, buttonsize, buttonsize));
        PHButtonView * pauseb = new PHButtonView(PHRect(buttonsize+buttonborder, 0, buttonsize, buttonsize));
        PHButtonView * stopb = new PHButtonView(PHRect((buttonsize+buttonborder)*2, 0, buttonsize, buttonsize));

        PHImage * img;
    
        img = gm->imageNamed("play");
        playb->setPressedImage(img);
        playb->setImage(img);
    
        img = gm->imageNamed("pause");
        pauseb->setPressedImage(img);
        pauseb->setImage(img);
     
        img = gm->imageNamed("stop");
        stopb->setPressedImage(img);
        stopb->setImage(img);       

        playb->setDownCallback(PHInv(snd, PHSound::play, NULL));
        pauseb->setDownCallback(PHInv(snd, PHSound::pause, NULL));
        stopb->setDownCallback(PHInv(snd, PHSound::stop, NULL));
        
        buttoncontainer->addChild(playb);
        buttoncontainer->addChild(pauseb);
        buttoncontainer->addChild(stopb);

        playb->release();
        pauseb->release();
        stopb->release();

#define nochk 3
        PHCheckbox ** c = new PHCheckbox* [nochk];
        for (int i=0; i<nochk; i++)
        {
            c[i] = new PHCheckbox(PHRect((buttonborder*(i+1)+buttonsize*i), buttonborder, buttonsize, buttonsize));
            pv->addChild(c[i]);
            c[i]->release();
        }

        c[0]->inv = PHInv(snd, PHSound::setLooping, NULL);
        c[1]->inv = PHInv(snd, PHSound::detachAndPlay, NULL);

        pv->addChild(buttoncontainer);
        buttoncontainer->release();

        PHRect b = pv->bounds();
        slider = new PHSeekSlider(PHRect(sliderb, b.height - sliderhb, b.width - 2*sliderb, sliderh), snd);
        pv->addChild(slider);

        return pv;
    }

#define playerborder 30
#define playerheight 100

    PHView * loadView(const PHRect & frame)
    {
        PHView * v = new PHView(frame);
        v->setUserInput(true);
        v->setAutoresizeMask(PHView::ResizeAll);
        v->setAutoresizesSubviews(true);

        snd = gm->soundManager()->soundNamed("music");
        snd->retain();
        pos = 0;

        PHRect pvfr = v->bounds();
        pvfr.x += playerborder;
        pvfr.y += playerborder;
        pvfr.width -= 2*playerborder;
        pvfr.height = playerheight;
        PHView * pv = playerView(pvfr);
        pv->setAutoresizeMask(PHView::ResizeFixedDown | PHView::ResizeFixedLeft | PHView::ResizeFixedRight | PHView::ResizeFlexibleWidth);
        v->addChild(pv);
        pv->release();

        return v;
    }

    ph_float pos;

    void updateScene(ph_float elapsed)
    {
        slider->update();
        pos+=elapsed;
        //snd->setPosition(PHVector3(sin(pos)*10, cos(pos)*10, 0));
    }
};

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHViewController * vc = new PHPlayerController();
    vc->init(gm);
    gm->navigationController()->pushViewController(vc);
    vc->release();
}

int main(int argc, char *argv[]) {
    return PHWMain(argc, argv, PHWGLES1 | PHWVSync | PHWResizable, &PHGameEntryPoint,NULL);
}
 

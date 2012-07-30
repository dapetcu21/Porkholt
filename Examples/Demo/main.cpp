/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/Core/PHImage.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHTextView.h>
#include "PHMenuController.h"
#include "PHTitleScreen.h"
#include <Porkholt/UI/PHTestView.h>
#include <Porkholt/Core/PHParticleView.h>
#include <Porkholt/Core/PHParticleAnimator.h>
#include <Porkholt/Core/PHBezierPath.h>
#include <Porkholt/Sound/PHSound.h>
#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/3D/PHMeshBody.h>
#include <Porkholt/3D/PHSphereMesh.h>
#include <Porkholt/3D/PHProjectionChanger.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/UI/PHButtonView.h>
#include <Porkholt/Core/PHDeferredView.h>

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
    
#define titleborder 0.1
#define titleheader 50.0f
#define spacing 20.0f
#define textleft 0.15
#define textright 0.1

    
    ph_float addTitle(const string & s, ph_float border)
    {
        PHView * v = getView();
        PHRect b = v->bounds();
        
        PHTextView * tv = new PHTextView(PHRect(b.width * titleborder, 0, b.width * (1.0f - 2*titleborder), b.height - border));
        tv->setFont(gm->fontNamed("Helvetica"));
        tv->setFontSize(80.0f);
        tv->setFontColor(PHClearColor);
        tv->setAlignment(PHTextView::justifyCenter | PHTextView::alignTop);
        tv->setText(s);
        PHSize sz = tv->textSize();
        tv->setFrame(PHRect(b.width * titleborder, b.height - border - sz.height, b.width * (1.0f - 2*titleborder), sz.height));
        tv->beginCinematicAnimation(1.0f, PHCinematicAnimator::FadeOutFunction);
        tv->animateCustomColor(PHWhiteColor);
        tv->commitCinematicAnimation();
        tv->setAutoresizeMask(PHView::ResizeFixedUp | PHView::ResizeFlexibleWidth);
        v->addChild(tv);
        tv->release();
        
        return border + sz.height;
    }
    
    void addFullScreenTitle(const string & s)
    {
        PHView * v = getView();
        PHRect b = v->bounds();
        
        PHTextView * tv = new PHTextView(b);
        tv->setFont(gm->fontNamed("Helvetica"));
        tv->setFontSize(100.0f);
        tv->setFontColor(PHClearColor);
        tv->setAlignment(PHTextView::justifyCenter | PHTextView::alignCenter);
        tv->setText(s);
        tv->setScaleX(10.0f);
        tv->setScaleY(10.0f);
        tv->beginCinematicAnimation(0.7f, PHCinematicAnimator::LinearFunction);
        tv->animateCustomColor(PHWhiteColor);
        tv->animateScale(PHSize(0.1f, 0.1f));
        tv->commitCinematicAnimation();
        tv->setAutoresizeMask(PHView::ResizeFixedUp | PHView::ResizeFlexibleWidth);
        v->addChild(tv);
        tv->release();
    }
    
    ph_float addBullet(const string & s, ph_float border, ph_float fontsize) {
        return addBullet(getView(), s, border, fontsize);
    }
    
    ph_float addBullet(const string & s, ph_float border) {
        return addBullet(getView(), s, border, 40.0f);
    }
    
    ph_float addBullet(PHView * v, const string & s, ph_float border) {
        return addBullet(v, s, border, 40.0f);
    }
    
    ph_float addBullet(PHView * v, const string & s, ph_float border, ph_float fontsize)
    {
        PHRect b = v->bounds();
        
        PHTextView * tv = new PHTextView(PHRect(b.width * titleborder, 0, b.width * (1.0f - 2*titleborder), b.height - border));
        tv->setFont(gm->fontNamed("Helvetica"));
        tv->setFontSize(fontsize);
        tv->setFontColor(PHClearColor);
        tv->setAlignment(PHTextView::justifyLeft | PHTextView::alignTop);
        tv->setText(s);
        PHSize sz = tv->textSize();
        tv->setFrame(PHRect(b.width * textleft, b.height - border - sz.height, b.width * (1.0f -textleft - textright), sz.height));
        tv->beginCinematicAnimation(1.0f, PHCinematicAnimator::FadeOutFunction);
        tv->animateCustomColor(PHWhiteColor);
        tv->commitCinematicAnimation();
        tv->setAutoresizeMask(PHView::ResizeFixedUp | PHView::ResizeFlexibleWidth);
        v->addChild(tv);
        tv->release();
        
        return border + sz.height;
    }
};

class PHCapturePView : public PHView
{
protected:
    ph_float y;
    PHEvent * e;
    bool has;
    
    void touchEvent(PHEvent * evt)
    {
        if ((evt->type() == PHEvent::touchDown) || (evt->type() == PHEvent::touchMoved))
        {
            if (e && (e!=evt)) return;
            e = evt;
            y = toMyCoordinates(evt->location()).y;
            has = true;
            evt->setHandled(true);
        }
        if ((evt->type() == PHEvent::touchUp) || (evt->type() == PHEvent::touchCancelled))
            e = NULL;
    }
public:
    PHCapturePView() : e(NULL), has(false) {}
    ph_float getY() { return y; }
    bool hasY() { return has; }
    void reset() 
    {
        y = bounds().height/2;
        has = false;
    }
};

class PHPongController : public PHSlide
{
protected:
    PHView * paddleA, * paddleB;
    PHCapturePView * captureA, * captureB;
    PHView * ball;
    PHVector2 velocity;
    PHView * rootView;
    
    int scoreA,scoreB;
    ph_float waitTime;
    
    bool useAIA, useAIB;
    
    PHTextView * scoreAV, * scoreBV;
    
    void clear()
    {
        scoreA = scoreB = 0;
        useAIA = useAIB = true;
        reset(true);
    }
    
    ph_float ap;
#define border 50
#define paddleWidth 20
#define paddleHeight 150
#define ballDiameter 20
#define textHeight 80
#define maxSpeed 1500.0f
    PHView * loadView(const PHRect & frame)
    {
        PHImage * img = gm->imageNamed("porkholt_labs");
        PHRect rr(0,0,frame.width,frame.width/img->width()*img->height());
        rr.y = (frame.height - rr.height)/2;
        PHImageView * vv = new PHImageView(rr);
        vv->setAutoresizeMask(PHView::ResizeFlexibleBorders);
        vv->setImage(img);
        vv->setTintColor(PHColor(1.0,1.0,1.0,0.4));
        
        PHView * view = rootView = new PHView(frame);
        view->setBackgroundColor(PHBlackColor);
        view->setUserInput(true);
        view->addChild(vv);
        view->setAutoresizeMask(PHView::ResizeAll);
        view->setAutoresizesSubviews(true);
        
        PHRect b = view->bounds();
        
        PHTextView * tv = new PHTextView(b);
        tv->setFont(gm->fontNamed("Helvetica"));
        tv->setFontSize(40.0f);
        tv->setFontColor(PHClearColor);
        tv->setAlignment(PHTextView::justifyCenter | PHTextView::alignCenter);
        tv->setText("http://porkholt.org\nhttp://facebook.com/porkholt\nhttp://twitter.com/porkholt");
        tv->beginCinematicAnimation(1.0f, PHCinematicAnimator::FadeOutFunction);
        tv->animateCustomColor(PHWhiteColor);
        tv->commitCinematicAnimation();
        tv->setAutoresizeMask(PHView::ResizeAll);
        view->addChild(tv);
        tv->release();
        
        scoreA = 0;
        scoreB = 0;
        waitTime = 0;
        
        paddleA = new PHView(PHRect(frame.x+border-paddleWidth,frame.y+(frame.height-paddleHeight)/2,paddleWidth,paddleHeight));
        paddleB = new PHView(PHRect(frame.x+frame.width-border,frame.y+(frame.height-paddleHeight)/2,paddleWidth,paddleHeight));
        paddleA->setBackgroundColor(PHWhiteColor);
        paddleB->setBackgroundColor(PHWhiteColor);
        
        
        scoreAV = new PHTextView(PHRect(frame.x+frame.width/2-textHeight,frame.height,frame.height,textHeight));
        scoreAV->setRotationalCenter(PHOriginPoint);
        scoreAV->setRotation(-M_PI_2);
        scoreAV->setFont(gm->fontNamed("Arial"));
        scoreAV->setFontColor(PHWhiteColor);
        scoreAV->setFontSize(textHeight);
        scoreAV->setAlignment(PHTextView::justifyLeft | PHTextView::alignCenter);
        
        scoreBV = new PHTextView(PHRect(frame.x+frame.width/2+textHeight,0,frame.height,textHeight));
        scoreBV->setRotationalCenter(PHOriginPoint);
        scoreBV->setRotation(M_PI_2);
        scoreBV->setFont(gm->fontNamed("Arial"));
        scoreBV->setFontColor(PHWhiteColor);
        scoreBV->setFontSize(textHeight);
        scoreBV->setAlignment(PHTextView::justifyLeft | PHTextView::alignCenter);
        
        ball = new PHView(PHRect(0,0,ballDiameter,ballDiameter));
        ball->setBackgroundColor(PHWhiteColor);
        
        captureA = new PHCapturePView();
        captureA->setFrame(PHRect(frame.x,frame.y,frame.width/2,frame.height));
        captureB = new PHCapturePView();
        captureB->setFrame(PHRect(frame.x+frame.width/2,frame.y,frame.width/2,frame.height));
        captureA->setUserInput(true);
        captureB->setUserInput(true);
        
        view->addChild(paddleA);
        view->addChild(paddleB);
        view->addChild(captureA);
        view->addChild(captureB);
        view->addChild(ball);
        view->addChild(scoreAV);
        view->addChild(scoreBV);
        clear();
        
        PHMessage::messageWithName("appResumed")->addListener(this, (PHCallback)&PHPongController::clear);
        
        return view;
    }
    
    void reset(bool player)
    {
        velocity = PHVector2(player?300:-300,rand()&1?300:-300);
        captureA->reset();
        captureB->reset();
        ball->setCenter(rootView->center());
        waitTime = 3.0f;
    }
    
    void updateScene(ph_float elapsed)
    {
        PHRect bounds = rootView->bounds();
        
        PHPoint pA = paddleA->center();
        if (captureA->hasY() || !useAIA)
        {
            pA.y = captureA->getY();
            useAIA = false;
        } else 
            pA.y = calculateAI(pA.y,elapsed,true);
        paddleA->setCenter(pA);
        
        PHPoint pB = paddleB->center();
        if (captureB->hasY() || !useAIB)
        {
            pB.y = captureB->getY();
            useAIB = false;
        } else
            pB.y = calculateAI(pB.y,elapsed,false);
        paddleB->setCenter(pB);
        
        ostringstream s;
        s<<scoreA;
        scoreAV->setText(s.str());
        
        ostringstream ss;
        ss<<scoreB;
        scoreBV->setText(ss.str());
        
        if (waitTime>0) 
        {
            waitTime -= elapsed;
            return;
        }
        
        PHPoint p = ball->center();
        p+= velocity*elapsed;
        
        if (p.y<=ballDiameter/2)
        {
            velocity.y = -velocity.y;
            p.y = ballDiameter-p.y;
        }
        
        if (p.y>=bounds.height-ballDiameter/2)
        {
            velocity.y = -velocity.y;
            p.y = bounds.height*2-ballDiameter - p.y;
        }
        
        if (p.x<border)
        {
            if (!((p.y<=pA.y+paddleHeight/2) && (p.y>=pA.y-paddleHeight/2)))
            {
                scoreB++;
                reset(true);
                return;
            } else
            {
                velocity.x = -velocity.x;
                if (velocity.squaredLength() < maxSpeed * maxSpeed)
                    velocity *= 1.3;
                p.x = 2*border-p.x;
            }
        }
        
        if (p.x>bounds.width-border)
        {
            if (!((p.y<=pB.y+paddleHeight/2) && (p.y>=pB.y-paddleHeight/2)))
            {
                scoreA++;
                reset(false);
                return;
            } else
            {
                velocity.x = -velocity.x;
                if (velocity.squaredLength() < maxSpeed * maxSpeed)
                    velocity *= 1.3;
                p.x = 2*(bounds.width - border)-p.x;
            }
        }
        
        ball->setCenter(p);
    }
    
#define maxAISpeed 800.0f
    ph_float calculateAI(ph_float old, ph_float elapsed, bool player)
    {
        PHPoint p = ball->center();
        if ((p.x<rootView->bounds().width/2) != player) return old;
        ph_float ms = maxAISpeed * elapsed;
        //        ph_float bp = p.y+(rand()/((ph_float)RAND_MAX)-0.5)*4*ballDiameter;
        ph_float bp = p.y;
        //        if ((bp<old+paddleHeight/2.5f) && (bp>old-paddleHeight/2.5f)) return old;
        ms = min<ph_float>(max<ph_float>(-ms,bp-old),ms);
        return old+ms;
    }
};

class PHPorkholt3D : public PHSlide
{
protected:
    PHMeshBody * lbody, *body;
    ph_float time;
    
    PHView * loadView(const PHRect & r)
    {
        PHProjectionChanger * container = new PHProjectionChanger(PHMatrix::perspective(M_PI/4, r.width/r.height, 0.5f, 50.0f));
        
        body = new PHMeshBody();
        PHSphereMesh * mesh = new PHSphereMesh(gm, 100, 100);
        body->setMesh(mesh);
        mesh->release();
        body->setPosition(PH3DPoint(0,0,-5));
        PHMaterial * mat = new PHMaterial(PHColor(27/255.0f, 122/255.0f, 224/255.0f), PHWhiteColor, 10.0f);
        body->setMaterial(mat);
        mat->release();
        container->addChild(body);
        
        lbody = new PHMeshBody();
        lbody->setMesh(PHSphereMesh::sphere(gm));
        lbody->setScale(PH3DSize(0.2,0.2,0.2));
        mat = new PHMaterial(PHClearColor, PHClearColor, 0, PHWhiteColor);
        lbody->setMaterial(mat);
        mat->release();
        container->addChild(lbody);
        
        PHView * v = new PHView(r);
        v->addChild(container);
        container->release();
        
        PHGLLight * l = new PHGLLight(PHGLLight::pointLight, PH3DOriginPoint, 2);
        gm->setCurrentLight(l);
        l->release();
        gm->setAmbientColor(PHColor(0.5,0.5,0.5));
        
        PHRect b = v->bounds();
        
        PHTextView * tv = new PHTextView(PHRect(b.width * titleborder, 0, b.width * (1.0f - 2*titleborder), b.height - titleheader));
        tv->setFont(gm->fontNamed("Helvetica"));
        tv->setFontSize(80.0f);
        tv->setFontColor(PHClearColor);
        tv->setAlignment(PHTextView::justifyCenter | PHTextView::alignTop);
        tv->setText("3D");
        PHSize sz = tv->textSize();
        tv->setFrame(PHRect(b.width * titleborder, b.height - titleheader - sz.height, b.width * (1.0f - 2*titleborder), sz.height));
        tv->beginCinematicAnimation(1.0f, PHCinematicAnimator::FadeOutFunction);
        tv->animateCustomColor(PHWhiteColor);
        tv->commitCinematicAnimation();
        tv->setAutoresizeMask(PHView::ResizeFixedUp | PHView::ResizeFlexibleWidth);
        v->addChild(tv);
        tv->release();
        
        return v;
    }
    
    void screenTapped(int count)
    {
        switch (count) {
            case 0:
            {
                PHNormalImage * img = dynamic_cast<PHNormalImage*>(gm->imageNamed("earth", true));
                if (img)
                    body->setTexture(img->texture(), img->textureCoordinates());
                body->material()->diffuse = PHWhiteColor;
                return;
            }
            case 1:
            {
                PHNormalImage * img = dynamic_cast<PHNormalImage*>(gm->imageNamed("earth", true)->normalMap());
                if (img)
                    body->setNormalMap(img->texture(), img->textureCoordinates());
                return;
            }                
            case 3:
            {
                PHViewController * vc = new PHPongController();
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
            }
        }
    }
    
    void updateScene(ph_float timeElapsed)
    {
        PH3DPoint p(0 + sin(time)*2.5, 0, -5 + cos(time)*2.5);
        lbody->setPosition(p);
        body->setRotation(PHQuaternion(time/3, PHVector3(0.0,-1.0,0.0)));
        gm->currentLight()->position = p;
        time += timeElapsed * 2;
    }
    
public:
    PHPorkholt3D() : lbody(NULL), time(0) {}
    ~PHPorkholt3D()
    {
        if (lbody)
            lbody->release();
        if (body)
            body->release();
    }
};

class PHOneMoreThing : public PHSlide
{
    void screenTapped(int count)
    {
        if (count == 0)
        {
            addFullScreenTitle("One more thing");
        }
        if (count == 1)
        {
            PHSlide * vc = new PHPorkholt3D();
            vc->init(gm);
            navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
            vc->release();
        }
    }
};

class PHPlayerController : public PHSlide
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

    void screenTapped(int count)
    {
        PHViewController * vc = new PHOneMoreThing();
        vc->init(gm);
        navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
        vc->release();
    }
};

class PHIgorController : public PHSlide
{
    
protected:
#define numLights 12
    
    struct {
        PHGLLight * light;
        PH3DPoint pnt;
        ph_float rot;
        ph_float rad;
        ph_float speed;
    } a[numLights];
    
    PHView * loadView(const PHRect & frm)
    {
        PHView * v = new PHView(frm);
        v->setBackgroundColor(PHBlackColor);
        
        PHDeferredView * dv = new PHDeferredView(frm);
        dv->setBackgroundColor(PHWhiteColor);
        for (int i = 0; i<numLights; i++)
        {
            PHColor c = PHWhiteColor;
            switch (i&3)
            {
                case 1:
                    c = PHColor(1,0,0);
                    break;
                case 2:
                    c = PHColor(0,1,0);
                    break;
                case 3:
                    c = PHColor(0,0,1);
                    break;
            }
            a[i].light = dv->addPointLight(a[i].pnt = PH3DPoint(rand()%int(frm.width),rand()%int(frm.height),100+rand()%100),c,150.0f);
            a[i].rot = (rand()/(ph_float)RAND_MAX)*M_PI*2;
            a[i].rad = 50+rand()%100;
            a[i].speed = ((rand()/(ph_float)RAND_MAX)-0.5)*8.0;
        }

        dv->setAmbientColor(PHColor(0.2,0.2,0.2,1.0));
        dv->setNormalMapping(true);
        
        PHImage * img = gm->imageNamed("igor", true);
        ph_float height = img->height()/2;
        ph_float width = img->width()/2;
        
#define columns 2
#define rows 3
        PHImageView * iv = new PHImageView(PHRect(0,0,width*columns/1.5,height*rows/1.5));
        iv->setRepeatX(columns);
        iv->setRepeatY(rows);
        iv->setImage(img);
        dv->addChild(iv);
        iv->release();
        
        v->addChild(dv);
        dv->release();

        PHRect b = v->bounds();

        PHTextView * tv = new PHTextView(PHRect(b.width * titleborder, 0, b.width * (1.0f - 2*titleborder), b.height - titleheader));
        tv->setFont(gm->fontNamed("Helvetica"));
        tv->setFontSize(80.0f);
        tv->setFontColor(PHClearColor);
        tv->setAlignment(PHTextView::justifyCenter | PHTextView::alignTop);
        tv->setText("Deferred Shading");
        PHSize sz = tv->textSize();
        tv->setFrame(PHRect(b.width * titleborder, b.height - titleheader - sz.height, b.width * (1.0f - 2*titleborder), sz.height));
        tv->beginCinematicAnimation(1.0f, PHCinematicAnimator::FadeOutFunction);
        tv->animateCustomColor(PHWhiteColor);
        tv->commitCinematicAnimation();
        tv->setAutoresizeMask(PHView::ResizeFixedUp | PHView::ResizeFlexibleWidth);
        v->addChild(tv);
        tv->release();

        return v;
    }
    
    void screenTapped(int count)
    {
        PHSlide * vc = new PHPlayerController;
        vc->init(gm);
        navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
        vc->release();
    }

    void updateScene(ph_float elapsed)
    { 
        for (int i=0; i<numLights; i++)
        {
            a[i].rot += a[i].speed * elapsed;
            a[i].light->position = a[i].pnt + PH3DPoint(a[i].rad*sin(a[i].rot),a[i].rad*cos(a[i].rot),0);
        }
    }
};


class PHPorkholtBezier : public PHSlide
{
    ph_float tb;
    PHBezierPath * bez;
    bool animates;
    ~PHPorkholtBezier() { bez->release(); }
    void screenTapped(int count)
    {
        PHView * v = getView();
        PHRect b = v->bounds();
        switch (count) {
            case 0:
                tb = addTitle("Curves", titleheader);
                return;
            case 1:
                tb = addBullet("- Custom shapes", tb + 2*spacing);
                return;
            case 2:
                tb = addBullet("- Bezier paths", tb + spacing);
                return;
            case 3:
            {
                PHImage * img = gm->imageNamed("platform");
                PHImageView * bv = new PHImageView(PHRect(0, 0, img->width(), img->height()));
                bez = new PHBezierPath();
                vector<PHCurve::anchorPoint> anchors;
                anchors.push_back(PHCurve::anchorPoint(PHPoint(0.00f, 0),1));
                anchors.push_back(PHCurve::anchorPoint(PHPoint(0.25f, 0),0));
                anchors.push_back(PHCurve::anchorPoint(PHPoint(0.50f, 0),0));
                anchors.push_back(PHCurve::anchorPoint(PHPoint(0.75f, 0),0));
                anchors.push_back(PHCurve::anchorPoint(PHPoint(1.00f, 0),2));
                anchors.push_back(PHCurve::anchorPoint(PHPoint(1.00f, 1),3));
                anchors.push_back(PHCurve::anchorPoint(PHPoint(0.75f, 1),0));
                anchors.push_back(PHCurve::anchorPoint(PHPoint(0.50f, 1),0));
                anchors.push_back(PHCurve::anchorPoint(PHPoint(0.25f, 1),0));
                anchors.push_back(PHCurve::anchorPoint(PHPoint(0.00f, 1),4));
                bez->setAnchorPoints(anchors);
                bez->addCurve(PHRange(0,5));
                bez->addCurve(PHRange(5,5));
                bv->setShape(bez);
                bv->setImage(img);
                bv->setConstrainCurveToFrame(true);
                bv->setCenter(v->center() + PHPoint(0, - tb/2));
                v->addChild(bv);
                return;
            }
            case 4:
                animates = true;
                return;
            case 5:
            {
                PHSlide * vc = new PHIgorController;
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
    ph_float time;
    void updateScene(ph_float f)
    {
        if (!animates || !bez) return;
        time += f;
        bez->replaceAnchorPoint(PHCurve::anchorPoint(PHPoint(0.25f, 0+3*sin(time)),0), 1);
        bez->replaceAnchorPoint(PHCurve::anchorPoint(PHPoint(0.25f, 1+3*sin(time)),0), 8);
        bez->replaceAnchorPoint(PHCurve::anchorPoint(PHPoint(0.75f, 0-3*sin(time)),0), 3);
        bez->replaceAnchorPoint(PHCurve::anchorPoint(PHPoint(0.75f, 1-3*sin(time)),0), 6);
    }
public:
    PHPorkholtBezier() : time(0), bez(NULL), animates(false) {}
};
#define particleString \
"PHParticleView * pv = new PHParticleView(PHRect(-100,0,10,10));\n"\
"pv->setImage(gm->imageNamed(\"leaf\"));\n"\
"PHParticleAnimator * pa = pv->particleAnimator();\n"\
"pa->setGenerationArea(PHRect(0,0,0,b.height));\n"\
"pa->setGravity(PHPoint(100, 0));\n"\
"pa->setParticleLifetime(5.0f);\n"\
"pa->setVelocity(PHPoint(200, 100));\n"\
"pa->setInitSize(PHSize(100, 100));\n"\
"pa->setFinalSize(PHSize(100,100));\n"\
"pa->setInitColor(PHWhiteColor);\n"\
"pa->setFinalColor(PHClearColor);\n"\
"pa->setSpreadAngle(M_PI/6);\n"\
"pa->setParticlesPerSecond(10);\n"\
"pa->setRotatesWithVelocity(true);\n"\
"v->addChild(pv);\n"

class PHPorkholtParticles : public PHSlide
{
    ph_float tb;
    void screenTapped(int count)
    {
        PHView * v = getView();
        PHRect b = v->bounds();
        switch (count) {
            case 0:
                tb = addTitle("Particles", titleheader);
                return;
            case 1:
            {
                PHParticleView * pv = new PHParticleView(PHRect(-100,0,10,10));
                pv->setImage(gm->imageNamed("leaf"));
                PHParticleAnimator * pa = pv->particleAnimator();
                pa->setGenerationArea(PHRect(0,0,0,b.height));
                pa->setGravity(PHPoint(100, 0));
                pa->setParticleLifetime(5.0f);
                pa->setVelocity(PHPoint(200, 100));
                pa->setInitSize(PHSize(100, 100));
                pa->setFinalSize(PHSize(100,100));
                pa->setInitColor(PHWhiteColor);
                pa->setFinalColor(PHClearColor);
                pa->setSpreadAngle(M_PI/6);
                pa->setParticlesPerSecond(10);
                pa->setRotatesWithVelocity(true);
                v->addChild(pv);
                pv->sendToBack();
                pv->release();

                tb = addBullet(particleString, tb + 2*spacing, 25.0f);
                return;
            }
            case 2:
            {
                PHSlide * vc = new PHPorkholtBezier;
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHPorkholtAnimImg : public PHSlide
{
    ph_float tb;
    PHImageView * vv;
    PHRect rr;
    void screenTapped(int count)
    {
        PHView * v = getView();
        PHRect b = v->bounds();
        switch(count)
        {
            case 0:
                tb = addTitle("Animated Images", titleheader);
                return;
            case 1:
            {
                PHImage * img = gm->imageNamed("ball");
                rr = PHRect(0, 0, img->width(), img->height());
                vv = new PHImageView(rr);
                vv->setCenter(PHPoint(b.width/2, b.height * 0.45));
                vv->setImage(img);
                vv->setAutoresizeMask(PHView::ResizeFlexibleBorders);
                static const ph_float scl = 1024.0f;
                vv->setScaleX(1/scl);
                vv->setScaleY(1/scl);
                vv->beginCinematicAnimation(0.5f, PHCinematicAnimator::BounceFunction);
                vv->animateScale(PHSize(scl, scl));
                vv->commitCinematicAnimation();
                v->addChild(vv);
                
                PHImageView * f = new PHImageView(vv->bounds());
                f->setImage(gm->imageNamed("face"));
                vv->addChild(f);
                
                f->release();
                vv->release();
                return;
            }
            case 2:
            {
                tb = addBullet("section(\"idle\");\nframe(0, function() return 3+math.random()*3 end);\nframe(1, 0.1);\nframe(2, 0.2);\nframe(1, 0.1);\njump(\"idle\", 0);\n\nsection(\"moving\");\nframe(0, 0);", tb + 2*spacing);
                vv->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeInOutFunction);
                vv->animateMove(PHPoint((b.width * (1.0f - 3*textright) - rr.width/2) - b.width/2, 0));
                vv->commitCinematicAnimation();
                return;
            }
            case 3:
            {
                PHSlide * vc = new PHPorkholtParticles;
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHPorkholtAnim : public PHSlide
{
    ph_float tb;
    PHView * v;
    
    void animCall()
    {
        v->setCenter(getView()->center() + PHPoint(-100, - tb/2));
        v->setRotation(0);
        v->setScaleX(1.0f);
        v->setScaleY(1.0f);
        v->beginCinematicAnimation(2.0f, PHCinematicAnimator::FadeInOutFunction);
        v->animateMove(PHPoint(200,0));
        v->animateRotate(M_PI / 2);
        v->animateScale(PHSize(1.3f,1.3f));
        v->animationCallback(PHInv(this, PHPorkholtAnim::animCall, NULL));
        v->commitCinematicAnimation();
    }
    
    ~PHPorkholtAnim() { v->release(); }
    
    void screenTapped(int count)
    {
        switch(count)
        {
            case 0:
                tb = addTitle("View Animations", titleheader);
                return;
            case 1:
            {
                tb = addBullet("v->beginCinematicAnimation(2.0f, PHCinematicAnimator::FadeInOutFunction);\nv->animateMove(PHPoint(200,0));\nv->animateRotate(M_PI / 2);\nv->animateScale(PHSize(1.3f,1.3f));\nv->commitCinematicAnimation();", tb + 2*spacing);
                
                v = new PHTestView(PHRect(100,100,100,100));
                getView()->addChild(v);
                animCall();
                
                return;
            }
            case 2:
            {
                PHSlide * vc = new PHPorkholtAnimImg;
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHPorkholtViews : public PHSlide
{
    ph_float tb;
    void screenTapped(int count)
    {
        switch(count)
        {
            case 0:
                tb = addTitle("Views", titleheader);
                return;
            case 1:
            {
                PHView * v = new PHTestView(PHRect(100,100,300,300));
                getView()->addChild(v);
                v->setUserInput(true);
                v->release();
                PHView * vv = new PHTestView(PHRect(300,300,100,100));
                vv->setUserInput(true);
                v->addChild(vv);
                vv->release();
                return;
            }
            case 2:
                tb = addBullet("v1->addChild(v2);", tb + 2*spacing);
                return;
            case 3:
            {
                PHSlide * vc = new PHPorkholtAnim;
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHPorkholtHighLevel : public PHSlide
{
    ph_float tb;
    void screenTapped(int count)
    {
        switch (count) {
            case 0:
                tb = addTitle("High Level Facilities", titleheader);
                return;
            case 1:
                tb = addBullet("- Animators", tb + 2 * spacing);
                return;
            case 2:
                tb = addBullet("- Drawables", tb + spacing);
                return;
            case 5:
            {
                PHSlide * vc = new PHPorkholtViews;
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHPorkholtLowLevel : public PHSlide
{
    ph_float tb;
    void screenTapped(int count)
    {
        switch(count) { 
            case 0:
                tb = addTitle("Low Level Facilities", titleheader);
                return;
            case 1:
                tb = addBullet("- Shaders (PHGLShaderProgram)", tb + 2 * spacing);
                return;
            case 2:
                tb = addBullet("- VBOs (PHGLVertexBufferObject)", tb + spacing);
                return;
            case 3:
                tb = addBullet("- VAOs (PHGLVertexArrayObject)", tb + spacing);
                return;
            case 4:
                tb = addBullet("- Shader params (PHGLUniformStates)", tb + spacing);
                return;
            case 5:
                tb = addBullet("- Framebuffer objects (PHGLFramebuffer)", tb + spacing);
                return;
            case 6:
                tb = addBullet("- Analytical Geometry (PHVector2/3/4D, PHQuaternion, PHMatrix, PHRect, PHColor, etc.)", tb + spacing);
                return;
            case 7:
                tb = addBullet("- Threads, Mutexes, Semaphores, Timers, Timing, Event Pools", tb + spacing);
                return;
            case 8:
            {
                PHSlide * vc = new PHPorkholtHighLevel;
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }   
        }
    }
};

class PHPorkholt2 : public PHSlide
{
    ph_float tb;
    void screenTapped(int count)
    {
        switch (count) {
            case 0:
                tb = addTitle("Porkholt SDK", titleheader);
                return;
            case 1:
                tb = addBullet("- Multipurpose", tb + 2 * spacing);
                return;
            case 2:
                tb = addBullet("- Easy to use", tb + spacing);
                return;
            case 3:
                tb = addBullet("- Cross Platform", tb + spacing);
                return;
            case 4:
                tb = addBullet("- Advanced", tb + spacing);
                return;
            case 5:
            {
                PHSlide * vc = new PHPorkholtLowLevel;
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHBeenoDemo : public PHMenuController
{
public:
    void backPressed(PHObject * sender, PHMenuController * ud)
    {
        PHSlide * vc = new PHPorkholt2();
        vc->init(gm);
        gm->navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
        vc->release();
    }
};

class PHBeeno4 : public PHSlide
{
    PHView * loadView(const PHRect & r)
    {
        PHImageView * v = new PHImageView(r);
        v->setAutoresizeMask(PHView::ResizeAll);
        v->setImage(gm->imageNamed("concept"));
        return v;
    }
    void screenTapped(int count)
    {
        if (count == 0)
        {
            PHBeenoDemo * vc = new PHBeenoDemo();
            vc->init(gm);
            gm->navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
            vc->titleScreen()->setBackButton(PHInv(vc, PHBeenoDemo::backPressed,vc));
            vc->release();
        }
    }
};

class PHBeeno3 : public PHSlide
{
    ph_float tb;
    void screenTapped(int count)
    {
        switch (count)
        {
            case 0:
                tb = addTitle("Story", titleheader);
                return;
            case 1:
                tb = addBullet("- 4 chapters", tb + 2*spacing);
                return;
            case 2:
                tb = addBullet("- 20 levels each", tb + spacing);
                return;
            case 3:
                tb = addBullet("- Total of 80 levels", tb + spacing);
                return;
            case 4:
                tb = addBullet("- 16 hours of gameplay", tb + spacing);
                return;
            case 6:
                PHSlide * vc = new PHBeeno4;
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
        }
    }
};

class PHBeeno2 : public PHSlide
{
    ph_float tb;
    void screenTapped(int count)
    {
        PHView * v = getView();
        PHRect b = v->bounds();
        
        switch (count)
        {
            case 0:
                tb = addTitle("Main Character", titleheader);
                return;
            case 1:
                tb = addBullet("- A pea...", tb + 2*spacing);
                return;
            case 2:
                tb = addBullet("- ...called Beeno", tb + spacing);
                return;
            case 3:
            {
                tb = addBullet("- ...not very cool?", tb + spacing);
                
                PHImage * img = gm->imageNamed("ball");
                PHRect rr(0, 0, img->width(), img->height());
                PHImageView * vv = new PHImageView(rr);
                vv->setCenter(PHPoint(b.width * (1.0f - 3*textright) - rr.width/2, b.height * 0.55));
                vv->setImage(img);
                vv->setAutoresizeMask(PHView::ResizeFlexibleBorders);
                static const ph_float scl = 1024.0f;
                vv->setScaleX(1/scl);
                vv->setScaleY(1/scl);
                vv->beginCinematicAnimation(0.5f, PHCinematicAnimator::BounceFunction);
                vv->animateScale(PHSize(scl, scl));
                vv->commitCinematicAnimation();
                v->addChild(vv);
                
                PHImageView * f = new PHImageView(vv->bounds());
                f->setImage(gm->imageNamed("face"));
                vv->addChild(f);
                
                f->release();
                vv->release();
                
                return;
            }
            case 4:
            {
                tb = addTitle("It has superpowers!", tb + 10*spacing);
                
                PHImage * img = gm->imageNamed("poof");
                PHImageView * f = new PHImageView(PHRect(0, 0, img->width(), img->height()));
                f->setImage(img);
                f->setCenter(PHPoint(b.width/2, b.height - tb + 100.0f ));
                v->addChild(f);
                f->release();
                return;
            }
            case 5:
                PHSlide * vc = new PHBeeno3;
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
        }
    }
};

class PHBeeno : public PHSlide
{
    ph_float tb;
    void screenTapped(int count)
    {
        switch (count)
        {
            case 0:
                tb = addTitle("Beeno", titleheader);
                return;
            case 1:
                tb = addBullet("- The most ambitious project of its kind", tb + 2*spacing);
                return;
            case 2:
                tb = addBullet("- Story is the main focus", tb + spacing);
                return;
            case 3:
                tb = addBullet("- Unique control scheme", tb + spacing);
                return;
            case 4:
                PHSlide * vc = new PHBeeno2;
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
        }
    }
};

class PHGames2 : public PHSlide
{
    ph_float tb;
    void screenTapped(int count)
    {
        switch (count)
        {
            case 0:
                tb = addTitle("Porkholt Labs!", titleheader);
                return;
            case 1:
                tb = addBullet("- Making games", tb + 2*spacing);
                return;
            case 2:
                tb = addBullet("- Turning them into reality", tb + spacing);
                return;
            case 3:
                PHSlide * vc = new PHBeeno;
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
        }
    }
};

class PHGames : public PHSlide
{
    void screenTapped(int count)
    {
        if (count == 0)
        {
            addFullScreenTitle("Games");
        }
        if (count == 1)
        {
            PHSlide * vc = new PHGames2;
            vc->init(gm);
            navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
            vc->release();
        }
    }
};

class PHPorkholt : public PHSlide
{
    ph_float tb;
    void screenTapped(int count)
    {
        switch (count)
        {
            case 0:
                tb = addTitle("Porkholt Engine", titleheader);
                return;
            case 1:
                tb = addBullet("- 2D Graphics engine", tb + 2*spacing);
                return;
            case 2:
                tb = addBullet("- Multipurpose", tb + spacing);
                return;
            case 3:
                tb = addBullet("- Crossplatform", tb + spacing);
                return;
            case 4:
                tb = addBullet("- More on that later", tb + spacing);
                return;
            case 5:
                PHSlide * vc = new PHGames;
                vc->init(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
        }
    }
};

class PHSparta : public PHSlide
{
protected:
    ph_float tb;
    void screenTapped(int count)
    {
        PHView * v = getView();
        PHRect b = v->bounds();
        
        if (count == 0)
        {
            tb = addTitle("Before we begin", titleheader);
        }
        if (count == 1)
        {
            tb = addBullet("- This is not PowerPoint", tb + 2*spacing);
        }
        if (count == 2)
        {
            tb = addBullet("- This is not Keynote", tb + spacing);
        }
        if (count == 3)
        {
            tb = addTitle("This is Porkholt!", tb + spacing);
            
            PHImage * img = gm->imageNamed("sparta");
            PHRect rr(0, 0, img->width(), img->height());
            PHImageView * vv = new PHImageView(rr);
            vv->setCenter(PHPoint(b.width/2, (b.height-tb)/2));
            vv->setImage(img);
            vv->setAutoresizeMask(PHView::ResizeFlexibleBorders);
            static const ph_float scl = 1024.0f;
            vv->setScaleX(1/scl);
            vv->setScaleY(1/scl);
            vv->beginCinematicAnimation(0.5f, PHCinematicAnimator::BounceFunction);
            vv->animateScale(PHSize(scl, scl));
            vv->commitCinematicAnimation();
            v->addChild(vv);
            vv->release();

        }
        if (count == 4)
        {
            PHSlide * vc = new PHBeeno4;
            vc->init(gm);
            navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
            vc->release();
        }
    }
};

class PHTitle : public PHSlide
{
protected:    
    void screenTapped(int count)
    {
        if (count == 0)
        {
            gm->soundManager()->setBackgroundMusic(gm->soundManager()->soundNamed("cfox"));
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
        if (count == 1)
        {
            PHSlide * vc = new PHSparta;
            vc->init(gm);
            navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
            vc->release();
        }
    }
};



class PHCapView : public PHView
{
public:
    PHCapView() {};
    PHCapView(const PHRect & r) : PHView(r) {}
    
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
    gm->imageNamed("poof");
    gm->imageNamed("porkholt_labs");
    gm->imageNamed("ball");
    gm->imageNamed("face");
    gm->imageNamed("sparta");
    gm->imageNamed("earth");
    gm->imageNamed("concept");
    
    PHCapView * v = new PHCapView(gm->navigationController()->getView()->frame());
    v->setAutoresizesSubviews(true);
    v->setUserInput(true);
    v->addChild(gm->navigationController()->getView());
    gm->setMainView(v);
    v->release();
    
    PHViewController * vc = new PHTitle();
	vc->init(gm);
    gm->navigationController()->pushViewController(vc);
    vc->release();
    
}

int main(int argc, char *argv[]) {
    return PHWMain(argc, argv, PHWVideoMode(800,600,PHWVideoMode::FullscreenWindowed), PHWGLES1 | PHWResizable | PHWVSync | PHWRemote | PHWDepthBuffer, &PHGameEntryPoint,NULL);
}
 

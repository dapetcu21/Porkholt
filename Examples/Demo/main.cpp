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
#include <Porkholt/Core/PHGenericParticleAnimator.h>
#include <Porkholt/Core/PHBezierPath.h>
#include <Porkholt/Sound/PHSound.h>
#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/3D/PHMeshBody.h>
#include <Porkholt/3D/PHSphereMesh.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/UI/PHButtonView.h>
#include <Porkholt/Core/PHDeferredView.h>
#include <Porkholt/Core/PHPostProcess.h>
#include <Porkholt/Core/PHPerspectiveCamera.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>
#include <Porkholt/Core/PH2DCamera.h>
#include <Porkholt/Core/PHViewControllerHolder.h>
#include <Porkholt/IO/PHDirectory.h>
#include "IGLevelController.h"

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
    float _pos;

    PHSliderView(const PHRect & frame) : PHView(frame)
    {
        k = new knob(this);
        addChild(k);
        setUserInput(true);
        setBackgroundColor(PHBlackColor);
        setPosition(0);
    }

    void setPosition(float pos)
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

    virtual void submitPosition(float pos)
    {
    }

    void touchEvent(PHEvent * evt)
    {
        if (evt->type() == PHEvent::touchDown)
        {
            PHPoint p = evt->drawableLocation()->pointInView(this);
            float pos = (p.x - bounds().x) / bounds().width;
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

    void submitPosition(float pos)
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
    
    PHSlide(PHGameManager * gm) : PHViewController(gm), cnt(0) {
        PHMessage::messageWithName("thingTapped")->addListener(PHInvBindN(this, PHSlide::_screenTapped));
    }
    
    ~PHSlide() {
        PHMessage::messageWithName("thingTapped")->removeListener(this);
    }
    
#define titleborder 0.1
#define titleheader 50.0f
#define spacing 20.0f
#define textleft 0.15
#define textright 0.1

    
    float addTitle(const string & s, float border)
    {
        PHView * v = getView();
        PHRect b = v->bounds();
        
        PHTextView * tv = new PHTextView(PHRect(b.width * titleborder, 0, b.width * (1.0f - 2*titleborder), b.height - border));
        tv->setFont(gm->defaultFont());
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
        tv->setFont(gm->defaultFont());
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
    
    float addBullet(const string & s, float border, float fontsize, bool code) {
        return addBullet(getView(), s, border, fontsize, code);
    }
    
    float addBullet(const string & s, float border) {
        return addBullet(getView(), s, border, 40.0f, false);
    }
    
    float addBullet(PHView * v, const string & s, float border) {
        return addBullet(v, s, border, 40.0f, false);
    }
    
    float addBullet(PHView * v, const string & s, float border, float fontsize, bool code)
    {
        PHRect b = v->bounds();
        
        PHTextView * tv = new PHTextView(PHRect(b.width * titleborder, 0, b.width * (1.0f - 2*titleborder), b.height - border));
        tv->setFont(gm->fontNamed(code?"Monaco":"Helvetica"));
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
    float y;
    void * e;
    bool has;
    
    void touchEvent(PHEvent * evt)
    {
        if ((evt->type() == PHEvent::touchDown) || (evt->type() == PHEvent::touchMoved))
        {
            if (e && (e!=evt->userData())) return;
            e = evt->userData();
            y = evt->drawableLocation()->pointInView(this).y;
            has = true;
            evt->setHandled(true);
        }
        if (evt->type() == PHEvent::touchMoved)
        if ((evt->type() == PHEvent::touchUp) || (evt->type() == PHEvent::touchCancelled))
            e = NULL;
    }
public:
    PHCapturePView() : e(NULL), has(false) {}
    float getY() { return y; }
    bool hasY() { return has; }
    void reset() 
    {
        y = bounds().height/2;
        has = false;
    }
};

class PHPongController : public PHSlide
{
public:
    PHPongController(PHGameManager * gm) : PHSlide(gm) {}
protected:
    PHView * paddleA, * paddleB;
    PHCapturePView * captureA, * captureB;
    PHView * ball;
    PHVector2 velocity;
    PHView * rootView;
    
    int scoreA,scoreB;
    float waitTime;
    
    bool useAIA, useAIB;
    
    PHTextView * scoreAV, * scoreBV;
    
    void clear()
    {
        scoreA = scoreB = 0;
        useAIA = useAIB = true;
        reset(true);
    }
    
    float ap;
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
        tv->setFont(gm->defaultFont());
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
        scoreAV->setFont(gm->defaultFont());
        scoreAV->setFontColor(PHWhiteColor);
        scoreAV->setFontSize(textHeight);
        scoreAV->setAlignment(PHTextView::justifyLeft | PHTextView::alignCenter);
        
        scoreBV = new PHTextView(PHRect(frame.x+frame.width/2+textHeight,0,frame.height,textHeight));
        scoreBV->setRotationalCenter(PHOriginPoint);
        scoreBV->setRotation(M_PI_2);
        scoreBV->setFont(gm->defaultFont());
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
        
        gm->messageWithName("appResumed")->addListener(PHInvBindN(this, PHPongController::clear));
        
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
    
    void updateScene(float elapsed)
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
    float calculateAI(float old, float elapsed, bool player)
    {
        PHPoint p = ball->center();
        if ((p.x<rootView->bounds().width/2) != player) return old;
        float ms = maxAISpeed * elapsed;
        //        float bp = p.y+(rand()/((float)RAND_MAX)-0.5)*4*ballDiameter;
        float bp = p.y;
        //        if ((bp<old+paddleHeight/2.5f) && (bp>old-paddleHeight/2.5f)) return old;
        ms = min<float>(max<float>(-ms,bp-old),ms);
        return old+ms;
    }
};

class PHCodeView : public PHView
{
    public:
        PHTextView * tv1, * tv2;
        PHSize dracie_size;
        PHCodeView(const PHRect & r, const string & t, const string & s, PHFont * f, PHFont * tf, float tts, float ts)
        {
            setFrame(r);
            #define border_dracie 20

            PHTextView * tv = tv1 = new PHTextView(PHRect(
                border_dracie,
                border_dracie,
                r.width - 2*border_dracie,
                r.height - 2*border_dracie));
            tv->setFont(f);
            tv->setFontSize(tts);
            tv->setFontColor(PHWhiteColor);
            tv->setAlignment(PHTextView::alignTop | PHTextView::justifyLeft);
            tv->setText(t);

            PHSize sz = tv->textSize();

            addChild(tv);
            tv->release();

            tv = tv2 = new PHTextView(PHRect(
                border_dracie,
                border_dracie,
                r.width - 2*border_dracie,
                r.height - border_dracie * 3 - sz.height));
            tv->setText(s);
            tv->setWordWrap(false);
            tv->setFont(tf);
            tv->setFontSize(ts);
            tv->setAlignment(PHTextView::alignTop | PHTextView::justifyLeft);
            tv->setFontColor(PHWhiteColor);

            dracie_size = tv->textSize();
            dracie_size.height += border_dracie *3 + sz.height;


            addChild(tv);
            tv->release();

            setScale(PHSize(1.4, 1.4));
            beginCinematicAnimation(0.5, PHCinematicAnimator::FadeOutFunction);
            animateScale(PHSize(1/1.4, 1/1.4));
            animateBgColor(PHColor(0.0, 0.0, 0.0, 0.8));
            commitCinematicAnimation();

        }
};

class PHRippleController : public PHObject
{
    protected:
        PHDrawable * loadDrawable();
        PHGLTexture2D * disperse[2];
        PHGLTexture2D * displace[3];

        PHPostProcess * disv, * difv;
        PHTextureCanvas * distort;

        PHGLUniformStates::uniform * umask, * uoldmask, * umap, * umap2, * utex;

        void onFrame()
        {
            PHGLTexture2D * aux;
            difv->setColorTexture(disperse[0]);
            aux = difv->colorTexture();
            if (!disperse[0]) 
                aux->retain();
            disperse[0] = aux;

            umask->setValue(disperse[0]);
            uoldmask->setValue(disperse[1]);

            disv->setColorTexture(aux = displace[0]);

            utex->setValue(aux);
            umap->setValue(displace[2]);
            umap2->setValue(displace[1]);


            aux = disperse[0]; 
            disperse[0] = disperse[1];
            disperse[1] = aux;

            aux = displace[0];
            displace[0] = displace[1];
            displace[1] = displace[2];
            displace[2] = aux;
        }

        PHGameManager * gm;
        PHDrawable * child;
        PHDrawable * root;
        PHView * stuff;
    public:
        ~PHRippleController()
        {
            if (disperse[0])
                disperse[0]->release();
            if (disperse[1])
                disperse[1]->release();
            if (displace[0])
                displace[0]->release();
            if (displace[1])
                displace[1]->release();
            if (displace[2])
                displace[2]->release();
            if (disv)
                disv->release();
            if (difv)
                difv->release();
            if (distort)
                distort->release();
        }

        PHRippleController(PHGameManager * _gm, PHDrawable * _child) : gm(_gm), child(_child) 
        {
            disperse[0] = disperse[1] = displace[0] = displace[1] = displace[2] = NULL;
        }

        PHDrawable * drawable()
        {
            for (int i = 0; i < 3; i++)
            {
                PHGLTexture2D * t = new PHGLTexture2D(gm);
                t->setWrapS(PHGLTexture::clampToEdge);
                t->setWrapT(PHGLTexture::clampToEdge);
                t->setMinFilter(PHGLTexture::linear);
                t->setMagFilter(PHGLTexture::linear);
                //uint16_t v[4] = {1<<15, 1<<15, 1<<15, 1<<15};
                //uint8_t v[4] = {128, 128, 128, 128};
                //t->setData((uint8_t*)&v, 1, 1, PHGLTexture::R16f);
                displace[i] = t;
            }
            
            disv = new PHPostProcess(gm);
            disv->retain();
            disv->setColorFormat(PHGLTexture::R16f);
            disv->setMaterial(gm->materialNamed("displacement"));
            disv->setOnRenderCallback(PHInvBind(this, PHRippleController::onFrame, NULL));

            difv = new PHPostProcess(gm);
            difv->setColorFormat(PHGLTexture::R16f);
            difv->setMaterial(gm->materialNamed("dispersion"));
            PHGLUniformStates * us = difv->additionalUniforms();
            PHGLUniformStates * uv = disv->additionalUniforms();
            umask = &(us->at("mask"));
            uoldmask = &(us->at("oldmask"));
            umap = &(us->at("map"));
            umap2 = &(us->at("map2"));
            utex = &(uv->at("map"));

            us->at("screenW").setValue(gm->screenWidth());
            us->at("screenH").setValue(gm->screenHeight());
            uv->at("screenWInv").setValue(1.0f/gm->screenWidth());
            uv->at("screenHInv").setValue(1.0f/gm->screenHeight());

            stuff = new PHView(gm->screenBounds());

            disv->addChild(difv);
            difv->setRenderMode(666); 
            difv->addChild(stuff->newProxy());

            distort = new PHTextureCanvas(gm);
            distort->setColorFormat(PHGLTexture::RGBA8);
            distort->setDepthFormat(PHGLFBOAttachment::Depth16);
            distort->setClearColor(PHColor(0.1, 0.1, 0.1, 1.0));
            distort->addChild(child);
            disv->addChild(distort);
            uv->at("texture").setValue(distort->colorTexture());

            PHDrawable * root = new PHDrawable();
            root->addChild(disv);
            root->addChild(stuff);

            return root;
        }

        void showOff() 
        {
            PHRect b = stuff->bounds();
            PHImageView * iv = new PHImageView(PHRect(-200, b.height/2 - 50, 100, 100));
            iv->setImage(gm->imageNamed("player"));
            iv->setMaterial(gm->materialNamed("density"));
            iv->beginCinematicAnimation(2.5);
            iv->animationSkipFirstFrames(1);
            iv->animateMove(PHPoint(b.width/2 + 100, 0));

            iv->chainCinematicAnimation(0.5);
            iv->animateScale(PHPoint(-1, 1));
            iv->chainCinematicAnimation(0.2);
            iv->chainCinematicAnimation(0.5);
            iv->animateScale(PHPoint(-1, 1));

            iv->chainCinematicAnimation(2.5);
            iv->animateMove(PHPoint(b.width/2 + 500, 0));
            iv->commitCinematicAnimation();
            stuff->addChild(iv);
            iv->release();

            PHView * vv = new PHView(PHRect(-500, b.height/2 - 100, 500, 200));
            for (int i = 0; i < 20; i++)
            {
                PHImageView * v = new PHImageView(PHRect(0, 0, 50 * (10.0/7), 50));
                v->setImage(gm->imageNamed("basic_mob"));
                v->setHorizontallyFlipped(true);
                v->setMaterial(gm->materialNamed("density"));
                v->setCenter(PHPoint(50 + 400 * (rand() / (float)RAND_MAX), 25 + 150 * (rand() / (float)RAND_MAX)));
                vv->addChild(v);
                v->release();
            }
            stuff->addChild(vv);
            vv->beginCinematicAnimation(5);
            vv->animationSkipFirstFrames(1);
            vv->chainCinematicAnimation(5);
            vv->animateMove(PHPoint(b.width + 500, 0));
            vv->commitCinematicAnimation();
            vv->release();
        }
};

class PHPorkholt3D : public PHSlide
{
protected:
    PHMeshBody * lbody, *body;
    float time;
    PHPostProcess * canvas;
    PHView * curtain;
    PHImageView * v1, * v2;
    PHRippleController * ripples;
    
    PHView * loadView(const PHRect & r)
    {
        PHView * v = new PHView(r);

        canvas = new PHPostProcess(gm);
        v->addChild(canvas);
        canvas->setColorFormat(PHGLFBOAttachment::RGBA8);
        canvas->setDepthFormat(PHGLFBOAttachment::Depth16);
        canvas->setMaterial(gm->materialNamed("cell_shading"));
        canvas->additionalUniforms()->at("texture").setValue(canvas->colorTexture());       
        PHGLTexture1D * cell_map = new PHGLTexture1D(gm);
        #define cells 5
        uint8_t b[cells];
        for (int i = 0; i<cells; i++)
            b[i] = i * (1.0f/(cells-1)) * 255;
        cell_map->setData(b, cells, PHGLTexture::R8);
        cell_map->setWrapS(PHGLTexture::clampToEdge);
        cell_map->setMinFilter(PHGLTexture::nearest);
        cell_map->setMagFilter(PHGLTexture::nearest);
        canvas->additionalUniforms()->at("cellmap").setValue(cell_map);
        cell_map->release();

        canvas->setEffectEnabled(false);
        

        PHPerspectiveCamera * container = new PHPerspectiveCamera();
        container->setNearClippingPlane(0.5f);
        container->setFarClippingPlane(50.0f);

        canvas->addChild(container);
        canvas->release();
        container->release();

        body = new PHMeshBody();
        body->setMesh(PHSphereMesh::sphere(gm));
        body->setPosition(PH3DPoint(0,0,-5));
        body->setMaterial(gm->materialNamed("chestie_albastra"));
        container->addChild(body);

        PHImage * img = gm->imageNamed("earth", true);
        PHImageView * iv = v1 =  new PHImageView(PHRect(0,0,200,100));
        iv->setImage(img);
        v->addChild(iv);
        iv->release();
        
        iv = v2 = new PHImageView(PHRect(200,0,200,100));
        iv->setImage(img->normalMap());
        v->addChild(iv);
        iv->release();

        lbody = new PHMeshBody();
        lbody->setMesh(PHSphereMesh::sphere(gm));
        lbody->setScale(PH3DSize(0.2,0.2,0.2));
        lbody->setMaterial(gm->materialNamed("chestie_alba"));
        container->addChild(lbody);
        
        PHGLLight * l = new PHGLLight(PHGLLight::pointLight, PH3DOriginPoint, 2);
        gm->setCurrentLight(l);
        l->release();
        gm->setAmbientColor(PHColor(0.5,0.5,0.5));
        
        curtain = new PHView(v->bounds());
        curtain->setBackgroundColor(PHColor(0.0f, 0.0f, 0.0f, 0.0f));
        v->addChild(curtain);
        curtain->release();
        
        PHView * vv = new PHView(r);

        ripples = new PHRippleController(gm, v);
        vv->addChild(ripples->drawable());
        v->release();
        return vv;
    }

    PHCodeView * cv1, * cv2;
   
    void screenTapped(int count)
    {
        switch (count) {
            case 0:
            {
                curtain->beginCinematicAnimation(1.0f, PHCinematicAnimator::FadeOutFunction);
                curtain->animateBgColor(PHColor(0.6f,0.6f,0.6f,0.8));
                curtain->commitCinematicAnimation();

                v1->beginCinematicAnimation(1.0f, PHCinematicAnimator::FadeOutFunction);
                v1->animateCustomColor(PHColor(1.0, 1.0, 1.0, 0.0));
                v1->commitCinematicAnimation();

                v2->beginCinematicAnimation(1.0f, PHCinematicAnimator::FadeOutFunction);
                v2->animateCustomColor(PHColor(1.0, 1.0, 1.0, 0.0));
                v2->commitCinematicAnimation();
                return;
            }
            case 1:
            {
#define cpp_dubios_w 800
#define cpp_dubios_h 200
                PHRect r = curtain->bounds();
                r.x = (r.width - cpp_dubios_w)/2;
                r.y = (r.height - cpp_dubios_h)/2;
                r.width = cpp_dubios_w;
                r.height = cpp_dubios_h;
                PHCodeView * cv = cv1 = new PHCodeView(r,  "C++ code:", 
                "body = new PHMeshBody();\n\
body->setMesh(PHSphereMesh::sphere(gm));\n\
body->setPosition(PH3DPoint(0, 0, -5));\n\
body->setMaterial(gm->materialNamed(\"chestie_albastra\"));\n\
container->addChild(body);"
                , gm->defaultFont(), gm->fontNamed("Monaco"), 30, 15);
                curtain->addChild(cv);
                cv->release();
                return;
            }
            case 2:
            {
#define lua_dubios_w 800
#define lua_dubios_h 560
                PHRect r = curtain->bounds();
                r.x = (r.width - lua_dubios_w)/2;
                r.y = (r.height - lua_dubios_h - cpp_dubios_h - 20)/2;
                r.width = lua_dubios_w;
                r.height = lua_dubios_h;
                
                PHPoint mov(0, r.y - cv1->frame().y + lua_dubios_h + 20); 
                r.y -= mov.y;

                cv1->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeOutFunction);
                cv1->animateMove(mov);
                cv1->commitCinematicAnimation();

                
                PHCodeView * cv = cv2 = new PHCodeView(r, "chestie_albastra.lua:", 
                "emissive = vec4(0.0, 0.0, 0.0, 0.0)\n\
diffuse = vec4(1.0, 1.0, 1.0, 1.0)\n\
specular = vec4(1.0, 1.0, 1.0, 1.0)\n\
shininess = 10\n\
\n\
material = {\n\
    [0] = {\n\
        shader = \"front_pixel_shading[point_light][texture][normal_map][specular]\",\n\
        uniforms = {\n\
            modelViewMatrix = vars.modelViewMatrix,\n\
            projectionMatrix = vars.projectionMatrix,\n\
            normalMatrix = vars.modelViewMatrix.inverse.transposed,\n\
            ambientColor = emissive + vars.ambientColor * diffuse,\n\
            hininess = shininess,\n\
            diffuseColor = diffuse * vars.lightDiffuse * vars.lightIntensity,\n\
            specularColor = specular * vars.lightSpecular * vars.lightIntensity,\n\
            lightPosition = vars.lightPosition;\n\
            tex = \"earth\",\n\
            nMap = \"earth.nmap\",\n\
            texR = vec4(0, 0, 1, 1),\n\
            nMapR = vec4(0, 0, 1, 1),\n\
        }\n\
    }\n\
}", gm->defaultFont(), gm->fontNamed("Monaco"), 25, 14);

                cv2->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeOutFunction);
                cv2->animateMove(mov);
                cv2->commitCinematicAnimation();


                curtain->addChild(cv);
                cv->release();
                return;
            }
            case 3:
            {
                cv1->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeInFunction);
                cv1->animationCallback(PHInvN(cv1, PHView::removeFromParent));
                cv1->animateScale(PHSize(3., 3.));
                cv1->animateBgColor(PHColor(0, 0, 0, 0));
                cv1->commitCinematicAnimation();

                cv1->tv1->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeInFunction);
                cv1->tv1->animateCustomColor(PHColor(0, 0, 0, 0));
                cv1->tv1->commitCinematicAnimation();

                cv1->tv2->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeInFunction);
                cv1->tv2->animateCustomColor(PHColor(0, 0, 0, 0));
                cv1->tv2->commitCinematicAnimation();


                cv2->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeInFunction);
                cv2->animationCallback(PHInvN(cv2, PHView::removeFromParent));
                cv2->animateScale(PHSize(3., 3.));
                cv2->animateBgColor(PHClearColor);
                cv2->commitCinematicAnimation();

                cv2->tv1->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeInFunction);
                cv2->tv1->animateCustomColor(PHColor(0, 0, 0, 0));
                cv2->tv1->commitCinematicAnimation();

                cv2->tv2->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeInFunction);
                cv2->tv2->animateCustomColor(PHColor(0, 0, 0, 0));
                cv2->tv2->commitCinematicAnimation();


                curtain->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeOutFunction);
                curtain->animateBgColor(PHClearColor);
                curtain->commitCinematicAnimation();

                return;
            }
            case 4:
            {
                canvas->setEffectEnabled(true);
                return;
            }                
            case 5:
            {
                curtain->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeOutFunction);
                curtain->animateBgColor(PHColor(.6, .6, .6, .8));
                curtain->commitCinematicAnimation();
                return;
            }
            case 6:
            {
#define cell_dubios_w 800
#define cell_dubios_h 600
                PHRect r = curtain->bounds();
                r.x = (r.width - cell_dubios_w)/2;
                r.y = (r.height - cell_dubios_h)/2;
                r.width = cell_dubios_w;
                r.height = cell_dubios_h;
               
                PHCodeView * cv = new PHCodeView(r, "All the code needed:", "canvas = new PHPostProcess(gm);\n\
v->addChild(canvas);\n\
canvas->setColorFormat(PHGLFBOAttachment::RGBA8);\n\
canvas->setDepthFormat(PHGLFBOAttachment::Depth16);\n\
canvas->setMaterial(gm->materialNamed(\"cell_shading\"));\n\
canvas->additionalUniforms()->at(\"texture\").setValue(canvas->colorTexture());       \n\
PHGLTexture1D * cell_map = new PHGLTexture1D(gm);\n\
#define cells 5\n\
uint8_t b[cells];\n\
for (int i = 0; i<cells; i++)\n\
    b[i] = i * (1.0f/(cells-1)) * 255;\n\
cell_map->setData(b, cells, PHGLTexture::R8);\n\
cell_map->setWrapS(PHGLTexture::clampToEdge);\n\
cell_map->setMinFilter(PHGLTexture::nearest);\n\
cell_map->setMagFilter(PHGLTexture::nearest);\n\
canvas->additionalUniforms()->at(\"cellmap\").setValue(cell_map);"
                , gm->defaultFont(), gm->fontNamed("Monaco"), 40, 20);
                curtain->addChild(cv);
                cv->release();
                return;
            }
            case 7:
            {
                ripples->showOff();
                return;
            }
            case 8:
            {
                PHViewController * vc = new PHPongController(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
    
    void updateScene(float timeElapsed)
    {
        PH3DPoint p(0 + sin(time)*2.5, 0, -5 + cos(time)*2.5);
        lbody->setPosition(p);
        body->setRotation(PHQuaternion(time/3, PHVector3(0.0,-1.0,0.0)));
        gm->currentLight()->position = p;
        time += timeElapsed * 2;
    }
    
public:
    PHPorkholt3D(PHGameManager * gm) : PHSlide(gm), lbody(NULL), time(0), ripples(NULL) {}
    ~PHPorkholt3D()
    {
        if (lbody)
            lbody->release();
        if (body)
            body->release();
        if (ripples)
            ripples->release();
    }
};

class PHOneMoreThing : public PHSlide
{
    public:
    PHOneMoreThing(PHGameManager * gm) : PHSlide(gm) {}
    protected:
    void screenTapped(int count)
    {
        if (count == 0)
        {
            addFullScreenTitle("One more thing");
        }
        if (count == 1)
        {
            PHSlide * vc = new PHPorkholt3D(gm);
            navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
            vc->release();
        }
    }
};

class PHPlayerController : public PHSlide
{
    PHSound * snd;
    PHSeekSlider * slider;

public:
    PHPlayerController(PHGameManager * gm) : PHSlide(gm) {}
    ~PHPlayerController()
    {
        slider->release();
        snd->release();
    }
protected:

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
    
        img = gm->imageNamed("playp");
        playb->setPressedImage(img);
        playb->setImage(img);
    
        img = gm->imageNamed("pausep");
        pauseb->setPressedImage(img);
        pauseb->setImage(img);
     
        img = gm->imageNamed("stopp");
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

    float pos;

    void updateScene(float elapsed)
    {
        slider->update();
        pos+=elapsed;
        //snd->setPosition(PHVector3(sin(pos)*10, cos(pos)*10, 0));
    }

    float tb;
    void screenTapped(int count)
    {
        switch(count)
        {
            case 0:
                tb = addTitle("Sound System", titleheader);
                return;
            case 1:
                tb = addBullet("- OpenAL: 3D Sound", tb + 2*spacing);
                return;
            case 2:
                tb = addBullet("- Asynchronous buffering", tb + spacing);
                return;
            case 3:
                tb = addBullet("- Decoder plugin system", tb + spacing);
                return;
            case 4:
            {
                PHViewController * vc = new PHOneMoreThing(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHIgorController : public PHSlide
{
public:
    PHIgorController(PHGameManager * gm) : PHSlide(gm) {}
protected:
#define numLights 12
    
    struct {
        PHGLLight * light;
        PH3DPoint pnt;
        float rot;
        float rad;
        float speed;
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
            a[i].rot = (rand()/(float)RAND_MAX)*M_PI*2;
            a[i].rad = 50+rand()%100;
            a[i].speed = ((rand()/(float)RAND_MAX)-0.5)*8.0;
        }

        dv->setAmbientColor(PHColor(0.2,0.2,0.2,1.0));
        //dv->setNormalMapping(true);
        
        PHImage * img = gm->imageNamed("igor", true);
        float height = img->height()/2;
        float width = img->width()/2;
        
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
        tv->setFont(gm->defaultFont());
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
        PHSlide * vc = new PHPlayerController(gm);
        navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
        vc->release();
    }

    void updateScene(float elapsed)
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
protected:
    float tb;
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
                PHSlide * vc = new PHIgorController(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
    float time;
    void updateScene(float f)
    {
        if (!animates || !bez) return;
        time += f;
        bez->replaceAnchorPoint(PHCurve::anchorPoint(PHPoint(0.25f, 0+3*sin(time)),0), 1);
        bez->replaceAnchorPoint(PHCurve::anchorPoint(PHPoint(0.25f, 1+3*sin(time)),0), 8);
        bez->replaceAnchorPoint(PHCurve::anchorPoint(PHPoint(0.75f, 0-3*sin(time)),0), 3);
        bez->replaceAnchorPoint(PHCurve::anchorPoint(PHPoint(0.75f, 1-3*sin(time)),0), 6);
    }
public:
    PHPorkholtBezier(PHGameManager * gm) : PHSlide(gm), time(0), bez(NULL), animates(false) {}
};
#define particleString \
"PHParticleView * pv = new PHParticleView(PHRect(-100,0,10,10));\n"\
"PHGenericParticleAnimator * pa = new PHGenericParticleAnimator();\n"\
"pv->setImage(gm->imageNamed(\"leaf\"));\n"\
"pv->setParticleAnimator(pa);\n"\
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
"v->addChild(pv);\n"\
"pa->release();\n"\
"pv->release();\n"\

class PHPorkholtParticles : public PHSlide
{
public:
    PHPorkholtParticles(PHGameManager * gm) : PHSlide(gm) {}
protected:
    float tb;
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
                PHGenericParticleAnimator * pa = new PHGenericParticleAnimator();
                pv->setImage(gm->imageNamed("leaf"));
                pv->setParticleAnimator(pa);
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
                pa->release();
                pv->sendToBack();
                pv->release();

                tb = addBullet(particleString, tb + 2*spacing, 25.0f, true);
                return;
            }
            case 2:
            {
                PHSlide * vc = new PHPorkholtBezier(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHPorkholtAnimImg : public PHSlide
{
public:
    PHPorkholtAnimImg(PHGameManager * gm) : PHSlide(gm) {}
protected:
    float tb;
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
                static const float scl = 1024.0f;
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
                tb = addBullet("section(\"idle\");\nframe(0, function() return 3+math.random()*3 end);\nframe(1, 0.1);\nframe(2, 0.2);\nframe(1, 0.1);\njump(\"idle\", 0);\n\nsection(\"moving\");\nframe(0, 0);", tb + 2*spacing, 40.0f, true);
                vv->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeInOutFunction);
                vv->animateMove(PHPoint((b.width * (1.0f - 3*textright) - rr.width/2) - b.width/2, 0));
                vv->commitCinematicAnimation();
                return;
            }
            case 3:
            {
                PHSlide * vc = new PHPorkholtParticles(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHPorkholtAnim : public PHSlide
{
public:
    PHPorkholtAnim(PHGameManager * gm) : PHSlide(gm) {}
protected:
    float tb;
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
                tb = addBullet("v->beginCinematicAnimation(2.0f, PHCinematicAnimator::FadeInOutFunction);\nv->animateMove(PHPoint(200,0));\nv->animateRotate(M_PI / 2);\nv->animateScale(PHSize(1.3f,1.3f));\nv->commitCinematicAnimation();", tb + 2*spacing, 40.0f, true);
                
                v = new PHTestView(PHRect(100,100,100,100));
                getView()->addChild(v);
                animCall();
                
                return;
            }
            case 2:
            {
                PHSlide * vc = new PHPorkholtAnimImg(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHPorkholtViews : public PHSlide
{
public:
    PHPorkholtViews(PHGameManager * gm) : PHSlide(gm) {}
protected:
    float tb;
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
                tb = addBullet("v1->addChild(v2);", tb + 2*spacing, 40.0f, true);
                return;
            case 3:
            {
                PHSlide * vc = new PHPorkholtAnim(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHPorkholtHighLevel : public PHSlide
{
public:
    PHPorkholtHighLevel(PHGameManager * gm) : PHSlide(gm) {}
protected:
    float tb;
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
            case 3:
            {
                PHSlide * vc = new PHPorkholtViews(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHPorkholtLowLevel : public PHSlide
{
public:
    PHPorkholtLowLevel(PHGameManager * gm) : PHSlide(gm) {}
protected:
    float tb;
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
                PHSlide * vc = new PHPorkholtHighLevel(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }   
        }
    }
};

class PHPorkholt2 : public PHSlide
{
public:
    PHPorkholt2(PHGameManager * gm) : PHSlide(gm) {}
protected:
    float tb;
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
                tb = addBullet("- Cross-platform", tb + spacing);
                return;
            case 4:
                tb = addBullet("- Advanced", tb + spacing);
                return;
            case 5:
            {
                PHSlide * vc = new PHPorkholtLowLevel(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHIgorDemo : public PHViewController
{
    public:
        PHIgorDemo(PHGameManager * gm) : PHViewController(gm) {}
        
        PHView * loadView(const PHRect & r)
        {
            PHView * v = new PHView(r);
            v->setGameManager(gm);
            v->setUserInput(true);

            PH2DCamera * camera = new PH2DCamera();
            camera->setScreenSize(PHSize(0,4.0f));
            camera->setResetsMatrices(true);
            v->addChild(camera);
            PHRect bounds = camera->realScreenBounds();
            PHViewControllerHolder * holder = new PHViewControllerHolder(bounds);
            camera->addChild(holder);
            
            PHNavigationController * nav = new PHNavigationController(gm);
            holder->setViewController(nav);
            
            camera->release();
            holder->release();
            nav->release();

            PHDirectory * dir = gm->resourceDirectory()->directoryAtPath("story");
            IGLevelController * vc = new IGLevelController(gm, dir);
            nav->pushViewController(vc);

            vc->setBackCallback(PHInv(this, PHIgorDemo::back, NULL));

            return v;
        }

        void back()
        {
            PHSlide * vc = new PHPorkholt2(gm);
            navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
            vc->release();
            gm->setClearColor(PHClearColor);
        }
};

class PHIgorJoking : public PHSlide
{
    public:
        PHIgorJoking(PHGameManager * gm) : PHSlide(gm) {}
        void screenTapped(int count)
        {
            switch (count) {
                case 0:
                    addFullScreenTitle("Just joking");
                    break;
                case 1:
                {
                    PHIgorDemo * vc = new PHIgorDemo(gm);
                    navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                    vc->release();
                }
            }
        }
};

class PHIgorThisYear : public PHSlide
{
    public:
        PHIgorThisYear(PHGameManager * gm) : PHSlide(gm) {}
        void screenTapped(int count)
        {
            switch (count) {
                case 0:
                    addFullScreenTitle("This year...");
                    return;
                case 1:
                    gm->soundManager()->setBackgroundMusic(gm->soundManager()->soundNamed("drumroll"));
                    return;
                case 2:
                {
                    gm->soundManager()->setBackgroundMusic(NULL);
                    PHImageView * iv = new PHImageView(gm->screenBounds());
                    iv->setImage(gm->imageNamed("bsod"));
                    getView()->addChild(iv);
                    iv->release();
                    return;
                }
                case 3:
                {
                    PHSlide * vc = new PHIgorJoking(gm);
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
    PHBeenoDemo(PHGameManager * gm) : PHMenuController(gm) {}
    void backPressed(PHObject * sender, PHMenuController * ud)
    {
        PHSlide * vc = new PHIgorThisYear(gm);
        gm->soundManager()->setBackgroundMusic(NULL);
        navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
        vc->release();
    }
};

class PHBeeno4 : public PHSlide
{
public:
    PHBeeno4(PHGameManager * gm) : PHSlide(gm) {}
protected:
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
            PHBeenoDemo * vc = new PHBeenoDemo(gm);
            navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
            vc->titleScreen()->setBackButton(PHInv(vc, PHBeenoDemo::backPressed,vc));
            vc->release();
        }
    }
};

class PHBeeno3 : public PHSlide
{
public:
    PHBeeno3(PHGameManager * gm) : PHSlide(gm) {}
protected:
    float tb;
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
                PHSlide * vc = new PHBeeno4(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
        }
    }
};

class PHBeeno2 : public PHSlide
{
public:
    PHBeeno2(PHGameManager * gm) : PHSlide(gm) {}
protected:
    float tb;
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
                static const float scl = 1024.0f;
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
            {
                PHSlide * vc = new PHBeeno3(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
            }
        }
    }
};

class PHBeeno : public PHSlide
{
public:
PHBeeno(PHGameManager * gm) : PHSlide(gm) {}
protected:
    float tb;
    void screenTapped(int count)
    {
        switch (count)
        {
            case 0:
                addFullScreenTitle("Last year...");
                return;
            case 1:
                PHBeenoDemo * vc = new PHBeenoDemo(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->titleScreen()->setBackButton(PHInv(vc, PHBeenoDemo::backPressed,vc));
                vc->release();
                return;
        }
    }
};

class PHGames2 : public PHSlide
{
public:
    PHGames2(PHGameManager * gm) : PHSlide(gm) {}
protected:
    float tb;
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
                PHSlide * vc = new PHBeeno(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
                return;
        }
    }
};

class PHGames : public PHSlide
{
public:
    PHGames(PHGameManager * gm) : PHSlide(gm) {}
protected:
    void screenTapped(int count)
    {
        if (count == 0)
        {
            addFullScreenTitle("Games");
        }
        if (count == 1)
        {
            PHSlide * vc = new PHGames2(gm);
            navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
            vc->release();
        }
    }
};

class PHPorkholt : public PHSlide
{
public:
    PHPorkholt(PHGameManager * gm) : PHSlide(gm) {}
protected:
    float tb;
    void screenTapped(int count)
    {
        switch (count)
        {
            case 0:
                tb = addTitle("Porkholt SDK", titleheader);
                return;
            case 1:
                tb = addBullet("- 2D Graphics engine", tb + 2*spacing);
                return;
            case 2:
                tb = addBullet("- Awesome, but more on that later", tb + spacing);
                return;
            case 3:
                PHSlide * vc = new PHGames(gm);
                navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
                vc->release();
        }
    }
};

class PHSparta : public PHSlide
{
public:
    PHSparta(PHGameManager * gm) : PHSlide(gm) {}
protected:
    float tb;
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
            static const float scl = 1024.0f;
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
            PHSlide * vc = new PHPorkholt(gm);
            navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
            vc->release();
        }
    }
};

class PHTitle : public PHSlide
{
public:
    PHTitle(PHGameManager * gm) : PHSlide(gm) {}
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
            PHSlide * vc = new PHSparta(gm);
            navigationController()->pushViewController(vc, PHNavigationController::FadeToColor, true);
            vc->release();
            gm->soundManager()->setBackgroundMusic(NULL);
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
        if (evt->type() == PHEvent::touchDown)
        {
            PHMessage::messageWithName("thingTapped")->broadcast(this, NULL);
            evt->setHandled(true);
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

    PH2DCamera * camera = new PH2DCamera();
//    camera->setScreenSize(PHVector2(0, 800));
    gm->setMainDrawable(camera);
    PHRect bounds = gm->screenBounds();

    PHCapView * cap = new PHCapView(bounds);
    cap->setUserInput(true);
    cap->setAutoresizesSubviews(true);
    cap->setAutoresizeMask(PHView::ResizeAll);
    camera->addChild(cap);

    PHViewControllerHolder * holder = new PHViewControllerHolder(bounds);
    cap->addChild(holder);
    
    PHNavigationController * nav = new PHNavigationController(gm);
    holder->setViewController(nav);
    
    camera->release();
    holder->release();
    cap->release();
        
    PHViewController * vc = new PHTitle(gm);
//    PHViewController * vc = new PHIgorThisYear(gm);
    nav->pushViewController(vc);
    vc->release();
    nav->release(); 
}

int main(int argc, char *argv[]) {
    return PHWMain(argc, argv, PHWVideoMode(800,600,PHWVideoMode::FullscreenWindowed), PHWGLES1 | PHWResizable | PHWVSync | PHWRemote | PHWDepthBuffer, &PHGameEntryPoint,NULL);
}
 

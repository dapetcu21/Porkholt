/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHTextView.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>
#include <sstream>


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

class PHPongController : public PHViewController
{
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
    
#define border 50
#define paddleWidth 20
#define paddleHeight 150
#define ballDiameter 20
#define textHeight 80
#define maxSpeed 1500.0f
    PHView * loadView(const PHRect & frame)
    {
        PHView * view = rootView = new PHView(frame);
        view->setBackgroundColor(PHBlackColor);
        view->setUserInput(true);
        
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

    public:
    PHPongController(PHGameManager * gm) : PHViewController(gm) {}
};


void PHGameEntryPoint(PHGameManager * gm)
{    
    PHPongController * vc = new PHPongController(gm);
    gm->setUpNavigationController()->pushViewController(vc);
}

PHMAIN_DEFINE
{
    srand(time(NULL));
    PHMAIN_RETURN(PHWMain(PHMAIN_ARGS, PHWVideoMode(1024, 700, 60, PHWVideoMode::Windowed), PHWGLES1 | PHWFrameAnimation | PHWVSync | PHWShowFPS, &PHGameEntryPoint,NULL));
}
 

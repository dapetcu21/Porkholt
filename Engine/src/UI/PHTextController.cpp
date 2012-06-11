/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/UI/PHTextController.h>
#include <Porkholt/UI/PHTextControllerView.h>
#include <Porkholt/Core/PHTextView.h>
#include <Porkholt/Core/PHGameManager.h>

PHTextController::PHTextController(vector<string> * s) : PHViewController(), bColor(PHBlackColor), fColor(PHWhiteColor), strings(s), canAdvance(false), pos(0), textView(NULL), blackoutView(NULL)
{
    
}

PHView * PHTextController::loadView(const PHRect & frame)
{
    PHTextControllerView * view = new PHTextControllerView(this);
    PHRect fr = gm->screenBounds();
    view->setFrame(fr);
    fr.width/=fr.height;
    fr.height = 1;
    view->setBounds(fr);
    view->setBackgroundColor(bColor);
    view->setUserInput(true);
    textView = new PHTextView(PHRect(fr.width/12,1.0f/6,fr.width*5/6,4.0f/6));
    textView->setFontSize(0.1f);
    textView->setFontColor(fColor);
    textView->setFont(gm->fontNamed("Arial"));
    pos = 0;
    textView->setText((*strings)[pos]);
    textView->setAlignment(PHTextView::alignCenter | PHTextView::justifyCenter);
    textView->adjustFontSizeToFit(5);
    view->addChild(textView);
    textView->release();
    blackoutView = new PHView(fr);
    view->addChild(blackoutView);
    blackoutView->release();
    return view;
}

void PHTextController::viewDidAppear()
{
    canAdvance = true;
}

void PHTextController::setBackgroundColor(const PHColor & c) 
{ 
    bColor = c; 
    if (view)
        view->setBackgroundColor(bColor);
}

void PHTextController::setForegroundColor(const PHColor & c) 
{ 
    fColor = c; 
    if (textView)
        textView->setFontColor(fColor);
}

void PHTextController::beginBlackout()
{
    blackoutView->setBackgroundColor(PHInvalidColor);
    blackoutView->beginCinematicAnimation(0.5f,PHCinematicAnimator::FadeOutFunction);
    blackoutView->animateBgColor(bColor);
    blackoutView->animationTag(-4444);
    blackoutView->animationCallback(PHInvN(this, PHTextController::middleBlackout));
    blackoutView->chainCinematicAnimation(0.5f,PHCinematicAnimator::FadeInFunction);
    blackoutView->animateBgColor(PHClearColor);
    blackoutView->animationTag(-4444);
    blackoutView->animationCallback(PHInvN(this,PHTextController::endBlackout));
    blackoutView->commitCinematicAnimation();
}

void PHTextController::middleBlackout()
{
    textView->setFontSize(0.1f);
    textView->adjustFontSizeToFit(5);
    textView->setText((*strings)[pos]);
}

void PHTextController::endBlackout()
{
    canAdvance = true;
}

void PHTextController::advance()
{
    if (!canAdvance) return;
    canAdvance = false;
    pos++;
    if (pos>=strings->size())
    {
        delete strings;
        strings = NULL;
        invocation.call(this);
    } else {
        beginBlackout();
    }
}

PHTextController::~PHTextController()
{
    if (strings)
        delete strings;
}

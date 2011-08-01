//
//  PHTextController.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHTextController.h"
#include "PHTextControllerView.h"
#include "PHTextView.h"
#include "PHMainEvents.h"

PHTextController::PHTextController(vector<string> * s) : PHViewController(), bColor(PHBlackColor), fColor(PHWhiteColor), strings(s), canAdvance(false), pos(0), textView(NULL), blackoutView(NULL), cb(NULL), trg(NULL), ud(NULL)
{
    
}

PHView * PHTextController::loadView(const PHRect & frame)
{
    PHTextControllerView * view = new PHTextControllerView(this);
    PHRect fr = PHMainEvents::sharedInstance()->screenBounds();
    view->setFrame(fr);
    fr.width/=fr.height;
    fr.height = 1;
    view->setBounds(fr);
    view->setBackgroundColor(bColor);
    view->setUserInput(true);
    textView = new PHTextView(PHRect(fr.width/12,1.0f/6,fr.width*5/6,4.0f/6));
    textView->setFontSize(0.1f);
    textView->setFontColor(fColor);
    textView->setFont(PHFont::fontNamed("Arial"));
    pos = 0;
    textView->setText((*strings)[pos]);
    textView->setAlignment(PHTextView::alignCenter | PHTextView::justifyCenter);
    textView->adjustFontSizeToFit(5);
    view->addSubview(textView);
    textView->release();
    blackoutView = new PHView(fr);
    view->addSubview(blackoutView);
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
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->bgColor = bColor;
	anim->tag=-4444;
	anim->view = blackoutView;
	anim->time = 0.5f;
	anim->callback = (PHCallback)&PHTextController::middleBlackout;
	anim->target = this;
	anim->timeFunction = PHAnimationDescriptor::FadeOutFunction;
	PHView::addAnimation(anim);
	anim->release();
}

void PHTextController::middleBlackout()
{
    textView->setFontSize(0.1f);
    textView->adjustFontSizeToFit(5);
    textView->setText((*strings)[pos]);
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->bgColor = PHClearColor;
	anim->tag=-4444;
	anim->view = blackoutView;
	anim->time = 0.5f;
	anim->callback = (PHCallback)&PHTextController::endBlackout;
	anim->target = this;
	anim->timeFunction = PHAnimationDescriptor::FadeInFunction;
	PHView::addAnimation(anim);
	anim->release();
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
        if (trg&&cb)
            (trg->*cb)(this,ud);
    } else {
        beginBlackout();
    }
}

PHTextController::~PHTextController()
{
    if (strings)
        delete strings;
}
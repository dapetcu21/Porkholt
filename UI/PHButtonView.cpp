/*
 *  PHButtonView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/25/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHImage.h"
#include "PHButtonView.h"
#include "PHNormalImage.h"
#include "PHAnimatedImage.h"

PHButtonView::PHButtonView() : PHView(), imgUp(NULL), imgDown(NULL), _state(StateUp), tgUp(NULL), tgDown(NULL)
{
	setUserInput(true);
};

PHButtonView::PHButtonView(const PHRect &frame) : PHView(frame), imgUp(NULL), imgDown(NULL), _state(StateUp), tgUp(NULL), tgDown(NULL)
{
	setUserInput(true);
};

void PHButtonView::draw()
{
	if (_state == StateDown)
	{
		if (imgDown)
            if (imgDown->isNormal())
                ((PHNormalImage*)imgDown)->renderInFrame(_bounds);
	} else {
		if (imgUp)
            if (imgUp->isNormal())
                ((PHNormalImage*)imgUp)->renderInFrame(_bounds);
	}
}

void PHButtonView::touchEvent(PHTouch * touch)
{
	if (touch->phase() == PHTouch::touchDownState)
	{
		_state = StateDown;
		if (tgDown)
			(tgDown->*cbDown)(this,udDown);
			
	}
	if (touch->phase() == PHTouch::touchUpState)
	{
		_state = StateUp;
		if (tgUp && PHPointInRect(toMyCoordinates(touch->location()),bounds()))
			(tgUp->*cbUp)(this,udUp);
	}
    if (touch->phase() == PHTouch::touchMovedState)
        _state = PHPointInRect(toMyCoordinates(touch->location()),bounds())?StateDown:StateUp;
    if (touch->phase() == PHTouch::touchCancelledState)
        _state = StateUp;
}

void PHButtonView::setPressedImage(PHImage * img)
{ 
    if (img) img->retain(); 
    if (imgDown) imgDown->release(); 
    imgDown=img; 
}

void PHButtonView::setImage(PHImage * img)
{
    if (img) img->retain();
    if (imgUp) imgUp->release();
    imgUp=img; 
}
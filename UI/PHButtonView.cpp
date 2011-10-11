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

void PHButtonView::touchEvent(PHEvent * event)
{
	if (event->type() == PHEvent::touchDown)
	{
		_state = StateDown;
		if (tgDown)
			(tgDown->*cbDown)(this,udDown);
        event->setHandled(true);
	}
	if (event->type() == PHEvent::touchUp)
	{
		_state = StateUp;
		if (tgUp && PHPointInRect(toMyCoordinates(event->location()),bounds()))
			(tgUp->*cbUp)(this,udUp);
        event->setHandled(true);
	}
    if (event->type() == PHEvent::touchMoved)
    {
        _state = PHPointInRect(toMyCoordinates(event->location()),bounds())?StateDown:StateUp;
        event->setHandled(true);
    }
    if (event->type() == PHEvent::touchCancelled)
    {
        _state = StateUp;
        event->setHandled(true);
    }
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
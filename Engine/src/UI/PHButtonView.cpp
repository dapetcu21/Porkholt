/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHImage.h>
#include <Porkholt/UI/PHButtonView.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/Core/PHAnimatedImage.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>

PHButtonView::PHButtonView() : PHView(), imgUp(NULL), imgDown(NULL), _state(StateUp)
{
	setUserInput(true);
};

PHButtonView::PHButtonView(const PHRect &frame) : PHView(frame), imgUp(NULL), imgDown(NULL), _state(StateUp)
{
	setUserInput(true);
};

void PHButtonView::draw()
{
	if (_state == StateDown)
	{
		if (imgDown)
            if (imgDown->isNormal())
                ((PHNormalImage*)imgDown)->renderInFrame(gm, _bounds);
	} else {
		if (imgUp)
            if (imgUp->isNormal())
                ((PHNormalImage*)imgUp)->renderInFrame(gm, _bounds);
	}
}

void PHButtonView::touchEvent(PHEvent * event)
{
	if (event->type() == PHEvent::touchDown)
	{
		_state = StateDown;
        invDown.call(this);
        event->setHandled(true);
	}
	if (event->type() == PHEvent::touchUp)
	{
		_state = StateUp;
		if (invUp.valid() && PHPointInRect(event->drawableLocation()->pointInView(this), bounds()))
			invUp.call(this);
        event->setHandled(true);
	}
    if (event->type() == PHEvent::touchMoved)
    {
        _state = PHPointInRect(event->drawableLocation()->pointInView(this), bounds())?StateDown:StateUp;
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

/*
 *  PHButtonView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/25/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"

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
			imgDown->renderInFrame(_bounds);
	} else {
		if (imgUp)
			imgUp->renderInFrame(_bounds);
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
		if (tgUp)
			(tgUp->*cbUp)(this,udUp);
	}
}

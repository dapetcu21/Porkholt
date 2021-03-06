/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHImage.h>
#include "PHGaugeView.h"
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/Core/PHAnimatedImage.h>
#include <Porkholt/Core/PHImageAnimator.h>

void PHGaugeView::draw()
{
    if (!_image) return;
	PHRect bounds = _bounds;
	bounds.width*=lvl;
	PHRect portion = PHWholeRect;
	portion.width*=lvl;
    
    renderInFramePortionTint(_bounds,PHWholeRect,((tint==PHInvalidColor)?PHWhiteColor:tint).multipliedAlpha(0.5f));
    renderInFramePortionTint(bounds, portion, tint);
}

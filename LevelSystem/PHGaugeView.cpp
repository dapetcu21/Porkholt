/*
 *  PHGaugeView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/22/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHImage.h"
#include "PHGaugeView.h"
#include "PHNormalImage.h"
#include "PHAnimatedImage.h"

void PHGaugeView::draw()
{
	PHRect bounds = _bounds;
	bounds.width*=lvl;
	PHRect portion = PHWholeRect;
	portion.width*=lvl;
	if (_image)
        if (_image->isNormal())
            ((PHNormalImage*)_image)->renderInFramePortion(bounds,portion);
}
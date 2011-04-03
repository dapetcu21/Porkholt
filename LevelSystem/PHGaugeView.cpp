/*
 *  PHGaugeView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/22/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHGaugeView.h"

void PHGaugeView::draw()
{
	PHRect bounds = _bounds;
	bounds.width*=lvl;
	PHRect portion = PHWholeRect;
	portion.width*=lvl;
	if (_image)
		_image->renderInFramePortion(bounds,portion);
}
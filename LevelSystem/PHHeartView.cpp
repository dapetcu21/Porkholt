//
//  PHHeartView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/26/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHHeartView.h"
#include "PHImage.h"


void PHHeartView::draw()
{
    if (!_image) return;
    double height = _bounds.height;
    double width = _image->width()/((double)_image->height())*height;
    for (int i=0; i<heartNo; i++)
    {
        bool isactive = (flipped?(heartNo-i<=active):(i<active));
        renderInFramePortionTint(PHMakeRect(i*width,0,height,width),PHWholeRect,isactive?PHInvalidColor:PHMakeColor(1.0f,1.0f,1.0f,0.5f));
    }
}
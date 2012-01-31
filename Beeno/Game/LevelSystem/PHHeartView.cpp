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
    ph_float height = _bounds.height;
    ph_float width = height/_image->height()*_image->width();
    for (int i=0; i<heartNo; i++)
    {
        bool isactive = (flipped?(heartNo-i<=active):(i<active));
        renderInFramePortionTint(PHRect(i*(width+gap),0,width,height),PHWholeRect,isactive?PHInvalidColor:PHColor(1.0f,1.0f,1.0f,0.5f));
    }
}
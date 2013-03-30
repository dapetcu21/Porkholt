/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHHeartView.h"
#include <Porkholt/Core/PHImage.h>


void PHHeartView::draw()
{
    if (!_image) return;
    float height = _bounds.height;
    float width = height/_image->height()*_image->width();
    for (int i=0; i<heartNo; i++)
    {
        bool isactive = (flipped?(heartNo-i<=active):(i<active));
        renderInFramePortionTint(PHRect(i*(width+gap),0,width,height),PHWholeRect,isactive?PHInvalidColor:PHColor(1.0f,1.0f,1.0f,0.5f));
    }
}

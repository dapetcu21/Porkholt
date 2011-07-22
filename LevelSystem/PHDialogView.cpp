//
//  PHDialogView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHDialogView.h"
#include "PHLNPC.h"

void PHDialogView::touchEvent(PHTouch * t)
{
    if (t->phase() == PHTouch::touchDownState && touchEnabled && delegate)
    {
        delegate->dialogViewFired(this);
        touchEnabled = false;
    }
}



void PHDialogView::draw()
{
    if (!_image) return;
    if (!stretch)
    {
        PHImageView::draw();
        return;
    }
    double aspectRatio = _image->width()/_image->height()*2;
    double aspectRatioC = _bounds.width/_bounds.height;
    if (aspectRatioC/aspectRatio >1.0f)
    {
        double marginLen = aspectRatio*_bounds.height/2;
        double toRecover = _bounds.width-marginLen*2;
        
        renderInFramePortionTint(PHMakeRect(0, 0, marginLen, _bounds.height), PHMakeRect(0, 0, 0.5, 1.0), tint);
        renderInFramePortionTint(PHMakeRect(marginLen+toRecover, 0, marginLen, _bounds.height), PHMakeRect(0.5, 0, 0.5, 1.0), tint);
        renderInFramePortionTint(PHMakeRect(marginLen, 0, toRecover, _bounds.height), PHMakeRect(0.5, 0, 0, 1.0), tint);
    } else
        PHImageView::draw();
}
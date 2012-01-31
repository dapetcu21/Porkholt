//
//  PHDialogView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHDialogView.h"
#include "PHLNPC.h"

void PHDialogView::touchEvent(PHEvent * t)
{
    if (t->type() == PHEvent::touchDown && touchEnabled && delegate)
    {
        delegate->dialogViewFired(this);
        touchEnabled = false;
        t->setHandled(true);
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
    ph_float aspectRatio = _image->width()/_image->height();
    ph_float aspectRatioC = _bounds.width/_bounds.height;
    if (aspectRatioC/aspectRatio >1.0f)
    {
        ph_float marginLen = aspectRatio*_bounds.height/2;
        ph_float toRecover = _bounds.width-marginLen*2;
        
        renderInFramePortionTint(PHRect(0, 0, marginLen, _bounds.height), PHRect(0, 0, 0.5, 1.0), tint);
        renderInFramePortionTint(PHRect(marginLen+toRecover, 0, marginLen, _bounds.height), PHRect(0.5, 0, 0.5, 1.0), tint);
        renderInFramePortionTint(PHRect(marginLen, 0, toRecover, _bounds.height), PHRect(0.5, 0, 0, 1.0), tint);
    } else
        PHImageView::draw();
}
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
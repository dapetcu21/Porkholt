//
//  PHPlayerView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/16/11.
//  Copyright 2011 Home. All rights reserved.
//


#include "PHMain.h"
#include <cmath>

void PHPlayerView::setRotation( double rot)
{
    double oldRot = _rotation;
    PHView::setRotation(rot);
    double dif = rot-oldRot;
    while (abs(dif+360)<abs(dif))
        dif+=360;
    while (abs(dif-360)<abs(dif))
        dif-=360;
    int fps = PHMainEvents::sharedInstance()->framesPerSecond();
    lastDif = (lastDif+dif)/2;
    for (ViewEl * ve = viewsSt; ve; ve=ve->next)
    {
        if (ve->el == _designatedView) continue;
        ve->el->setRotation(ve->el->rotation()-lastDif);
    }
}
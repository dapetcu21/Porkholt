//
//  PHPlayerView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//


#include "PHPlayerView.h"
#include "PHGameManager.h"

void PHPlayerView::setRotation( ph_float rot)
{
    ph_float oldRot = _rotation;
    PHView::setRotation(rot);
    ph_float dif = rot-oldRot;
    while (abs(dif+360)<abs(dif))
        dif+=360;
    while (abs(dif-360)<abs(dif))
        dif-=360;
    ph_float llastDif = lastDif;
    ph_float newDif = elapsed?((dif/elapsed)*0.01666):lastDif;
    PHLowPassFilter(lastDif, newDif, elapsed, 50.0f);
    for (list<PHView*>::iterator i = views.begin(); i!= views.end(); i++)
    {
        if ((*i)->tag() != _designatedTag) continue;
        (*i)->setRotation((*i)->rotation()-dif+lastDif-llastDif);
    }
}
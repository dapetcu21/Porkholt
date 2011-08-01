//
//  PHPlayerView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//


#include "PHPlayerView.h"
#include "PHMainEvents.h"

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
    PHLowPassFilter(lastDif, dif, 1.0f/fps, 50.0f);
#ifdef PHVIEW_STD_LIST
    for (list<PHView*>::iterator i = views.begin(); i!= views.end(); i++)
    {
        if ((*i)->tag() != _designatedTag) continue;
        (*i)->setRotation((*i)->rotation()-lastDif);
    }
#else
    for (ViewEl * ve = viewsSt; ve; ve=ve->next)
    {
        if (ve->el->tag() != _designatedTag) continue;
        ve->el->setRotation(ve->el->rotation()-lastDif);
    }
#endif
}
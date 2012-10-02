/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHGameView.h"
#include <Porkholt/Core/PHDrawableCoordinates.h>

#define INIT t1(NULL), t2(NULL)

PHGameView::PHGameView() : INIT, p1(0,0), p2(0,0) {}
PHGameView::PHGameView(const PHRect & f) : PHView(f), INIT, p1(0,f.height/2), p2(0,f.height/2) {};

void PHGameView::touchEvent(PHEvent *evt)
{
    switch (evt->type())
    {
        case PHEvent::touchDown:
        {
            PHPoint p = evt->drawableLocation()->pointInView(this);
            if (p.x <boundsCenter().x)
            {
                if (!t1)
                {
                    t1 = evt->userData();
                    p1 = p;
                    evt->setHandled(true);
                }
            } else {
                if (!t2)
                {
                    t2 = evt->userData();
                    p2 = p;
                    evt->setHandled(true);
                }
            }
            break;
        }
        case PHEvent::touchMoved:
        {
            PHPoint p = evt->drawableLocation()->pointInView(this);
            if (evt->userData()==t1)
                p1 = p;
            if (evt->userData()==t2)
                p2 = p;
            break;
        }
        case PHEvent::touchUp:
        case PHEvent::touchCancelled:
        {
            if (evt->userData()==t1)
                t1 = NULL;
            if (evt->userData()==t2)
                t2 = NULL;
            break;
        }
    }
};

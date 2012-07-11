/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHGameView.h"

#define INIT t1(NULL), t2(NULL)

PHGameView::PHGameView() : INIT, p1(0,0), p2(0,0) {}
PHGameView::PHGameView(const PHRect & f) : PHView(f), INIT, p1(0,f.height/2), p2(0,f.height/2) {};

void PHGameView::touchEvent(PHEvent *evt)
{
    switch (evt->type())
    {
        case PHEvent::touchDown:
        {
            PHPoint p = toMyCoordinates(evt->location());
            if (p.x <boundsCenter().x)
            {
                if (!t1)
                {
                    t1 = evt;
                    p1 = p;
                    evt->setHandled(true);
                }
            } else {
                if (!t2)
                {
                    t2 = evt;
                    p2 = p;
                    evt->setHandled(true);
                }
            }
            break;
        }
        case PHEvent::touchMoved:
        {
            PHPoint p = toMyCoordinates(evt->location());
            if (evt==t1)
                p1 = p;
            if (evt==t2)
                p2 = p;
            break;
        }
        case PHEvent::touchUp:
        case PHEvent::touchCancelled:
        {
            if (evt==t1)
                t1 = NULL;
            if (evt==t2)
                t2 = NULL;
            break;
        }
    }
};

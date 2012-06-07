/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHTextControllerView.h"
#include "PHTextController.h"

PHTextControllerView::PHTextControllerView(PHTextController * del) : PHView(), delegate(del)
{
    
}

void PHTextControllerView::touchEvent(PHEvent * event)
{
    if (event->type() == PHEvent::touchDown)
    {
        delegate->advance();
        event->setHandled(true);
    }
}

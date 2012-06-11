/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/UI/PHTextControllerView.h>
#include <Porkholt/UI/PHTextController.h>

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

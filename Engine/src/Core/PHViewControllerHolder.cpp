/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PHViewControllerHolder.h>
#include <Porkholt/Core/PHViewController.h>

PHViewControllerHolder::PHViewControllerHolder() : vc(NULL)
{
    setAutoresizesSubviews(true);
}


PHViewControllerHolder::PHViewControllerHolder(const PHRect & fr) : PHView(fr), vc(NULL)
{
    setAutoresizesSubviews(true);
    setUserInput(true);
}

PHViewControllerHolder::~PHViewControllerHolder()
{
    if (vc)
        vc->release();
}

void PHViewControllerHolder::setViewController(PHViewController * c)
{
    if (c)
    {
        c->retain();
        if (gm)
            c->_viewWillAppear();
        addChild(c->getView());
        c->getView()->setAutoresizeMask(PHView::ResizeAll);
        if (gm)
            c->_viewDidAppear();
    }

    if (vc)
    {
        if (gm)
            vc->_viewWillDisappear();
        vc->getView()->removeFromParent();
        if (gm)
            vc->_viewDidDisappear();
        vc->release();
    }
    vc = c;
}

void PHViewControllerHolder::attachedToGameManager()
{
    if (vc)
    {
        vc->_viewWillAppear();
        vc->_viewWillAppear();
    }
}

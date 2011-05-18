//
//  PHAuxLayerView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/18/11.
//  Copyright 2011 Home. All rights reserved.
//

#include "PHMain.h"

map<int,PHAuxLayerView*> PHAuxLayerView::ilist;

PHAuxLayerView::PHAuxLayerView(int name) : PHView(), _name(name)
{
    ilist[_name] = this;
    setOptimizations(false);
}

PHAuxLayerView::~PHAuxLayerView()
{
    map<int,PHAuxLayerView*>::iterator it = ilist.find(_name);
    if (it!=ilist.end())
    {
        ilist.erase(it);
    }
    while (!views.empty())
        views.front()->bindToAuxLayer(NULL, NULL);
}

PHAuxLayerView * PHAuxLayerView::auxLayerViewWithName(int n)
{
    map<int,PHAuxLayerView*>::iterator it = ilist.find(n);
    if (it==ilist.end())
        return NULL;
    return it->second;
}

void PHAuxLayerView::addView(PHView * view)
{
    views.push_back(view);
}

void PHAuxLayerView::removeView(PHView * view)
{
    for (list<PHView*>::iterator i = views.begin(); i!=views.end(); i++)
        if (*i==view)
        {
            views.erase(i);
            return;
        }
}

void PHAuxLayerView::draw()
{
    for (list<PHView*>::iterator i = views.begin(); i!=views.end(); i++)
    {
        glPushMatrix();
        PHView * view = *i;
        if (view->superview()!=view->auxSuperview)
            view->superview()->loadMatrixTree(view->auxSuperview);
        view->drawingOnAuxLayer = true;
        view->auxRender();
        view->drawingOnAuxLayer = false;
        glPopMatrix();
    }
}
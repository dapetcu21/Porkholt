/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHTITLESCREEN_H
#define PHTITLESCREEN_H

#include <Porkholt/Core/PHViewController.h>

class PHTitleScreen : public PHViewController
{
protected:
    virtual PHView * loadView(const PHRect & frame);
    void buttonPressed(PHObject * sender, void * ud);
    
public:
    void setBackButton(PHInvocation inv);
    PHTitleScreen(PHGameManager * gm) : PHViewController(gm) {} 
};

#endif

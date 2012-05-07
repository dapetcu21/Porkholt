//
//  PHTitleScreen.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 8/20/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHTITLESCREEN_H
#define PHTITLESCREEN_H

#include "PHViewController.h"

class PHTitleScreen : public PHViewController
{
protected:
    virtual PHView * loadView(const PHRect & frame);
    void buttonPressed(PHObject * sender, void * ud);
    
public:
    void setBackButton(PHInvocation inv);
};

#endif

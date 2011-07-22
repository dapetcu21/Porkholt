//
//  PHDialogView.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHDIALOGVIEW_H
#define PHDIALOGVIEW_H

#include "PHImageView.h"

class PHLNPC;
class PHDialogView : public PHImageView
{
private:
    PHLNPC * delegate;
    bool touchEnabled;
    virtual void touchEvent(PHTouch * t);
    virtual void draw();
public:
    PHDialogView(PHLNPC * d) : delegate(d), touchEnabled(true) {}
    void reenableTouch() { touchEnabled = true; }
};

#endif
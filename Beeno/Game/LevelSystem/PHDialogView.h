/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHDIALOGVIEW_H
#define PHDIALOGVIEW_H

#include <Porkholt/Core/PHImageView.h>

class PHLNPC;
class PHDialogView : public PHImageView
{
private:
    PHLNPC * delegate;
    bool touchEnabled;
    virtual void touchEvent(PHEvent * t);
    virtual void draw();
    bool stretch;
public:
    PHDialogView(PHLNPC * d) : delegate(d), touchEnabled(true), stretch(false) {}
    void reenableTouch() { touchEnabled = true; }
    void setStretchBubble(bool s) { stretch = s; }
    bool stretchBubble() { return stretch; }
};

#endif

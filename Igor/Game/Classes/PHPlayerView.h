/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHPLAYERVIEW_H
#define PHPLAYERVIEW_H

#include "PHImageView.h"

class PHPlayerView : public PHImageView
{
protected:
    void attachedToGameManager();
public:
    PHPlayerView();
    PHPlayerView(const PHRect & f);
};

#endif

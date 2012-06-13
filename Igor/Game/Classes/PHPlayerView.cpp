/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHPlayerView.h"

void PHPlayerView::attachedToGameManager()
{
    PHImageView::attachedToGameManager();
    if (!image())
        setImage(gm->imageNamed("player"));
}

PHPlayerView::PHPlayerView() {}
PHPlayerView::PHPlayerView(const PHRect & f) : PHImageView(f) {}

//
//  PHPlayerView.cpp
//  Igor
//
//  Created by Marius Petcu on 2/16/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHPlayerView.h"

void PHPlayerView::attachedToGameManager()
{
    if (!image())
        setImage(_gameManager->imageNamed("player"));
}

PHPlayerView::PHPlayerView() {}
PHPlayerView::PHPlayerView(const PHRect & f) : PHImageView(f) {}
//
//  PHPlayerView.h
//  Igor
//
//  Created by Marius Petcu on 2/16/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

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

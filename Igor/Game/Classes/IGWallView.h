/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGWALLVIEW_H
#define IGWALLVIEW_H

#include <Porkholt/Core/PHImageView.h>

class IGWallView : public PHImageView 
{
    public:
        IGWallView();
        IGWallView(const PHRect & r);
        ~IGWallView();
};

#endif

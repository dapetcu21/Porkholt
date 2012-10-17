/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGBACKGROUND_H
#define IGBACKGROUND_H

#include <Porkholt/Core/PHViewController.h>

class IGBackground : public PHViewController
{
    protected:
    public:
        IGBackground(PHGameManager * gm);
        ~IGBackground();
        
        PHView * loadView(const PHRect & f);

        void updateScene(ph_float elapsed);
};

#endif

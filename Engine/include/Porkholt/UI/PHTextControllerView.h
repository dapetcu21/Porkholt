/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHTEXTCONTROLLERVIEW_H
#define PHTEXTCONTROLLERVIEW_H

#include <Porkholt/Core/PHView.h>

class PHTextController;
class PHTextControllerView : public PHView
{
private:
    PHTextController * delegate;
public:
    PHTextControllerView(PHTextController * delegate);
    void touchEvent(PHEvent * touch);
};

#endif

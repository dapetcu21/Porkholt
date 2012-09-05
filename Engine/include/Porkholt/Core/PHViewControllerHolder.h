/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHVIEWCONTROLLERHOLDER_H
#define PHVIEWCONTROLLERHOLDER_H

#include <Porkholt/Core/PHView.h>

class PHViewController;
class PHViewControllerHolder : public PHView 
{
    protected:
        void attachedToGameManager();
        PHViewController * vc;

    public:
        void setViewController(PHViewController * vc);
        PHViewController * viewController() { return vc; }

        PHViewControllerHolder();
        PHViewControllerHolder(const PHRect & frame);
        ~PHViewControllerHolder();
};

#endif

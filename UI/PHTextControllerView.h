//
//  PHTextControllerView.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/17/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHTEXTCONTROLLERVIEW_H
#define PHTEXTCONTROLLERVIEW_H

#include "PHView.h"

class PHTextController;
class PHTextControllerView : public PHView
{
private:
    PHTextController * delegate;
public:
    PHTextControllerView(PHTextController * delegate);
    void touchEvent(PHTouch * touch);
};

#endif
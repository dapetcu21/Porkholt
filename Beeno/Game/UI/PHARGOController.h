//
//  PHARGOController.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 12/29/11.
//  Copyright (c) 2011 Porkholt Labs!. All rights reserved.
//

#ifndef Porkholt_iOS_PHARGOController_h
#define Porkholt_iOS_PHARGOController_h

#include "PHViewController.h"

class PHImageView;

class PHARGOController : public PHViewController
{
protected:
    PHImageView * imageView;
    
    virtual PHView * loadView(const PHRect & frame);
    virtual void updateScene(double timeElapsed);
};

#endif

//
//  PHLLevelEnd.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/26/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLLEVELEND_H
#define PHLLEVELEND_H

#include "PHLSensor.h"

class PHLLevelEnd : public PHLSensor
{
public:
    PHLLevelEnd();
private:
    virtual void objectEntered(PHLObject * obj);
};

#endif

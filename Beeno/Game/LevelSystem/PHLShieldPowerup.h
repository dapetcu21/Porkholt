//
//  PHLShieldPowerup.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/25/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLSHIELDPOWERUP_H
#define PHLSHIELDPOWERUP_H

#include "PHLPowerup.h"

class PHLShieldPowerup : public PHLPowerup
{
public:
    PHLShieldPowerup();
private:
    virtual void collected();
};

#endif

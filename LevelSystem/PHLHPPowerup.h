//
//  PHLHPPowerup.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/29/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLHPPOWERUP_H
#define PHLHPPOWERUP_H

#include "PHLPowerup.h"

class PHLHPPowerup : public PHLPowerup
{
public:
    PHLHPPowerup();
private:
    virtual void collected();
};

#endif
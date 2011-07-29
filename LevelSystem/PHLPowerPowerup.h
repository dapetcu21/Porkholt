//
//  PHLPowerPowerup.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/29/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLPOWERPOWERUP_H
#define PHLPOWERPOWERUP_H

#include "PHLPowerup.h"

class PHLPowerPowerup : public PHLPowerup
{
public:
    PHLPowerPowerup();
private:
    virtual void collected();
};

#endif

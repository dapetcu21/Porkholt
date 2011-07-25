//
//  PHLGround.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/25/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLGROUND_H
#define PHLGROUND_H

#include "PHLObject.h"

class PHLGround : public PHLObject
{
public:
    PHLGround();
    virtual ~PHLGround();
    
    virtual void contactBegin(bool b,b2Contact* contact);
    virtual void contactEnd(bool b,b2Contact* contact);
};

#endif

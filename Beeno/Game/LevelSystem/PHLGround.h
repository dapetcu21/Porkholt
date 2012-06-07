/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

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

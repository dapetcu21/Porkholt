//
//  PHLPlatform.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/24/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLPLATFORM_H
#define PHLPLATFORM_H

#include "PHLObject.h"

class b2World;
class b2Body;
class b2Contact;
class b2Manifold;
class b2ContactImpulse;

class PHLPlatform : public PHLObject
{
protected:
    set<b2Contact*> contacts;
public:
	PHLPlatform();
	virtual ~PHLPlatform();
        
    virtual void contactPreSolve(bool b,b2Contact* contact, const b2Manifold* oldManifold);
	virtual void contactEnd(bool b,b2Contact * contact);
};

#endif

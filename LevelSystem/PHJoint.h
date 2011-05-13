//
//  PHJoint.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/13/11.
//  Copyright 2011 Home. All rights reserved.
//

#ifndef PHJOINT_H
#define PHJOINT_H
#define INSIDE_PHJOINT_H
#include "PHMain.h"

class b2Joint;
class PHWorld;

class PHJoint : public PHObject
{
protected:
    b2Joint * joint;
    PHWorld * world;
    PHLObject * body1;
    PHLObject * body2;
    bool worldCoord;
    bool collideConnected;
public:
    PHJoint(PHWorld * world);
    virtual ~PHJoint();
    static PHJoint * jointWithClassAndWorld(string clss, PHWorld * world);
    virtual void loadFromLua(void * L);
    void destroyJoint();
    virtual void recreateJoint();
    
    PHLObject * object1() {return body1;};
    PHLObject * object2() {return body2;};
    
    void setObject1(PHLObject*obj);
    void setObject2(PHLObject*obj);
};
#undef INSIDE_PHJOINT_H
#endif
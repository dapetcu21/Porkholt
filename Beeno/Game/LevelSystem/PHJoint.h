/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHJOINT_H
#define PHJOINT_H
#include "PHMain.h"

class b2Joint;
class PHWorld;

#include "PHLObject.h"

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
    virtual void loadFromLua(lua_State * L);
    void destroyJoint();
    virtual void recreateJoint();
    
    PHLObject * object1() {return body1;};
    PHLObject * object2() {return body2;};
    
    void setObject1(PHLObject*obj);
    void setObject2(PHLObject*obj);
};
#endif

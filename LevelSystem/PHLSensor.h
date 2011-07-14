//
//  PHLSensor.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/14/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLSENSOR_H
#define PHLSENSOR_H

#include "PHLObject.h"

class PHLSensor : public PHLObject
{
public:
    PHLSensor();
private:
    bool customizeFixture(lua_State * L, b2FixtureDef & fixtureDef);
    
    void contactBegin(bool b,b2Contact* contact);
    void contactEnd(bool b,b2Contact* contact);

    void _objectEntered(PHLObject * obj);
    void _objectExited(PHLObject * obj);
    
    virtual void objectEntered(PHLObject * obj);
    virtual void objectExited(PHLObject * obj);
};

#endif
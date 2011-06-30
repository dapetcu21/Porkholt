//
//  PHLAuxLayer.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/17/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLAUXLAYER_H
#define PHLAUXLAYER_H

#include "PHLObject.h"

class b2World;
class PHAuxLayerView;

class PHLAuxLayer : public PHLObject
{
protected:
    int name;
public:
	PHLAuxLayer();
	virtual ~PHLAuxLayer();
	
	virtual void loadFromLua(void * L, const string & root,b2World * world);
	virtual void loadView();
	
};

#endif

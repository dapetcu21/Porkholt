//
//  PHLAuxLayer.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/17/11.
//  Copyright 2011 Home. All rights reserved.
//

#ifndef INSIDE_PHLOBJECT_H
#ifndef PHLAUXLAYER_H
#define PHLAUXLAYER_H

#include "PHMain.h"

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
#endif

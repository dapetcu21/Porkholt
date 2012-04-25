//
//  PHProjectionChanger.cpp
//  Porkholt
//
//  Created by Marius Petcu on 4/13/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHProjectionChanger.h"

void PHProjectionChanger::render()
{
	if (enabled)
	{
		PHMatrix o = gm->projectionMatrix();
		gm->setProjectionMatrix(m);
		PHDrawable::render();
		gm->setProjectionMatrix(o);
	} else
		PHDrawable::render();
}
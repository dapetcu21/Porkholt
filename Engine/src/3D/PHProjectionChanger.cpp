/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/3D/PHProjectionChanger.h>

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

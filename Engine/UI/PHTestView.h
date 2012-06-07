/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHTESTVIEW_H
#define PHTESTVIEW_H

#include "PHView.h"

class PHTestView: public PHView 
{
public:
	PHTestView() : PHView() {};
	PHTestView(const PHRect &frame) : PHView(frame) {};
	
protected:
	virtual void draw();
	virtual void touchEvent(PHEvent * touch);
	
};

#endif

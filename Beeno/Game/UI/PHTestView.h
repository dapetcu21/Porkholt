/*
 *  PHTestView.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/12/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

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
/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHTESTVIEWCONTROLLER_H
#define PHTESTVIEWCONTROLLER_H

#include "PHViewController.h"

class PHTestViewController : public PHViewController
{
private:
	PHView * view4;
	PHView * view2;
protected:
	virtual PHView * loadView(const PHRect & frame);
public:
	virtual void updateScene(ph_float timeElapsed);
	
	void callback1(PHObject * sender, void * ud);
	void callback2(PHObject * sender, void * ud);
    void callback3(PHObject * sender, void * ud);
};
#endif

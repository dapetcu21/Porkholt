/*
 *  PHTestViewController.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/22/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef INSIDE_PHVIEWCONTROLLER_H
#ifndef PHTESTVIEWCONTROLLER_H
#define PHTESTVIEWCONTROLLER_H

#include "PHMain.h"

class PHTestViewController : public PHViewController
{
private:
	PHView * view4;
	PHView * view2;
protected:
	virtual PHView * loadView(const PHRect & frame);
public:
	virtual void updateScene(double timeElapsed);
	
	void callback1(PHObject * sender, void * ud);
	void callback2(PHObject * sender, void * ud);
};
#endif
#endif
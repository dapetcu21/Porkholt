/*
 *  PHViewController.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/22/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHVIEWCONTROLLER_H
#define PHVIEWCONTROLLER_H
#define INSIDE_PHVIEWCONTROLLER_H

#include "PHMain.h"

class PHViewController : public PHObject
{
private:
	PHRect _frame;
protected:
	PHView * view;
	virtual PHView * loadView(const PHRect & frame);
public:
	PHView * getView() { return view; };
	PHViewController(const PHRect & frame);
	PHViewController();
	void init();
	
	virtual void updateScene(double timeElapsed);
};

#undef INSIDE_PHVIEWCONTROLLER_H
#endif
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

class PHNavigationController;

class PHViewController : public PHObject
{
private:
protected:
	PHView * view;
	virtual PHView * loadView(const PHRect & frame);
	PHNavigationController * navController;
	friend class PHNavigationController;
	friend class PHMainEvents;
public:
	PHView * getView() { return view; };
	PHViewController() : navController(NULL),view(NULL) {};
	virtual ~PHViewController();
	
	void init();
	void init(const PHRect & frame);
	
	virtual void updateScene(double timeElapsed);
	
	virtual void viewDidAppear();
	virtual void viewWillAppear();
	virtual void viewWillDisappear();
	virtual void viewDidDisappear();
};

#undef INSIDE_PHVIEWCONTROLLER_H
#endif
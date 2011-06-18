/*
 *  PHViewController.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/22/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
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
	int viewState;
public:
	PHView * getView() { return view; };
	PHViewController() : navController(NULL),view(NULL),viewState(StateNotAppeared) {};
	virtual ~PHViewController();
	
	void init();
	void init(const PHRect & frame);
	
	virtual void updateScene(double timeElapsed);
	
	enum ViewStates
	{
		StateNotAppeared,
		StateAppearing,
		StateAppeared,
		StateDisappearing,
		NUMVIEWSTATES
	};
	
	int getViewState() { return viewState; };
	void _viewDidAppear()
	{
		if (viewState==StateAppearing)
		{
			viewState = StateAppeared;
			viewDidAppear();
		}
	}
	void _viewWillAppear()
	{
		if (viewState==StateNotAppeared)
		{
			viewState = StateAppearing;
			viewWillAppear();
		}
	}
	void _viewWillDisappear()
	{
		if (viewState==StateAppeared)
		{
			viewState = StateDisappearing;
			viewWillDisappear();
		}
	}
	void _viewDidDisappear()
	{
		if (viewState==StateDisappearing)
		{
			viewState = StateNotAppeared;
			viewWillDisappear();
		}
	}
	
	virtual void viewDidAppear();
	virtual void viewWillAppear();
	virtual void viewWillDisappear();
	virtual void viewDidDisappear();
};

#undef INSIDE_PHVIEWCONTROLLER_H
#endif
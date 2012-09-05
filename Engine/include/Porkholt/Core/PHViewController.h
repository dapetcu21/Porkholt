/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHVIEWCONTROLLER_H
#define PHVIEWCONTROLLER_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHAnimator.h>

class PHNavigationController;

class PHViewController : public PHAnimator 
{
private:
protected:
	PHView * view;
	virtual PHView * loadView(const PHRect & frame);
	PHNavigationController * navController;
	friend class PHNavigationController;
	friend class PHGameManager;
	int viewState;
    bool rmNav;
    
    void setNavigationController(PHNavigationController * nc);
    virtual void updateScene(ph_float timeElapsed);
    
    PHGameManager * gm;
    
public:
	PHView * getView() { return view; };
	PHViewController() : view(NULL), navController(NULL), viewState(StateNotAppeared), rmNav(false) {};
	virtual ~PHViewController();
	
	void init(PHGameManager * gameManager);
	void init(PHGameManager * gameManager, const PHRect & frame);
	
    PHNavigationController * navigationController() { return navController; }
    
	virtual void advanceAnimation(ph_float timeElapsed);
	
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
    
    void setGameManager(PHGameManager * gameManager) { gm = gameManager; }
    PHGameManager * gameManager() { return gm; }
    
private:
	virtual void viewDidAppear();
	virtual void viewWillAppear();
	virtual void viewWillDisappear();
	virtual void viewDidDisappear();
    
    virtual PHColor fadeToColor() { return PHInvalidColor; }
};

#include <Porkholt/Core/PHNavigationController.h>

#endif

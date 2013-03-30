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
    PHRect ir;
    
    virtual void updateScene(float timeElapsed);
    
    PHGameManager * gm;
    
public:
	virtual ~PHViewController();
	
    PHViewController(PHGameManager * gameManager);
	
	PHView * getView(); 
    void setInitialViewFrame(const PHRect & r) { ir = r; }

    PHNavigationController * navigationController() { return navController; }
    void setNavigationController(PHNavigationController * nc);
    
	virtual void advanceAnimation(float timeElapsed);
	
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

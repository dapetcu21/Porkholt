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

#include "PHMain.h"
#include "PHView.h"

class PHNavigationController;

class PHViewController : public PHObject
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
    virtual void updateScene(double timeElapsed);
    
    set<PHViewController*> managedControllers;
    
    PHGameManager * _gameManager;
    
public:
	PHView * getView() { return view; };
	PHViewController() : navController(NULL),view(NULL),viewState(StateNotAppeared),rmNav(false) {};
	virtual ~PHViewController();
	
	void init(PHGameManager * gm);
	void init(PHGameManager * gm, const PHRect & frame);
	
    PHNavigationController * navigationController() { return navController; }
    
	virtual void _updateScene(double timeElapsed);
	
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
            for(set<PHViewController*>::iterator i = managedControllers.begin(); i!= managedControllers.end(); i++)
                (*i)->_viewDidAppear();
			viewDidAppear();
		}
	}
	void _viewWillAppear()
	{
		if (viewState==StateNotAppeared)
		{
			viewState = StateAppearing;
            for(set<PHViewController*>::iterator i = managedControllers.begin(); i!= managedControllers.end(); i++)
                (*i)->_viewWillAppear();
			viewWillAppear();
		}
	}
	void _viewWillDisappear()
	{
		if (viewState==StateAppeared)
		{
			viewState = StateDisappearing;
            for(set<PHViewController*>::iterator i = managedControllers.begin(); i!= managedControllers.end(); i++)
                (*i)->_viewWillDisappear();
			viewWillDisappear();
		}
	}
	void _viewDidDisappear()
	{
		if (viewState==StateDisappearing)
		{
			viewState = StateNotAppeared;
            for(set<PHViewController*>::iterator i = managedControllers.begin(); i!= managedControllers.end(); i++)
                (*i)->_viewDidDisappear();
			viewWillDisappear();
		}
	}
    
    void manageViewController(PHViewController *vc);
    void stopManagingViewController(PHViewController *vc);
    
    void setGameManager(PHGameManager * gm) { _gameManager = gm; }
    PHGameManager * gameManager() { return _gameManager; }
    
private:
	virtual void viewDidAppear();
	virtual void viewWillAppear();
	virtual void viewWillDisappear();
	virtual void viewDidDisappear();
    
    virtual PHColor fadeToColor() { return PHInvalidColor; }
};

#include "PHNavigationController.h"

#endif
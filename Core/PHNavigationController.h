/*
 *  PHNavigationController.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/25/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHNAVIGATIONCONTROLLER_H
#define PHNAVIGATIONCONTROLLER_H

#include "PHMain.h"

class PHNavigationController : public PHViewController
{
private:
	list<PHViewController*> stack;
	PHViewController * currentVC, * lastVC;
	int animation;
	void startAnimating();
	void stopAnimating();
public:
	PHNavigationController() : currentVC(NULL),lastVC(NULL) {};
	
	enum Animations
	{
		NoAnim = 0
	};
	void pushViewController(PHViewController * vc, int anim, bool replace);
	void popViewController(int anim);
	void pushViewController(PHViewController * vc, int anim) { pushViewController(vc, anim, false); };
	void pushViewController(PHViewController * vc)  { pushViewController(vc, NoAnim, false); };
	void popViewController() { popViewController(NoAnim); }
	
	virtual void updateScene(double timeElapsed);
};

#endif
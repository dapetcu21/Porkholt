/*
 *  PHNavigationController.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/25/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef INSIDE_PHVIEWCONTROLLER_H
#ifndef PHNAVIGATIONCONTROLLER_H
#define PHNAVIGATIONCONTROLLER_H

#include "PHMain.h"

class PHNavigationController : public PHViewController
{
private:
	list<PHViewController*> stack;
	PHViewController * currentVC, * lastVC;
	PHView * fadeView;
	PHColor _fadeColor;
	int animation;
	bool hidden;
	
	void startAnimating();
	void stopAnimating();
	void startSlideAnimation(double x, double y);
	void endSlideAnimation(void * dmy);
	void cancelAnimation();
	void startFadeAnimation();
	void middleFadeAnimation();
	void endFadeAnimation();
	
	friend class PHAnimationDescriptor;
public:
	PHNavigationController() : currentVC(NULL),lastVC(NULL),_fadeColor(PHBlackColor),fadeView(NULL),hidden(true) {};
	enum Animations
	{
		NoAnim = 0,
		SlideLeft,
		SlideRight,
		SlideUp,
		SlideDown,
		FadeToColor,
		NUMANIMATIONS
	};
	void pushViewController(PHViewController * vc, int anim, bool replace);
	void popViewController(int anim);
	void pushViewController(PHViewController * vc, int anim) { pushViewController(vc, anim, false); };
	void pushViewController(PHViewController * vc)  { pushViewController(vc, NoAnim, false); };
	void popViewController() { popViewController(NoAnim); }
	
	virtual void updateScene(double timeElapsed);
	
	virtual void viewDidDisappear();
	virtual void viewWillAppear();
	
	PHColor fadeColor() { return _fadeColor; };
	void setFadeColor(const PHColor & c) { _fadeColor = c; };
};

#endif
#endif
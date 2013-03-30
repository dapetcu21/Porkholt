/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHNAVIGATIONCONTROLLER_H
#define PHNAVIGATIONCONTROLLER_H

#include <Porkholt/Core/PHViewController.h>

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
	void startSlideAnimation(float x, float y);
	void endSlideAnimation(void * dmy);
	void cancelAnimation();
	void startFadeAnimation();
	void middleFadeAnimation();
	void endFadeAnimation();
	
	friend class PHAnimationDescriptor;
public:
	PHNavigationController(PHGameManager * gm) : PHViewController(gm), currentVC(NULL), lastVC(NULL), fadeView(NULL), _fadeColor(PHBlackColor), animation(NoAnim), hidden(true) {};
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
	
	virtual void viewDidDisappear();
	virtual void viewWillAppear();
	
	PHColor fadeColor() { return _fadeColor; };
	void setFadeColor(const PHColor & c) { _fadeColor = c; };

    ~PHNavigationController();
};

#endif

/*
 *  PHView.h
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/17/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#ifndef PHVIEW_H
#define PHVIEW_H
#define INSIDE_PHVIEW_H

#include "PHMain.h"
#include "PHAnimationDescriptor.h"
#include <list>

class PHView : public PHObject
{
protected:
	struct ViewEl;
	ViewEl * viewsSt;
	ViewEl * viewsEn;
	ViewEl * currPos;
	PHView * superView;
	PHRect _frame,_bounds;
	PHPoint _rotationalCenter,_scalingCenter;
	double _rotation,_scaleX,_scaleY;
	double _alpha;
	bool _userInput;
	PHColor _backColor;
	int effOrder;

	void render();
	void drawBackground();
	virtual void draw();
	
	void apply_rotation();
	void apply_scaling();
	void applyMatrices();
	void loadMatrixTree();
	
	virtual void touchEvent(PHTouch * touch);
	
public:
	
	enum EffectOrder
	{
		EffectOrderScaleRotate = 0,
		EffectOrderRotateScale
	};
	
	PHView();
	PHView(const PHRect &frame);
	
	void setFrame(const PHRect &frame);
	PHRect frame() const { return _frame; }
	PHRect bounds() const { return _bounds; }
	PHPoint center() const;
	void setCenter(const PHPoint &center);
	PHPoint boundsCenter() const;
	PHPoint rotationalCenter() const {return _rotationalCenter;}
	PHPoint scalingCenter() const {return _scalingCenter; }
	void setRotationalCenter(const PHPoint &center) { _rotationalCenter = center;}
	void setScalingCenter(const PHPoint &center) { _scalingCenter = center; }
	double rotation() { return _rotation; };
	void setRotation(double rot) { _rotation = rot; };
	double scaleX() { return _scaleX; };
	void setScaleX(double scale) { _scaleX = scale; };
	double scaleY() { return _scaleY; };
	void setScaleY(double scale) { _scaleY = scale; };
	int effectOrder() { return effOrder; };
	void setEffectOrder(int eff) { effOrder = eff; };
	void rotate(double rot) { _rotation+= rot; };
	void setAlpha(double alpha) { _alpha = alpha; if (alpha<0) alpha = 0; if (alpha>1) alpha = 1; };
	double alpha() { return _alpha; }
	void setBackgroundColor(const PHColor &color) { _backColor = color; };
	PHColor backgroundColor() { return _backColor; };
	void setUserInput(bool ui) { _userInput = ui; };
	bool userInput() { return _userInput; };
	PHView * superview() { return superView; };
	
	void addSubview(PHView * view);
	void removeFromSuperview();
	void bringToFront();

	PHPoint toMyCoordinates(PHPoint pnt);
	void toMyCoordinates(PHPoint * pnt, int n);
	PHPoint fromMyCoordinates(PHPoint pnt);
	void fromMyCoordinates(PHPoint * pnt, int n);
	
	
	virtual ~PHView();
	friend class PHMainEvents;
	friend class PHEventHandler;
	
//animation system
private:
	static std::list<PHAnimationDescriptor*> animations;
	PHView * pointerDeepFirst(PHTouch * touch);
	
public:
	static void addAnimation(PHAnimationDescriptor * anim);
	static void updateAnimation(double time);
};

#undef INSIDE_PHVIEW_H
#endif
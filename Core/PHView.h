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

#include "PHMain.h"
#include "PHAnimationDescriptor.h"
#include <list>

class PHView : PHObject
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
	int effOrder;
	PHView();
	PHView(const PHRect &frame);
	void render();
	virtual void draw();
	virtual void setup_matrix();
	
	void apply_rotation();
	void apply_scaling();
	
public:
	
	enum EffectOrder
	{
		EffectOrderScaleRotate = 0,
		EffectOrderRotateScale
	};
	
	void setFrame(const PHRect &frame);
	PHRect frame() const { return _frame; }
	PHRect bounds() const { return _bounds; }
	PHPoint center() const;
	void setCenter(const PHRect &center);
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
	
	void addSubview(PHView * view);
	void removeFromSuperview();
	void bringToFront();
	
	virtual ~PHView();
	friend class PHMainEvents;
	
//animation system
private:
	static std::list<PHAnimationDescriptor*> animations;
public:
	static void addAnimation(PHAnimationDescriptor * anim);
	static void updateAnimation(double time);
};

#endif
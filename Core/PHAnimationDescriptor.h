/*
 *  PHAnimationDescriptor.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/11/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHANIMATIONDESCRIPTOR_H
#define PHANIMATIONDESCRIPTOR_H

#include "PHMain.h"

class PHAnimationDescriptor : public PHObject
{
private:
	PHAnimationDescriptor * next;
public:
	double scaleX,scaleY,_scaleX,_scaleY;
	double moveX,moveY;
	double rotate;
	double time;
	PHObject * view;
	PHColor bgColor;
	int tag;
	
	typedef void (PHObject::*Callback)(void *);
	Callback callback;
	PHObject * target;
	void * userdata;
	
	
	PHAnimationDescriptor() : 
		next(NULL), scaleX(1), scaleY(1),_scaleX(1),
		_scaleY(1), moveX(0), moveY(0), rotate(0), tag(0), bgColor(PHInvalidColor),
		view(NULL), callback(NULL), target(NULL), userdata(NULL) {};
	void setNextAnimation(PHAnimationDescriptor * nexta)
	{
		if (nexta) nexta->retain();
		if (next) next->release();
		next = nexta;
	}
	PHAnimationDescriptor * nextAnimation()
	{
		return next;
	}
	~PHAnimationDescriptor()
	{
		if (next) next->release();
	}
};

#endif
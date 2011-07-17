/*
 *  PHAnimationDescriptor.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/11/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
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
	double scaleX,scaleY;
	double moveX,moveY;
	double rotate;
	double time;
	double totalTime;
	PHObject * view;
	PHColor bgColor;
	int tag;
	int timeFunction;
	
	PHCallback callback;
	PHObject * target;
	void * userdata;
	
	enum Functions
	{
		LinearFunction = 0,
		BounceFunction,
		FadeInFunction,
		FadeOutFunction,
		FadeInOutFunction,
		NUMFUNCTIONS
	};
	
	PHAnimationDescriptor() : 
		next(NULL), scaleX(1), scaleY(1),
		moveX(0), moveY(0), rotate(0), tag(0), bgColor(PHInvalidColor),
		view(NULL), callback(NULL), target(NULL), userdata(NULL), time(0), totalTime(0), timeFunction(LinearFunction) {};
	
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
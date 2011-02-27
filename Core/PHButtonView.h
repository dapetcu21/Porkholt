/*
 *  PHButtonView.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/25/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHBUTTONVIEW_H
#define PHBUTTONVIEW_H
#include "PHMain.h"

class PHButtonView : public PHView
{
private:
	PHImage * imgUp, * imgDown;
	int _state;
	PHCallback cbUp,cbDown;
	void * udUp, *udDown;
	PHObject * tgUp, * tgDown;
public:
	enum states
	{
		StateUp = 0,
		StateDown
	};
	
	PHButtonView();
	PHButtonView(const PHRect &frame);
	void setPressedImage(PHImage * img) { if (img) img->retain(); if (imgDown) imgDown->release(); imgDown=img; }
	void setImage(PHImage * img) { if (img) img->retain(); if (imgUp) imgUp->release(); imgUp=img; }
	PHImage * pressedImage() { return imgDown; };
	PHImage * image() { return imgUp; };
	int state() { return _state; }
	void setUpCallBack(PHObject * tg, PHCallback cb, void * ud ) { tgUp = tg; cbUp = cb; udUp = ud; };
	void setDownCallBack(PHObject * tg, PHCallback cb, void * ud ) { tgDown = tg; cbDown = cb; udDown = ud; };
protected:
	virtual void draw();
	virtual void touchEvent(PHTouch * touch);
};

#endif
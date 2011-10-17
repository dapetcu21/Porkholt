/*
 *  PHButtonView.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/25/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHBUTTONVIEW_H
#define PHBUTTONVIEW_H
#include "PHView.h"

class PHImage;
class PHButtonView : public PHView
{
private:
	PHImage * imgUp, * imgDown;
	int _state;
    PHInvocation invUp,invDown;
    
public:
	enum states
	{
		StateUp = 0,
		StateDown
	};
	
	PHButtonView();
	PHButtonView(const PHRect &frame);
	void setPressedImage(PHImage * img);
	void setImage(PHImage * img);
	PHImage * pressedImage() { return imgDown; };
	PHImage * image() { return imgUp; };
	int state() { return _state; }
	void setUpCallback(PHInvocation inv) { invUp = inv; };
	void setDownCallback(PHInvocation inv) { invDown = inv; };
protected:
	virtual void draw();
	virtual void touchEvent(PHEvent * touch);
};

#endif
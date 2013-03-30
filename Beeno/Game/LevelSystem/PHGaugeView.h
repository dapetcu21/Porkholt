/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */


#ifndef PHGAUGEVIEW_H
#define PHGAUGEVIEW_H

#include <Porkholt/Core/PHImageView.h>

class PHGaugeView : public PHImageView
{
private:
	float lvl;
public:
	PHGaugeView() : PHImageView(), lvl(0) {};
	PHGaugeView(const PHRect &frame) : PHImageView(frame), lvl(0) {};
	PHGaugeView(PHImage * image) : PHImageView(image), lvl(0) {};
	float level() { return lvl; }
	void setLevel(float l) { lvl = l; }
	virtual void draw();
};

#endif

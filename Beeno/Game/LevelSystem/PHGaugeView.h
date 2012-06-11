/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */


#ifndef PHGAUGEVIEW_H
#define PHGAUGEVIEW_H

#include <Porkholt/Core/PHImageView.h>

class PHGaugeView : public PHImageView
{
private:
	ph_float lvl;
public:
	PHGaugeView() : PHImageView(), lvl(0) {};
	PHGaugeView(const PHRect &frame) : PHImageView(frame), lvl(0) {};
	PHGaugeView(PHImage * image) : PHImageView(image), lvl(0) {};
	ph_float level() { return lvl; }
	void setLevel(ph_float l) { lvl = l; }
	virtual void draw();
};

#endif

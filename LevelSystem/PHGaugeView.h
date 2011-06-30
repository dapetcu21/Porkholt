/*
 *  PHGaugeView.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/22/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */


#ifndef PHGAUGEVIEW_H
#define PHGAUGEVIEW_H

#include "PHImageView.h"

class PHGaugeView : public PHImageView
{
private:
	double lvl;
public:
	PHGaugeView() : PHImageView(), lvl(0) {};
	PHGaugeView(const PHRect &frame) : PHImageView(frame), lvl(0) {};
	PHGaugeView(PHImage * image) : PHImageView(image), lvl(0) {};
	double level() { return lvl; }
	void setLevel(double l) { lvl = l; }
	virtual void draw();
};

#endif
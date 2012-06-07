/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHPROJECTIONCHANGER_H
#define PHPROJECTIONCHANGER_H

#include "PHDrawable.h"

class PHProjectionChanger : public PHDrawable
{
private:
    bool enabled;
	PHMatrix m;
public:
    PHProjectionChanger() : enabled(false) {}
    PHProjectionChanger(const PHMatrix & p) : enabled(true), m(p) {}
	
	void setEnabled(bool e) { enabled = e; }
	bool isEnabled() { return enabled; }
	void enable() { enabled = true; }
	void disable() { enabled = false; }
	
	void setProjection(const PHMatrix & p) { m = p; }
	const PHMatrix & projection() { return m; }
	
    void render();
};

#endif

/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSCROLLERVIEW_H
#define PHSCROLLERVIEW_H

#include <Porkholt/Core/PHView.h>

class PHScrollerView : public PHView
{
private:
    double scl;
    PHView * content;
    bool inertial;
    PHPoint inscroll,lastin;
public:
    PHScrollerView();
	PHScrollerView(const PHRect &frame);
    
    double scale() { return scl; }
    void setScale(double s, const PHPoint & center);
    void resetToOrigin();
    void move(const PHPoint & p);

    void setContentView(PHView * cv);
    PHView * contentView() { return content; }
    
    void setFrame(const PHRect &frame);
    
    bool inertialScrolling() { return inertial; }
    void setInertialScrolling(bool is) { inertial = is; }
    
protected:
	virtual void draw();
	virtual void touchEvent(PHEvent * event);
    
    void drawGrid(double gap, double width, const PHColor & color);
    
};

#endif

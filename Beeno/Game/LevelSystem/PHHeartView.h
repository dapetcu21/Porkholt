/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHHEARTVIEW_H
#define PHHEARTVIEW_H

#include <Porkholt/Core/PHImageView.h>

#define PHHEARTVIEW_INIT heartNo(3), active(3), flipped(false), gap(0)

class PHHeartView : public PHImageView
{
private:
    int heartNo;
    int active;
    bool flipped;
    ph_float gap;
public:
    PHHeartView() : PHImageView(), PHHEARTVIEW_INIT {}
    PHHeartView(const PHRect & frame) : PHImageView(frame), PHHEARTVIEW_INIT {}
    int heartNumber() { return heartNo; }
    void setHeartNumber(int hn) { heartNo = hn; }
    int activeHearts() { return active; }
    void setActiveHearts(int ah) { active = ah; }
    bool flippedOrder() { return flipped; }
    void setFlippedOrder(bool f) { flipped = f; }
    virtual void draw();
    void setHeartGap(ph_float g) { gap = g; }
    ph_float heartGap() { return gap; }
};

#endif

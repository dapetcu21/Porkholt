//
//  PHHeartView.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/26/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHHEARTVIEW_H
#define PHHEARTVIEW_H

#include "PHImageView.h"

#define PHHEARTVIEW_INIT heartNo(3), active(3), flipped(false)

class PHHeartView : public PHImageView
{
private:
    int heartNo;
    int active;
    bool flipped;
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
};

#endif
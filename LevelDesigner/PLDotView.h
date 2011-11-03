//
//  PLDotView.h
//  LevelDesigner
//
//  Created by Marius Petcu on 11/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PLDOTVIEW_H
#define PLDOTVIEW_H

#import "PHView.h"

class PLDotView : public PHView
{
protected:
    PHColor color;
    double radius;
    int circleChunks;
public:
    const PHColor & dotColor() { return color; }
    void setDotColor(const PHColor & c) { color = c; }
    double dotRadius() { return radius; }
    void setDotRadius(double d) { radius = d; }
    int numberOfChunks() { return circleChunks; }
    void setNumberOfChunks(int c) { circleChunks = c; }
    
    PLDotView() : color(PHWhiteColor), PHView(), radius(0.03f), circleChunks(50) { setUserInput(true); setFrame(PHRect(-radius,-radius,2*radius,2*radius)); }
    void setPosition(const PHPoint & p)
    {
        setFrame(_frame-center()+p);
    }
    virtual void draw();
};


#endif

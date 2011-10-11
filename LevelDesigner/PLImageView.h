//
//  PLImageView.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/9/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PLIMAGEVIEW_H
#define PLIMAGEVIEW_H

#include "PHImageView.h"
@class PLImage;
class PLObjectView;

class PLImageView : public PHImageView
{
protected:
    PLObjectView * objectView;
public:
    PLObjectView * controller() { return objectView; }
    void setController(PLObjectView * ov) { objectView = ov; }
    
protected:
    PLImage * model;
    bool moving,rotating;
    
public:
    PLImageView(PLImage * _model);
    ~PLImageView();
    void modelChanged();
    
    bool intersectsRect(PHView * base, const PHRect & rect);
    bool intersectsPoint(const PHPoint & pnt);
    
    void touchEvent(PHEvent * event);
    
    bool objectMode();
    
protected:
    void draw();
};

#endif

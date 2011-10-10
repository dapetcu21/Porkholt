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
    
public:
    PLImageView(PLImage * _model);
    ~PLImageView();
    void modelChanged();
    
protected:
    void draw();
};

#endif

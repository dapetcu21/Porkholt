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
class PLBezierView;

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
    int grab;
    NSRect initialGrabFrame;
    PLBezierView * bezierView;
    
    int grabTypeForPoint(const PHPoint & pnt);
    NSUndoManager * undoManager();
    void resizeWithTypeAndDelta(int type, PHPoint delta);
    void resizeToBounds(const PHRect & newBounds);
    
    void setGameManager(PHGameManager * gm);
    
public:
    PLImageView(PLImage * _model);
    ~PLImageView();
    void modelChanged();
    
    bool intersectsRect(PHView * base, const PHRect & rect);
    bool intersectsPoint(const PHPoint & pnt);
    void selectedChanged();
    
    void touchEvent(PHEvent * event);
    
    bool objectMode();
    
    void resetAspectRatio();
    
protected:
    void draw();
};

#endif

//
//  PLFixtureView.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/10/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PLFIXTUREVIEW
#define PLFIXTUREVIEW

#include "PHView.h"
@class PLFixture;
class PLObjectView;
class PLBezierView;
@class PLBezier;

class PLFixtureView : public PHView
{
protected:
    PLObjectView * objectView;
public:
    PLObjectView * controller() { return objectView; }
    void setController(PLObjectView * ov) { objectView = ov; }
    void touchEvent(PHEvent * event);
    
protected:
    PLFixture * model;
    PLBezier * curve;
    PLBezierView * bezierView;
    
    GLuint arraysVBO,indexesVBO;
    size_t nVertices;
    size_t nIndexes;
    int nLines;
    
    void draw();
    int grabTypeForPoint(const PHPoint &pnt);
    void resizeToBounds(const PHRect & newBounds);
    void resizeWithTypeAndDelta(int type, PHPoint delta);
    int grab;
    bool moving,rotating;
    NSRect initialGrabFrame;
    double initialGrabRadius;
    
    void bezierChanged(PHObject * sender, void * ud);
    void changeBezier(PLBezier * cv, bool showView);
    
public:
    PLFixtureView(PLFixture * _model);
    ~PLFixtureView();
    void modelChanged();
    
    bool intersectsRect(PHView * base, const PHRect & rect);
    bool intersectsPoint(const PHPoint & pnt);
    
    bool objectMode();
    void selectedChanged();
    NSUndoManager * undoManager();
};

#endif

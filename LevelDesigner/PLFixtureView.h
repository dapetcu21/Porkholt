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

class PLFixtureView : public PHView
{
protected:
    PLObjectView * objectView;
public:
    PLObjectView * controller() { return objectView; }
    void setController(PLObjectView * ov) { objectView = ov; }
    
protected:
    PLFixture * model;
    
public:
    PLFixtureView(PLFixture * _model);
    ~PLFixtureView();
    void modelChanged();
    
    bool intersectsRect(PHView * base, const PHRect & rect);
    bool intersectsPoint(const PHPoint & pnt);
    
    bool objectMode();
};

#endif

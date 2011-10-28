//
//  PLBezierView.mm
//  LevelDesigner
//
//  Created by Marius Petcu on 10/26/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLBezierView.h"
#import "PLBezier.h"
#import "PHBezierPath.h"

class PLDotView : public PHView
{
public:
    PHColor color;
    static const double radius;
    static const int circleChunks;
    PLBezierView * delegate;
    PLDotView(PLBezierView * del) : color(PHWhiteColor), delegate(del), PHView(PHRect(-radius,-radius,2*radius,2*radius)) { setUserInput(true); }
    void setPosition(const PHPoint & p)
    {
        setFrame(_frame-center()+p);
    }
    void touchEvent(PHEvent * evt)
    {
        if (evt->type() == PHEvent::touchDown)
        {
            if ((toMyCoordinates(evt->location())-PHPoint(radius,radius)).length()<=radius)
            {
                evt->setHandled(true);
                bringToFront();
            }
        }
    }
    void draw()
    {
        GLfloat circleVertices [(circleChunks+2)*2];
        double rad = radius;
        PHPoint center = _bounds.center();
        circleVertices[0] = center.x;
        circleVertices[1] = center.y;
        for (int i=0; i<=circleChunks; i++)
        {
            double angle = M_PI*2*((double)i/circleChunks);
            circleVertices[i*2+2] = sin(angle)*rad+center.x;
            circleVertices[i*2+3] = cos(angle)*rad+center.y;
        }
        
        PHGLSetStates(PHGLVertexArray);
        PHGLSetColor(color);
        glVertexPointer(2, GL_FLOAT, 0, circleVertices);
        glDrawArrays(GL_TRIANGLE_FAN, 0, circleChunks+2);
    }
};
const double PLDotView::radius = 0.03f;
const int PLDotView::circleChunks = 50;

void PLBezierView::setModel(PLBezier * bezier)
{
    if (bezier==_model) return;
    [_model removeActor:this];
    [bezier addActor:this];
    
    if (_model)
        _model.bezierPath->removeCallback(this);
    if (bezier)
        bezier.bezierPath->addCallback(PHInv(this,PLBezierView::modelChanged,nil));
    [bezier retain];
    [_model release];
    _model = bezier;
    modelChanged();
}

PLBezierView::~PLBezierView() { 
    if (_model)
        _model.bezierPath->removeCallback(this);
    [_model removeActor:this];
    [_model release]; 
    _model = nil;
    modelChanged();
}

void PLBezierView::modelChanged()
{
    size_t n = dots.size();
    const vector<PHBezierPath::anchorPoint> * ap = NULL;
    if (_model)
        ap = & _model.bezierPath->anchorPoints();
    size_t m = ap?ap->size():0;
    if (m<n)
    {
        for (size_t i=m; i<n; i++)
        {
            dots[i]->delegate = nil;
            dots[i]->removeFromSuperview();
            dots[i]->release();
        }
        dots.resize(m);
    } else
    if (m>n)
    {
        for (size_t i=n; i<m; i++)
        {
            PLDotView * dv = new PLDotView(this);
            addSubview(dv);
            dots.push_back(dv);
        }
    }
    for (int i=0; i<m; i++)
    {
        const PHBezierPath::anchorPoint & p = ap->at(i);
        PLDotView * dv = dots[i];
        dv->setPosition(p.point*_bounds.size());
        dv->setTag(i);
    }
}

void PLBezierView::setBounds(const PHRect & bounds)
{
    PHView::setBounds(bounds);
    modelChanged();
}
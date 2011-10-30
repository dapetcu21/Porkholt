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
#import "PHEventHandler.h"

class PLDotView : public PHView
{
public:
    PHColor color;
    static const double radius;
    static const int circleChunks;
    PLBezierView * delegate;
    bool moved;
    PLDotView(PLBezierView * del) : color(PHWhiteColor), delegate(del), moved(false), PHView(PHRect(-radius,-radius,2*radius,2*radius)) { setUserInput(true); }
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
        if (evt->type() == PHEvent::touchMoved)
            if (evt->userData() == (void*)1)
            {
                if (!moved)
                {
                    delegate->moveDotStarted(this);
                    moved = true;
                }
                delegate->moveDot(this,superView->toMyCoordinates(evt->location())-superView->toMyCoordinates(evt->lastLocation()));
            }
        if (evt->type() == PHEvent::touchUp)
        {
            if (evt->userData() == (void*)1)
            {
                if (moved)
                    delegate->moveDotEnded(this);
                if (PHEventHandler::modifierMask() & PHEventHandler::shiftModifier)
                    delegate->changeTag(this);
            } else
            if (evt->userData() == (void*)2)
            {
                if (PHEventHandler::modifierMask() & PHEventHandler::shiftModifier)
                    delegate->removeDot(this);
                else
                    delegate->newDot(this);
            } else
            if (evt->userData() == (void*)3)
                delegate->markAsCurve(this);
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

void PLBezierView::newDot(PLDotView * sender)
{
    int cidx = sender->tag();
    if (cidx<=markedAnchor)
        markedAnchor++;
    const vector<PHBezierPath::anchorPoint> * ap = NULL;
    if (_model)
        ap = & _model.bezierPath->anchorPoints();
    if (ap)
    {
        PHBezierPath::anchorPoint nx;
        if (cidx+1<ap->size())
            nx = ap->at(cidx+1);
        else
            nx = ap->at(0);
        _model.bezierPath->insertAnchorPoint(PHBezierPath::anchorPoint((ap->at(cidx).point+nx.point)/2,0), cidx+1);
    }
}

void PLBezierView::removeDot(PLDotView * sender)
{
    int cidx = sender->tag();
    if (cidx == markedAnchor) 
        markedAnchor = -1;
    if (cidx<markedAnchor)
        markedAnchor--;
    if (_model)
    {
        [_model saveUndoState];
        _model.bezierPath->removeAnchorPoint(cidx);
    }
}

void PLBezierView::changeTag(PLDotView * sender)
{
    int cidx = sender->tag();
    const vector<PHBezierPath::anchorPoint> * ap = NULL;
    if (_model)
        ap = & _model.bezierPath->anchorPoints();
    if (ap)
    {
        PHBezierPath::anchorPoint a = ap->at(cidx);
        if (a.tag<0 || a.tag>4)
            a.tag = 0;
        else
            a.tag=(a.tag==4)?0:a.tag+1;
        [_model saveUndoState];
        _model.bezierPath->replaceAnchorPoint(a, cidx);
    }
}

void PLBezierView::markAsCurve(PLDotView * sender)
{
    int cidx = sender->tag();
    const vector<PHBezierPath::anchorPoint> * ap = NULL;
    if (_model)
        ap = & _model.bezierPath->anchorPoints();
    if (ap)
    {
        int n = (int)ap->size();
        PHBezierPath::anchorPoint a = ap->at(cidx);
        set<PHRange> r;
        const set<PHRange> & rn = _model.bezierPath->bezierCurves();
        for (set<PHRange>::const_iterator i = rn.begin(); i!=rn.end(); i++)
            if ((*i).intersectsCircularily(cidx,n))
                r.insert(*i);
        if (!r.empty())
        {
            [_model saveUndoState];
            _model.bezierPath->beginCommitGrouping();
            for (set<PHRange>::const_iterator i = r.begin(); i!=r.end(); i++)
                _model.bezierPath->removeCurve(*i);
            _model.bezierPath->endCommitGrouping();
            markedAnchor = -1;
        } else
        {
            if (markedAnchor==-1)
                markedAnchor = cidx;
            else
            {
                PHRange r;
                r.start = markedAnchor;
                if (cidx>markedAnchor)
                    r.length = cidx - markedAnchor;
                else
                    r.length = cidx - markedAnchor + n;
                markedAnchor = -1;
                if (r.length==n)
                {
                    NSBeep();
                    return;
                }
                for (set<PHRange>::const_iterator i = rn.begin(); i!=rn.end(); i++)
                    if ((*i).intersectsCircularily(r, n))
                    {
                        NSBeep();
                        return;
                    }
                [_model saveUndoState];
                _model.bezierPath->addCurve(PHRange(r.start,r.length+1));
            }
        }
    }
}

void PLBezierView::moveDotStarted(PLDotView * sender)
{
    [_model saveUndoState];
}

void PLBezierView::moveDot(PLDotView * sender, PHPoint delta)
{
    const vector<PHBezierPath::anchorPoint> * ap = NULL;
    if (_model)
        ap = & _model.bezierPath->anchorPoints();
    if (ap)
    {
        PHBezierPath::anchorPoint a = ap->at(sender->tag());
        a.point += delta/_bounds.size();
        sender->setPosition(sender->center()+delta);
        fromWithin = true;
        _model.bezierPath->replaceAnchorPoint(a, sender->tag());
        fromWithin = false;
    }
}

void PLBezierView::moveDotEnded(PLDotView * sender)
{
    //nothing here... move along
}

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
    if (fromWithin) return;
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
    const set<PHRange> * cv;
    if (_model)
        cv = & _model.bezierPath->bezierCurves();
    for (int i=0; i<m; i++)
    {
        const PHBezierPath::anchorPoint & p = ap->at(i);
        PLDotView * dv = dots[i];
        dv->setPosition(p.point*_bounds.size());
        dv->setTag(i);
        bool inside = false;
        bool edge = false;
        if (cv)
            for (set<PHRange>::const_iterator j = cv->begin(); j!=cv->end(); j++)
            {
                const PHRange & r = (*j);
                if (r.intersectsCircularily(i, (int)m))
                    inside = true;
                if (r.start==i || PHRange::wrap(r.end()-1,(int)m)==i)
                    edge = true;
            }
        PHColor c;
        switch (p.tag)
        {
            case 1:
                c=PHColor(1,0,0);
                break;
            case 2:
                c=PHColor(0,1,0);
                break;
            case 3:
                c=PHColor(0,0,1);
                break;
            case 4:
                c=PHColor(1,1,0);
                break;
            default:
                c=PHColor(1,1,1);
                break;
        }
        if (inside)
            c*=0.5;
        if (edge)
            c=PHColor(c.r*0.5,c.g*0.5,c.b*0.5,c.a);
        dv->color = c;
    }
}

void PLBezierView::setBounds(const PHRect & bounds)
{
    PHView::setBounds(bounds);
    modelChanged();
}
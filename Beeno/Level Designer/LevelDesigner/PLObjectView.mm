//
//  PLObjectView.mm
//  LevelDesigner
//
//  Created by Marius Petcu on 10/5/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLObjectView.h"
#import "PLProperty.h"
#import "PLObject.h"
#import "PHEventHandler.h"
#import "ObjectController.h"
#import "WorldController.h"
#import "PLImage.h"
#import "PLImageView.h"
#import "SubentityController.h"
#import "PLFixture.h"
#import "PLFixtureView.h"
#import "PLObjectMarker.h"
#import "PLJointDot.h"

PLObjectView::PLObjectView(PLObject * _model) : PHView(PHRect(-0.7,-0.7,0.14,0.14)), model(_model), sel(false), moving(false), rotating(false)
{
    setBounds(bounds()-boundsCenter());
    setRotationalCenter(PHOriginPoint);
    setScalingCenter(PHOriginPoint);
    marker = new PLObjectMarker(bounds());
    [model retain];
    [model setActor:this];
    modelChanged();
    setUserInput(true);
}

PLObjectView::~PLObjectView()
{
    removeAllJoints();
    [model setActor:NULL];
    [model release];
    marker->release();
}

static inline bool lineIntersectsRect(const PHPoint & p1, const PHPoint & p2, const PHRect & r)
{
    double x;
    double y;
    double maxX = p1.x;
    double minX = p2.x;
    double maxY = p1.y;
    double minY = p2.y;
    if (maxY<minX)
    {
        maxX = p2.x;
        minX = p1.x;
    }
    if (maxY<minY)
    {
        maxY = p2.y;
        minY = p1.y;
    }
    y = r.y;
    x = p1.x-(p1.x-p2.x)*(p1.y-y)/(p1.y-p2.y);
    if (x>=r.x && x<=r.x+r.width && x<=maxX && x>=maxX && y<=maxY && y>=minY) return true;
    y = r.y+r.height;
    x = p1.x-(p1.x-p2.x)*(p1.y-y)/(p1.y-p2.y);
    if (x>=r.x && x<=r.x+r.width && x<=maxX && x>=maxX && y<=maxY && y>=minY) return true;
    x = r.x;
    y = p1.y-(p1.y-p2.y)*(p1.x-x)/(p1.x-p2.x);
    if (y>=r.y && y<=r.y+r.height && x<=maxX && x>=maxX && y<=maxY && y>=minY) return true;
    x = r.x+r.width;
    y = p1.y-(p1.y-p2.y)*(p1.x-x)/(p1.x-p2.x);
    if (y>=r.y && y<=r.y+r.height && x<=maxX && x>=maxX && y<=maxY && y>=minY) return true;
    return false;
}

bool PLObjectView::rectsIntersect(PHView * base, const PHRect & r1, PHView * v1, const PHRect & r2, PHView * v2)
{
    PHPoint p1[4],p2[4];
    double minX = DBL_MAX, minY = DBL_MAX, maxX = -DBL_MAX, maxY = -DBL_MAX;
    for (int i=0; i<4; i++)
    {
        p1[i].x = r1.x+((i&1)?r1.width:0);
        p1[i].y = r1.y+((i&2)?r1.height:0);
        p1[i] = v2->toMyCoordinates(v1->fromMyCoordinates(p1[i], base),base);
        if (p1[i].x<minX) minX = p1[i].x;
            if (p1[i].x>maxX) maxX = p1[i].x;
                if (p1[i].y<minY) minY = p1[i].y;
                    if (p1[i].y>maxY) maxY = p1[i].y;
                        if (PHPointInRect(p1[i],r2)) return true;
    }
    if ((minX>r2.x+r2.width)||
        (maxX<r2.x)||
        (minY>r2.y+r2.height)||
        (maxY<r2.y))
        return false;
    for (int i=0; i<4; i++)
    {
        p2[i].x = r2.x+((i&1)?r2.width:0);
        p2[i].y = r2.y+((i&2)?r2.height:0);
        p2[i] = v1->toMyCoordinates(v2->fromMyCoordinates(p1[i], base),base);
        if (PHPointInRect(p2[i],r1)) return YES;
    }
    if (lineIntersectsRect(p1[0],p1[1],r2)) return true;
    if (lineIntersectsRect(p1[1],p1[3],r2)) return true;
    if (lineIntersectsRect(p1[0],p1[2],r2)) return true;
    if (lineIntersectsRect(p1[2],p1[3],r2)) return true;
    return false;
}

void PLObjectView::addJoint(PLJointDot * jointView)
{
    if (!jointView) return;
    joints.push_back(jointView);
    jointView->retain();
    jointView->removeFromSuperview();
    if (((ObjectController*)[model owner]).showJoints)
        addSubview(jointView);
}

void PLObjectView::removeAllJoints()
{
    for (vector<PLJointDot*>::iterator i = joints.begin(); i!=joints.end(); i++)
    {
        (*i)->removeFromSuperview();
        (*i)->release();
    }
    joints.clear();
}

void PLObjectView::reloadSubviews()
{
    for (PLImage * obj in [[model subentityModel] images])
        if (obj.actor)
            (obj.actor)->retain();
    for (PLFixture * obj in [[model subentityModel] fixtures])
        if (obj.actor)
            (obj.actor)->retain();
    
    removeAllSubviews();
    if (((ObjectController*)[model owner]).showImages)
        for (PLImage * obj in [[model subentityModel] images])
        {
            if (!obj.actor)
            {
                obj.actor = new PLImageView(obj);
                obj.actor->setController(this);
            }
            addSubview(obj.actor);
            obj.actor->modelChanged();
        }
    if (((ObjectController*)[model owner]).showFixtures)
        for (PLFixture * obj in [[model subentityModel] fixtures])
        {
            if (!obj.actor)
            {
                obj.actor = new PLFixtureView(obj);
                obj.actor->setController(this);
            } else
                obj.actor->modelChanged();
            addSubview(obj.actor);
        }

    if (((ObjectController*)[model owner]).showMarkers)
        addSubview(marker);
    if (((ObjectController*)[model owner]).showJoints)
        for (vector<PLJointDot*>::iterator i = joints.begin(); i!=joints.end(); i++)
            addSubview(*i);
    
    for (PLImage * obj in [[model subentityModel] images])
        if (obj.actor)
            (obj.actor)->release();
    for (PLFixture * obj in [[model subentityModel] fixtures])
        if (obj.actor)
            (obj.actor)->release();
}

void PLObjectView::modelChanged()
{
    NSPoint pos = [model propertyAtKeyPath:@"pos"].pointValue;
    setPosition(PHPoint(pos.x,pos.y)+bounds().origin());
    setRotation(-toRad([model propertyAtKeyPath:@"rotation"].numberValue));
    setHorizontallyFlipped([model propertyAtKeyPath:@"flipped"].numberValue);
    
    reloadSubviews();
}

void PLObjectView::flagsChanged()
{
    modelChanged();
}

bool PLObjectView::intersectsRect(const PHRect & rect)
{
    if (((ObjectController*)[model owner]).showMarkers?PHPointInRect(center(), rect):false)
        return true;
    const list<PHView*> & sv = subViews();
    for (list<PHView*>::const_iterator i = sv.begin(); i!=sv.end(); i++)
    {
        PLImageView * iv = dynamic_cast<PLImageView*>(*i);
        if (iv && iv->intersectsRect(superView,rect))
            return true;
        PLFixtureView * fv = dynamic_cast<PLFixtureView*>(*i);
        if (fv && fv->intersectsRect(superView,rect))
            return true;
    }
    return false;
}

void PLObjectView::touchEvent(PHEvent * evt)
{
    if (((evt->sender() == this) || (evt->sender() == NULL))  && (!((ObjectController*)[model owner]).showMarkers))
        return;
    if (evt->type() == PHEvent::touchDown)
    {
        if ((evt->userData() != (void*)1) || (PHEventHandler::modifierMask() & PHEventHandler::shiftModifier)||((ObjectController*)[model owner]).objectMode)
            return;
        bool cmd = PHEventHandler::modifierMask() & PHEventHandler::commandModifier;
        bool alt = PHEventHandler::modifierMask() & PHEventHandler::optionModifier;
        if (!cmd && ! alt && !sel)
            [[worldController model] clearSelection];
        if (alt)
            [[worldController model] removeEntity:model inSelectionForArray:0];
        else
            [[worldController model] insertEntity:model inSelectionForArray:0];
        evt->setHandled(true);
    } 
    else
    if (evt->type() == PHEvent::touchMoved)
    {
        if (((ObjectController*)[model owner]).objectMode)
            return;
        if (evt->userData() == (void*)1)
        {
            if (!moving)
            {
                [worldController startMoving];
                moving = true;
            }
            //PHLog("%f %f",(evt->location()-evt->lastLocation()).x,(evt->location()-evt->lastLocation()).y);
            PHPoint delta = superView->toMyCoordinates(evt->location()) - superView->toMyCoordinates(evt->lastLocation());
            [worldController move:delta];
            evt->setHandled(true);
        }
        else
        if (evt->userData() == (void*)2)
        {
            if (!rotating)
            {
                [worldController startRotating];
                rotating = true;
            }
            [worldController rotate:-(evt->location().y - evt->lastLocation().y)/2];
            evt->setHandled(true);
        }
    }
    else
    if (evt->type() == PHEvent::touchUp)
    {
        if ((evt->userData() == (void*)1) && moving)
        {
            [worldController stopMoving];
            moving = false;
            evt->setHandled(true);
        }
        if ((evt->userData() == (void*)2) && rotating)
        {
            [worldController stopRotating];
            rotating = false;
            evt->setHandled(true);
        }
    }
}

void PLObjectView::startMoving()
{
    [worldController startMoving];
}

void PLObjectView::stopMoving()
{
    [worldController stopMoving];
}

void PLObjectView::startRotating()
{
    [worldController startRotating];
}

void PLObjectView::stopRotating()
{
    [worldController stopRotating];
}

void PLObjectView::moveSubviews(PHPoint delta)
{
    [worldController move:delta];
}

void PLObjectView::rotateSubviews(double ammount)
{
    [worldController rotate:ammount];
}

bool PLObjectView::readOnly()
{
    return [model readOnly];
}   
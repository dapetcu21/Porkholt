//
//  PLImageView.mm
//  LevelDesigner
//
//  Created by Marius Petcu on 10/10/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PLImageView.h"
#import "PLImage.h"
#import "PLObjectView.h"
#include "PHImage.h"
#import "SubentityController.h"
#import "ObjectController.h"
#import "PLObject.h"
#import "PHFileManager.h"
#import "PHGeometry.h"
#import "PLObjectView.h"
#import "PHEventHandler.h"
#import "PHImage.h"
#import "PLBezier.h"
#import "PLBezierView.h"
#import "PHGameManager.h"
#import "PHBezierPath.h"

PLImageView::PLImageView(PLImage * _model) : model(_model), moving(false), rotating(false), grab(0), bezierView(NULL)
{
    setUserInput(true);
    [model retain];
    [model setActor:this];
}

void PLImageView::attachedToGameManager()
{
    modelChanged();
}

PLImageView::~PLImageView()
{
    if (bezierView)
    {
        bezierView->removeFromParent();
        bezierView->release();
    }
    [model setActor:NULL];
    [model release];
}

void PLImageView::modelChanged()
{
    if (!gm)
        return;
    NSRect frame = [model frame];
    setFrame(PHRect(frame.origin.x,frame.origin.y,frame.size.width,frame.size.height));
    NSString * s = [model fileName];
    if (!s)
        s = @"";
    string path = string([s UTF8String]);
    if (path[0]=='/')
        path = gm->resourcePath()+"/img/"+path;
    else
        path = string([[[[((ObjectController*)(((SubentityController*)(model.owner)).object.owner)) fileURL] URLByAppendingPathComponent:s] path] UTF8String]);;
    setImage(gm->imageFromPath(path));
    setRotation(-toRad(model.rotation));
    NSRect portion = model.portion;
    setTextureCoordinates(PHRect(portion.origin.x,portion.origin.y,portion.size.width,portion.size.height));
    setHorizontallyFlipped(model.horizontallyFlipped);
    setVerticallyFlipped(model.verticallyFlipped);
    PLColor c = model.tint;
    setTintColor(PHColor(c.r,c.g,c.b,c.a));
    setAlpha(model.alpha);
    setConstrainCurveToFrame(model.constrainToFrame);
    setShape(model.bezierCurve.bezierPath);
    setRepeatX(model.repeatX);
    setRepeatY(model.repeatY);
    selectedChanged();
}

void PLImageView::selectedChanged()
{
    SubentityController * sc = (SubentityController*)[model owner];
    ObjectController * oc = (ObjectController*)[[sc object] owner];
    bool selected = ([model selected] && [oc objectMode] && ([oc selectedEntity] == [sc object]) && !model.readOnly && ![sc object].readOnly);
    PLBezier * b = selected?[model bezierCurve]:nil;
    if (((bezierView!=NULL)!=(b!=NULL)) || (bezierView && bezierView->model()!=b))
    {
        if (bezierView)
        {
            bezierView->removeFromParent();
            bezierView->release();
        }
        if (b)
        {
            [b setUndoManager:undoManager()];
            bezierView = new PLBezierView;
            bezierView->setModel(b);
            addChild(bezierView);
        } else
            bezierView = NULL; 
    }
    if (bezierView)
        bezierView->setFrame(_bounds);
}

bool PLImageView::intersectsRect(PHView * base, const PHRect & rect)
{
    return PLObjectView::rectsIntersect(base, rect, base, _bounds, this);
}

bool PLImageView::intersectsPoint(const PHPoint & pnt)
{
    return PHPointInRect(pnt, _bounds);
}

bool PLImageView::objectMode()
{
    SubentityController * sc = (SubentityController*)[model owner];
    ObjectController * oc = (ObjectController*)[[sc object] owner];
    return [oc objectMode]&& ([oc selectedEntity] == [sc object]);
}

int PLImageView::grabTypeForPoint(const PHPoint &pnt)
{
    if ([model readOnly] || [[model object] readOnly]) return 0;
    PHPoint p = pnt-_bounds.origin();
    if (p.x<=0.1 && p.y<=0.1) return 1;
    if (p.x>=_bounds.width-0.1 && p.y<=0.1) return 2;
    if (p.x>=_bounds.width-0.1 && p.y>=_bounds.height-0.1) return 3;
    if (p.x<=0.1 && p.y>=_bounds.height-0.1) return 4;
    if (p.x<=0.05) return 5;
    if (p.y<=0.05) return 6;
    if (p.x>=_bounds.width-0.05) return 7;
    if (p.y>=_bounds.height-0.05) return 8;
    return 0;
}

NSUndoManager * PLImageView::undoManager()
{
    SubentityController * sc = (SubentityController*)[model owner];
    ObjectController * oc = (ObjectController*)[[sc object] owner];
    return [oc undoManager];
}

void PLImageView::resizeToBounds(const PHRect & newBounds)
{
    PHRect bounds = this->bounds();
    NSRect f = [model frame];
    PHRect frame = PHRect(f.origin.x,f.origin.y,f.size.width,f.size.height);
    PHSize deltaCenter = fromMyCoordinates(newBounds.center(), superview())-fromMyCoordinates(bounds.center(), superview());
    PHSize deltaSize = (newBounds.size()-bounds.size());
    PHRect nf = frame+deltaCenter-deltaSize/2;
    nf.width = newBounds.width;
    nf.height = newBounds.height;
    [model setFrame:NSMakeRect(nf.x, nf.y, nf.width, nf.height)];
}

void PLImageView::resizeWithTypeAndDelta(int type, PHPoint delta)
{
    PHRect bounds = this->bounds();
    PHRect newBounds = bounds;    
    switch (type) {
        case 1:
            if (delta.x>= bounds.width)
                delta.x = bounds.width;
            if (delta.y>= bounds.height)
                delta.y = bounds.height;
            newBounds.width-=delta.x;
            newBounds.height-=delta.y;
            newBounds.x+=delta.x;
            newBounds.y+=delta.y;
            break;
        case 2:
            if (delta.x<= -bounds.width)
                delta.x = -bounds.width;
            if (delta.y>= bounds.height)
                delta.y = bounds.height;
            newBounds.width+=delta.x;
            newBounds.height-=delta.y;
            newBounds.y+=delta.y;
            break;
        case 4:
            if (delta.x>= bounds.width)
                delta.x = bounds.width;
            if (delta.y<= -bounds.height)
                delta.y = -bounds.height;
            newBounds.width-=delta.x;
            newBounds.height+=delta.y;
            newBounds.x+=delta.x;
            break;
        case 3:
            if (delta.x<= -bounds.width)
                delta.x = -bounds.width;
            if (delta.y<= -bounds.height)
                delta.y = -bounds.height;
            newBounds.width+=delta.x;
            newBounds.height+=delta.y;
            break;
        case 5:
            if (delta.x>= bounds.width)
                delta.x = bounds.width;
            newBounds.width-=delta.x;
            newBounds.x+=delta.x;
            break;
        case 6:
            if (delta.y>= bounds.height)
                delta.y = bounds.height;
            newBounds.height-=delta.y;
            newBounds.y+=delta.y;
            break;
        case 7:
            if (delta.x<= -bounds.width)
                delta.x = -bounds.width;
            newBounds.width+=delta.x;
            break;
        case 8:
            if (delta.y<= -bounds.height)
                delta.y = -bounds.height;
            newBounds.height+=delta.y;
            break;
        default:
            break;
    }
    resizeToBounds(newBounds);
}

void PLImageView::resetAspectRatio()
{
    PHImage * img = image();
    if (!img)
    {
        NSBeep();
        return;
    }
    double aspectRatio = img->width()/img->height();
    PHRect bounds = this->bounds();
    PHRect newBounds = bounds;
    if (PHEventHandler::modifierMask() & PHEventHandler::shiftModifier)
        newBounds.width = newBounds.height * aspectRatio;
    else
        newBounds.height = newBounds.width / aspectRatio;
    resizeToBounds(newBounds);
}

void PLImageView::touchEvent(PHEvent * event)
{
    if (event->type() == PHEvent::touchDown)
    {
        if (event->userData() != (void*)1 && event->userData() != (void*)2)
            return;
        PHPoint localPoint = toMyCoordinates(event->location());
        if (intersectsPoint(localPoint))
        {
            if (objectMode())
            {
                if (PHEventHandler::modifierMask() & PHEventHandler::shiftModifier)
                    return;
                bool cmd = PHEventHandler::modifierMask() & PHEventHandler::commandModifier;
                bool alt = PHEventHandler::modifierMask() & PHEventHandler::optionModifier;
                EntityController * ec = (EntityController*)[model owner];
                if (!cmd && ! alt && ![model selected])
                    [ec clearSelection];
                if (alt)
                    [ec removeEntity:model inSelectionForArray:0];
                else
                    [ec insertEntity:model inSelectionForArray:0];
                grab = grabTypeForPoint(localPoint);
                if (grab)
                {
                    initialGrabFrame = [model frame];
                    [undoManager() disableUndoRegistration];
                }
                event->setHandled(true);
            } else
            {
                event->setSender(this);
                event->setOwnerView(superview());
                superview()->touchEvent(event);
            }
            return;
        }
    }
    if (!objectMode())
        return;
    if (event->type() == PHEvent::touchMoved)
    {
        if (event->userData() == (void*)1)
        {
            if (grab)
            {
                PHPoint delta = toMyCoordinates(event->location()) - toMyCoordinates(event->lastLocation());
                resizeWithTypeAndDelta(grab,delta);
                event->setHandled(true);
                return;
            }
            if (!moving)
            {
                objectView->startMoving();
                moving = true;
            }
            PHPoint delta = superView->toMyCoordinates(event->location()) - superView->toMyCoordinates(event->lastLocation());
            objectView->moveSubviews(delta);
            event->setHandled(true);
        }
        else
        if (event->userData() == (void*)2)
        {
            if (!rotating)
            {
                objectView->startRotating();
                rotating = true;
            }
            objectView->rotateSubviews(-(event->location().y - event->lastLocation().y)/2);
            event->setHandled(true);
        }
    }
    if (event->type() == PHEvent::touchUp)
    {
        if (event->userData() == (void*)1)
        {
            if (moving)
            {
                objectView->stopMoving();
                moving = false;
                event->setHandled(true);
            }
            if (grab)
            {
                grab = 0;
                NSRect fr = [model frame];
                [model setFrame:initialGrabFrame];
                [undoManager() enableUndoRegistration];
                [model setFrame:fr];
                event->setHandled(true);
            }
        }
        if ((event->userData() == (void*)2) && rotating)
        {
            objectView->stopRotating();
            rotating = false;
            event->setHandled(true);
        }
    }
}

void PLImageView::draw()
{
    PHImageView::draw();
    SubentityController * sc = (SubentityController*)[model owner];
    ObjectController * oc = (ObjectController*)[[sc object] owner];
    if ([model selected] && [oc showMarkers] && [oc objectMode] && ([oc selectedEntity] == [sc object]))
    {
        GLfloat vertices[] = {
            0,0,
            0.3,0,
            0,1,
            0.3,1,
            0.3,1,
            0,0,
            0,0,
            1,0,
            0,0.3,
            1,0.3
        };
        gm->setGLStates(PHGLBlending | PHGLVertexArray);
        gm->vertexPointer(2, GL_FLOAT, 0, vertices);
        gm->setColor(PHColor(0.5,0.5,1));
        
        PHMatrix om = gm->modelViewMatrix();
        for (int i=0; i<4; i++)
        {
            gm->setModelViewMatrix(om * 
                PHMatrix::translation(_bounds.x+(i&1)?_bounds.width:0, _bounds.y+(i&2)?_bounds.height:0) * 
                PHMatrix::scaling(((i&1)?-1:1)*0.1, ((i&2)?-1:1)*0.1));
            gm->applyShader(gm->solidColorShader());
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
        }
        gm->setModelViewMatrix(om);
    }
}
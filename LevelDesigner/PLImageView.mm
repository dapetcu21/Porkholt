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

PLImageView::PLImageView(PLImage * _model) : model(_model), moving(false), rotating(false)
{
    setUserInput(true);
    [model retain];
    [model setActor:this];
    modelChanged();
}

PLImageView::~PLImageView()
{
    [model setActor:NULL];
    [model release];
}

void PLImageView::modelChanged()
{
    NSRect frame = [model frame];
    setFrame(PHRect(frame.origin.x,frame.origin.y,frame.size.width,frame.size.height));
    string path = string([[model fileName] UTF8String]);
    if (path[0]=='/')
        path = PHFileManager::resourcePath()+"/img/"+path;
    else
        path = string([[[[((ObjectController*)(((SubentityController*)(model.owner)).object.owner)) fileURL] URLByAppendingPathComponent:[model fileName]] path] UTF8String]);
    setImage(PHImage::imageFromPath(path));
    setRotation(-toRad(model.rotation));
    NSRect portion = model.portion;
    setTextureCoordinates(PHRect(portion.origin.x,portion.origin.y,portion.size.width,portion.size.height));
    setHorizontallyFlipped(model.horizontallyFlipped);
    setVerticallyFlipped(model.verticallyFlipped);
    PLColor c = model.tint;
    setTintColor(PHColor(c.r,c.g,c.b,c.a));
    setAlpha(model.alpha);
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

void PLImageView::touchEvent(PHEvent * event)
{
    if (event->type() == PHEvent::touchDown)
    {
        if (intersectsPoint(toMyCoordinates(event->location())))
        {
            if (objectMode())
            {
                if (PHEventHandler::modifierMask() & PHEventHandler::shiftModifier)
                {
                    eventHandled = false;
                    return;
                }
                bool cmd = PHEventHandler::modifierMask() & PHEventHandler::commandModifier;
                bool alt = PHEventHandler::modifierMask() & PHEventHandler::optionModifier;
                EntityController * ec = (EntityController*)[model owner];
                if (!cmd && ! alt && ![model selected])
                    [ec clearSelection];
                if (alt)
                    [ec removeEntity:model inSelectionForArray:0];
                else
                    [ec insertEntity:model inSelectionForArray:0];
            } else
            {
                event->setSender(this);
                event->setOwnerView(superview());
                redirectEvent(superview(), event);
            }
            return;
        }
    }
    if (!objectMode())
    {
        eventHandled = false;
        return;
    }
    if (event->type() == PHEvent::touchMoved)
    {
        if (event->userData() == (void*)1)
        {
            if (!moving)
            {
                objectView->startMoving();
                moving = true;
            }
            PHPoint delta = superView->toMyCoordinates(event->location()) - superView->toMyCoordinates(event->lastLocation());
            objectView->moveSubviews(delta);
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
        }
    }
    if (event->type() == PHEvent::touchUp)
    {
        if ((event->userData() == (void*)1) && moving)
        {
            objectView->stopMoving();
            moving = false;
        }
        if ((event->userData() == (void*)2) && rotating)
        {
            objectView->stopRotating();
            rotating = false;
        }
    }
    eventHandled = false;
}

void PLImageView::draw()
{
    PHImageView::draw();
    if ([model selected] && objectMode())
    {
        GLfloat vertices[] = {
            0,0,
            0.5,0,
            0,1,
            0.5,1,
            0.5,1,
            0,0,
            0,0,
            1,0,
            0,0.5,
            1,0.5
        };
        PHGLSetStates(PHGLVertexArray);
        glVertexPointer(2, GL_FLOAT, 0, vertices);
        PHGLSetColor(PHColor(0.5,0.5,1));
        

        for (int i=0; i<4; i++)
        {
            glPushMatrix();
            glTranslatef(_bounds.x+(i&1)?_bounds.width:0, _bounds.y+(i&2)?_bounds.height:0, 0);
            glScalef(((i&1)?-1:1)*0.1, ((i&2)?-1:1)*0.1, 1);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
            glPopMatrix();
        }
    }
}
//
//  PLJointDot.cpp
//  LevelDesigner
//
//  Created by Marius Petcu on 11/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLJointDot.h"
#import "PLJoint.h"
#import "ObjectController.h"
#include "PHEventHandler.h"

void PLJointDot::setJoint(PLJoint * j) 
{
    if (j==model) return;
    if (first)
    {
        [model setActor1:NULL];
        [j setActor1:this];
    } else {
        [model setActor2:NULL];
        [j setActor2:this];
    }
    [j retain];
    [model release];
    model = j; 
    jointChanged();
}

PLJointDot::~PLJointDot()
{
    if (first)
        [model setActor1:NULL];
    else
        [model setActor2:NULL];
}

PLJointDot::PLJointDot() : first(false), model(NULL)
{
    setDotColor(PHColor(1,0,0));
}

void PLJointDot::jointChanged()
{
    if (model)
    {
        NSPoint p = first?model.anchor1:model.anchor2;
        setPosition(PHPoint(p.x,p.y));
    }
}

void PLJointDot::moveModel(PHPoint delta)
{
    if (!model || model.readOnly || (!delta.x && !delta.y))
        return;
    if (dragged)
        [[model undoManager] disableUndoRegistration];
    if (first)
    {
        NSPoint p = model.anchor1;
        model.anchor1 = NSMakePoint(p.x+delta.x, p.y+delta.y);
    }
    else
    {
        NSPoint p = model.anchor2;
        model.anchor2 = NSMakePoint(p.x+delta.x, p.y+delta.y);
    }
    if (dragged)
        [[model undoManager] enableUndoRegistration];
    dragged = true;
}

void PLJointDot::touchEvent(PHEvent * evt)
{
    if (evt->type() == PHEvent::touchDown)
    {
        if (model && (toMyCoordinates(evt->location())-PHPoint(radius,radius)).length()<=radius)
        {
            evt->setHandled(true);
            dragged = false;
            bringToFront();
            if ((PHEventHandler::modifierMask() & PHEventHandler::commandModifier)==0)
                [((ObjectController*)model.owner) clearSelection];
            [((ObjectController*)model.owner) insertEntity:model inSelectionForArray:1];
        }
    }
    if (evt->type() == PHEvent::touchMoved)
        if (evt->userData() == (void*)1)
        {
            moveModel(superView->toMyCoordinates(evt->location())-superView->toMyCoordinates(evt->lastLocation()));
        }
}

void PLJointDot::draw()
{
    setDotColor(model.selected?PHColor(0,0,1):PHColor(1,0,0));
    if (!first && model.actor1)
    {
        gm->setGLStates(PHGLVertexArray);
        PHPoint p1 = boundsCenter();
        PHPoint p2 = toMyCoordinates(model.actor1->fromMyCoordinates(model.actor1->boundsCenter()));
        GLfloat v[] = { p1.x,p1.y, p2.x,p2.y };
        glLineWidth(1.0f);
        gm->setColor(color);
        gm->vertexPointer(2, GL_FLOAT, 0, v);
        gm->applyShader(gm->solidColorShader());
        glDrawArrays(GL_LINE_STRIP, 0, 2);
    }
    PLDotView::draw();
}
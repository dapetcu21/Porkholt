//
//  PLFixtureView.mm
//  LevelDesigner
//
//  Created by Marius Petcu on 10/10/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PLFixtureView.h"
#import "PLFixture.h"
#import "PLObjectView.h"
#import "SubentityController.h"
#import "PLObject.h"
#import "ObjectController.h"
#import "PHEventHandler.h"
#import "PLBezier.h"
#import "PHBezierPath.h"
#import "PLBezierView.h"

#ifndef GL_INVALID_INDEX
#define GL_INVALID_INDEX 0xffffffff
#endif

PLFixtureView::PLFixtureView(PLFixture * _model) : model(_model), moving(false), rotating(false), grab(0), curve(NULL), bezierView(NULL), arraysVBO(GL_INVALID_INDEX), indexesVBO(GL_INVALID_INDEX)
{
    setUserInput(true);
    [model retain];
    [model setActor:this];
    modelChanged();
}

PLFixtureView::~PLFixtureView()
{
    [model setActor:NULL];
    changeBezier(NULL, false);
    [model release];
}

void PLFixtureView::bezierChanged(PHObject * sender, void * ud)
{
    PHBezierPath * bp = curve.bezierPath;
    if (bp)
    {
        if (arraysVBO == GL_INVALID_INDEX)
            glGenBuffers(1, &arraysVBO);
        if (indexesVBO == GL_INVALID_INDEX)
            glGenBuffers(1, &indexesVBO);
        
        const vector<PHBezierPath::anchorPoint> * anchors = &bp->calculatedVertices();
        nVertices = (int)anchors->size();
        GLushort * indexes = PHBezierPath::triangulate(*anchors,nIndexes);
        
        glBindBuffer(GL_ARRAY_BUFFER, arraysVBO);
        glBufferData(GL_ARRAY_BUFFER, nVertices*2*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
        GLfloat * arr = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        for (int i = 0 ; i<nVertices; i++)
        {
            PHPoint p = anchors->at(i).point;
            arr[i<<1] = p.x;
            arr[(i<<1)+1] = p.y;
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexesVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (nIndexes+nVertices+1)*sizeof(GLushort), NULL, GL_DYNAMIC_DRAW);
        GLushort * idx = (GLushort*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(idx,indexes,nIndexes*sizeof(GLushort));
        idx+=nIndexes;
        for (int i=0; i<nVertices; i++)
            idx[i] = i;
        idx[nVertices] = 0;
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        delete indexes;
        
    } else {
        if (arraysVBO!=GL_INVALID_INDEX)
            glDeleteBuffers(1, &arraysVBO);
        if (indexesVBO!=GL_INVALID_INDEX)
            glDeleteBuffers(1, &indexesVBO);
        arraysVBO = indexesVBO = GL_INVALID_INDEX;
    }
}

void PLFixtureView::changeBezier(PLBezier * cv, bool showView)
{
    if (curve!=cv)
    {
        if (curve) 
        {
            [curve bezierPath]->removeCallback(this);
            [curve release];
        }
        curve = cv;
        if (curve)
        {
            [curve retain];
            [curve bezierPath]->addCallback(PHInv(this,PLFixtureView::bezierChanged,NULL));
        }
        bezierChanged(this, NULL);
    }
    if (!cv)
        showView = false;
    if (curve!=cv || showView!=(bezierView!=NULL))
    {
        if (bezierView)
        {
            bezierView->removeFromSuperview();
            bezierView->release();
        }
        if (showView)
        {
            [curve setUndoManager:undoManager()];
            bezierView = new PLBezierView;
            bezierView->setModel(curve);
            addSubview(bezierView);
        } else
            bezierView = NULL;
    }
}

void PLFixtureView::modelChanged()
{
    PHRect fr = PHWholeRect;
    if (model.shape == PLFixtureRect | model.shape == PLFixtureFreestyle)
    {
        NSRect box = [model box];
        fr = PHRect(box.origin.x,box.origin.y,box.size.width, box.size.height);
    }
    if (model.shape == PLFixtureCircle)
    {
        NSPoint c = [model position];
        double rad = [model radius];
        fr = PHRect(c.x-rad,c.y-rad,2*rad,2*rad);
    }
    setFrame(fr);
    setRotation(-toRad(model.rotation));
    selectedChanged();
}

void PLFixtureView::selectedChanged()
{
    SubentityController * sc = (SubentityController*)[model owner];
    ObjectController * oc = (ObjectController*)[[sc object] owner];
    bool selected = ([model selected] && [oc objectMode] && ([oc selectedEntity] == [sc object]) && !model.readOnly && ![sc object].readOnly);
    changeBezier((model.shape == PLFixtureFreestyle)?[model bezierCurve]:nil,selected);
    if (bezierView)
        bezierView->setFrame(_bounds);
}

bool PLFixtureView::intersectsRect(PHView * base, const PHRect & rect)
{
    if (model.shape == PLFixtureCircle)
    {
        PHPoint center = fromMyCoordinates(_bounds.center(), base);
        double rad = [model radius];
        if (PHPointInRect(center, rect)) return true;
        for (int i=0; i<4; i++)
            if (PHPointInCircle(rect.corner(i), center, rad))
                return true;
        if (center.y<=rect.y && rect.x<=center.x && center.x<rect.x+rect.width && 
            rect.y-center.y<=rad) return true;
        if (center.x<=rect.x && rect.y<=center.y && center.y<rect.y+rect.height && 
            rect.x-center.x<=rad) return true;
        if (center.y>=rect.y+rect.height && rect.x<=center.x && center.x<rect.x+rect.width && 
            center.y-rect.y-rect.height<=rad) return true;
        if (center.x>=rect.x+rect.width && rect.y<=center.y && center.y<rect.y+rect.height && 
            center.x-rect.x-rect.width<=rad) return true;
        return false;
    }
    if (model.shape == PLFixtureRect || model.shape == PLFixtureFreestyle)
        return PLObjectView::rectsIntersect(base, rect, base, _bounds, this);
    return false;
}

bool PLFixtureView::intersectsPoint(const PHPoint & pnt)
{
    if ([model shape] == PLFixtureRect || model.shape == PLFixtureFreestyle)
        return PHPointInRect(pnt, _bounds);
    if ([model shape] == PLFixtureCircle)
    {
        double r = (pnt-_bounds.center()).length();
        double mr = [model radius];
        return (r<=mr);
    }
    return false;
}

bool PLFixtureView::objectMode()
{
    SubentityController * sc = (SubentityController*)[model owner];
    ObjectController * oc = (ObjectController*)[[sc object] owner];
    return [oc objectMode]&& ([oc selectedEntity] == [sc object]);
}

int PLFixtureView::grabTypeForPoint(const PHPoint &pnt)
{
    if ([model readOnly] || [[model object] readOnly]) return 0;
    PHPoint p = pnt-_bounds.origin();
    if ([model shape] == PLFixtureRect || [model shape] == PLFixtureFreestyle)
    {
        if (p.x<=0.1 && p.y<=0.1) return 1;
        if (p.x>=_bounds.width-0.1 && p.y<=0.1) return 2;
        if (p.x>=_bounds.width-0.1 && p.y>=_bounds.height-0.1) return 3;
        if (p.x<=0.1 && p.y>=_bounds.height-0.1) return 4;
        if (p.x<=0.05) return 5;
        if (p.y<=0.05) return 6;
        if (p.x>=_bounds.width-0.05) return 7;
        if (p.y>=_bounds.height-0.05) return 8;
    }
    if ([model shape] == PLFixtureCircle)
    {
        double r = (p-_bounds.size()/2).length();
        double mr = [model radius];
        if (r<=mr && r>=mr-0.05) return 9;
    }
    return 0;
}

NSUndoManager * PLFixtureView::undoManager()
{
    SubentityController * sc = (SubentityController*)[model owner];
    ObjectController * oc = (ObjectController*)[[sc object] owner];
    return [oc undoManager];
}

void PLFixtureView::resizeToBounds(const PHRect & newBounds)
{
    PHRect bounds = this->bounds();
    NSRect f = [model box];
    PHRect frame = PHRect(f.origin.x,f.origin.y,f.size.width,f.size.height);
    PHSize deltaCenter = fromMyCoordinates(newBounds.center(), superview())-fromMyCoordinates(bounds.center(), superview());
    PHSize deltaSize = (newBounds.size()-bounds.size());
    PHRect nf = frame+deltaCenter-deltaSize/2;
    nf.width = newBounds.width;
    nf.height = newBounds.height;
    [model setBox:NSMakeRect(nf.x, nf.y, nf.width, nf.height)];
}

void PLFixtureView::resizeWithTypeAndDelta(int type, PHPoint delta)
{
    if (model.shape == PLFixtureRect || model.shape == PLFixtureFreestyle)
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
    if (model.shape == PLFixtureCircle && type == 9)
    {
        [model setRadius:model.radius+delta.x];
    }
}

void PLFixtureView::touchEvent(PHEvent * event)
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
                    [ec removeEntity:model inSelectionForArray:1];
                else
                    [ec insertEntity:model inSelectionForArray:1];
                grab = grabTypeForPoint(localPoint);
                if (grab)
                {
                    initialGrabFrame = [model box];
                    initialGrabRadius = [model radius];
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
                PHPoint lastP = toMyCoordinates(event->location());
                PHPoint currP = toMyCoordinates(event->lastLocation());
                PHPoint delta = lastP - currP;
                if (grab == 9)
                {
                    double deltaR = (lastP-_bounds.center()).length() - (currP-_bounds.center()).length();
                    delta.x = deltaR;
                }
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
                if (grab==9)
                {
                    double rad = [model radius];
                    [model setRadius:initialGrabRadius];
                    [undoManager() enableUndoRegistration];
                    [model setRadius:rad];
                } else
                {
                    NSRect fr = [model box];
                    [model setBox:initialGrabFrame];
                    [undoManager() enableUndoRegistration];
                    [model setBox:fr];
                }
                grab = 0;
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

#define borderSize 0.010
#define circleChunks 100

void PLFixtureView::draw()
{
    SubentityController * sc = (SubentityController*)[model owner];
    ObjectController * oc = (ObjectController*)[[sc object] owner];
    bool selected = ([model selected] && [oc objectMode] && ([oc selectedEntity] == [sc object]));
    if (model.shape == PLFixtureRect)
    {
        GLfloat vertices[] = {
            _bounds.x, _bounds.y,
            _bounds.x+_bounds.width, _bounds.x,
            _bounds.x, _bounds.y+_bounds.height,
            _bounds.x+_bounds.width,_bounds.y+_bounds.height
        };
        
        PHGLSetStates(PHGLVertexArray);
        PHGLSetColor(PHColor(0.18,0.24,0.92,0.3));
        glVertexPointer(2, GL_FLOAT, 0, vertices);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        const GLubyte idx[] = {0,1,3,2,0};
        glVertexPointer(2, GL_FLOAT, 0, vertices);
        PHGLSetColor(PHColor(0.7,0.7,1));
        glLineWidth(1.0f);
        glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_BYTE, idx);
    }
    if (model.shape == PLFixtureFreestyle && arraysVBO!=GL_INVALID_INDEX && indexesVBO!=GL_INVALID_INDEX)
    {
        glBindBuffer(GL_ARRAY_BUFFER, arraysVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexesVBO);
        
        PHGLSetStates(PHGLVertexArray);
        PHGLSetColor(PHColor(0.18,0.24,0.92,0.3));
        glVertexPointer(2, GL_FLOAT, 0, NULL);
        glPushMatrix();
        glTranslatef(_bounds.x, _bounds.y, 0);
        glScalef(_bounds.width, _bounds.height, 1);
        glDrawElements(GL_TRIANGLES, nIndexes, GL_UNSIGNED_SHORT, NULL);
        PHGLSetColor(PHColor(0.7,0.7,1));
        glLineWidth(1.0f);
        glDrawElements(GL_LINE_STRIP, nVertices+1, GL_UNSIGNED_SHORT, ((GLushort*)NULL)+nIndexes);
        glPopMatrix();
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    if (selected && [oc showMarkers] && (model.shape == PLFixtureRect || model.shape == PLFixtureFreestyle))
    {
        GLfloat markersVertices[] = {
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
        glVertexPointer(2, GL_FLOAT, 0, markersVertices);
        PHGLSetStates(PHGLVertexArray);
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
    if (model.shape == PLFixtureCircle)
    {
        GLfloat circleVertices [(circleChunks+2)*2];
        double rad = [model radius];
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
        PHGLSetColor(PHColor(0.18,0.24,0.92,0.3));
        glVertexPointer(2, GL_FLOAT, 0, circleVertices);
        glDrawArrays(GL_TRIANGLE_FAN, 0, circleChunks+2);
        
        GLfloat circleBorder[(circleChunks+1)*2];
        for (int i=0; i<=circleChunks; i++)
        {
            double angle = M_PI*2*((double)i/circleChunks);
            circleBorder[i*2] = sin(angle)*rad+center.x;
            circleBorder[i*2+1] = cos(angle)*rad+center.y;
        }
        
        PHGLSetColor(selected?PHColor(0.5,0.5,1):PHColor(0.7,0.7,1));
        glLineWidth(selected?3.0f:1.0f);
        glVertexPointer(2, GL_FLOAT, 0, circleBorder);
        glDrawArrays(GL_LINE_STRIP, 0, circleChunks+1);
    }
}


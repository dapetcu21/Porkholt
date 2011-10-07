//
//  PLWorldView.cpp
//  LevelDesigner
//
//  Created by Marius Petcu on 10/7/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PLWorldView.h"
#include "PHEvent.h"
#include "WorldController.h"
#include "PHEventHandler.h"

PLWorldView::PLWorldView() : dragRect(0,0,0,0), startPoint(0,0)
{
    
}

void PLWorldView::touchEvent(PHEvent * event)
{
    if (event->type() == PHEvent::touchDown)
    {
        if (event->userData() != (void*)1)
        {
            eventHandled = false;
            return;
        }
        int type = 0;
        if (PHEventHandler::modifierMask() & PHEventHandler::commandModifier)
            type = 1;
        if (PHEventHandler::modifierMask() & PHEventHandler::optionModifier)
            type = 2;
        startPoint = toMyCoordinates(event->location());
        dragRect = PHRect(startPoint.x,startPoint.y,0,0);
        [worldController startSelectionOfType:type atPoint:startPoint];
    }
    else
    if (event->type() == PHEvent::touchMoved)
    {
        if (event->userData() != (void*)1)
        {
            eventHandled = false;
            return;
        }
        PHPoint s = toMyCoordinates(event->location())-startPoint;
        dragRect = PHRect(startPoint.x,startPoint.y,s.width,s.height);
        if (dragRect.height<0)
        {
            dragRect.y+=dragRect.height;
            dragRect.height*=-1;
        }
        if (dragRect.width<0)
        {
            dragRect.x+=dragRect.width;
            dragRect.width*=-1;
        }
        [worldController moveSelection:dragRect];
    }
    else
    if (event->type() == PHEvent::touchUp)
    {
        if (event->userData() != (void*)1)
        {
            eventHandled = false;
            return;
        }
        PHPoint s = toMyCoordinates(event->location())-startPoint;
        dragRect = PHRect(startPoint.x,startPoint.y,s.width,s.height);
        if (dragRect.height<0)
        {
            dragRect.y+=dragRect.height;
            dragRect.height*=-1;
        }
        if (dragRect.width<0)
        {
            dragRect.x+=dragRect.width;
            dragRect.width*=-1;
        }
        [worldController endSelection:dragRect];
        dragRect = PHRect(0,0,0,0);
    }
    else
        eventHandled = false;
}

void PLWorldView::draw()
{
    if (!dragRect.width && !dragRect.height) return;
    GLfloat squareVertices[] = {
        dragRect.x, dragRect.y,
        dragRect.x+dragRect.width, dragRect.y,
        dragRect.x,  dragRect.y+dragRect.height,
        dragRect.x+dragRect.width,  dragRect.y+dragRect.height
    };
	
    PHGLSetStates(PHGLVertexArray);
	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
    PHGLSetColor(PHColor(1,1,1,0.5));
               
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}
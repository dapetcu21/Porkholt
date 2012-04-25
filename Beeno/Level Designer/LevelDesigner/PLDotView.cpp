//
//  PLDotView.cpp
//  LevelDesigner
//
//  Created by Marius Petcu on 11/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PLDotView.h"

void PLDotView::draw()
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
    
    gm->setGLStates(PHGLBlending | PHGLVertexArray);
    gm->setColor(color);
    gm->vertexPointer(2, GL_FLOAT, 0, circleVertices);
    gm->applyShader(gm->solidColorShader());
    glDrawArrays(GL_TRIANGLE_FAN, 0, circleChunks+2);
}
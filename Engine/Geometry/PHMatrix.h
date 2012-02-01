//
//  PHMatrix.h
//  Porkholt
//
//  Created by Marius Petcu on 2/1/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHMATRIX_H
#define PHMATRIX_H

#include "PHMain.h"
#include "PHMatrix.h"

void PHInvertMatrix(const GLfloat * m, GLfloat * inverse);
PHPoint PHTransformPointMatrix(const GLfloat * m,const PHPoint & pnt);
PHPoint PHUnTransformPointMatrix(const GLfloat * m, const PHPoint & pnt);
PHPoint PHTransformedPoint(const PHPoint & pnt);
PHPoint PHUnTransformedPoint(const PHPoint & pnt);

void PHGLRotate(ph_float angle);
void PHGLFlip(PHPoint center, bool horiz, bool vert);

#endif

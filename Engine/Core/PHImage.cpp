/*
 *  PHImage.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHImage.h"
#include "PHFileManager.h"
#include "PHNormalImage.h"
#include "PHAnimatedImage.h"
#include "PHBezierPath.h"

#ifdef PHIMAGE_ORDERED_LOADING
PHMutex * PHImage::loadingMutex = new PHMutex;
#endif

PHImage::PHImage(const string & path) : loaded(false) { };

void PHImage::buildImageVBO(GLuint vbo, VBOParams & params, const PHPoint & repeat, const PHRect & portion, const PHRect & texCoord, const PHPoint & adj)
{
    vector<GLfloat> v;
    PHRect r = portion*repeat;
    ph_float lx = r.x;
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    for (ph_float x = ((int)r.x)+1; lx<r.x+r.width; (lx=x),(x+=1))
    {
        if (x>r.x+r.width)
            x=r.x+r.width;
        ph_float ly = r.y;
        if (!v.empty())
            for (int i=0; i<4; i++)
                v.push_back(v[v.size()-4]);
        bool first = !v.empty();
        for (ph_float y = ((int)r.y)+1; ly<r.y+r.height; (ly=y),(y+=1))
        {
            if (y>r.y+r.height)
                y=r.y+r.height;
            PHRect rc = PHRect(lx-floor(lx),ly-floor(ly),x-ceil(x)+1,y-ceil(y)+1);
            
            v.push_back((lx-r.x)/r.width); v.push_back((ly-r.y)/r.height);
            v.push_back(texCoord.x+rc.x    *texCoord.width+adj.x); v.push_back(texCoord.y+rc.height*texCoord.height-adj.y);
            if (first)
            {
                first = true;
                for (int i=0; i<4; i++)
                    v.push_back(v[v.size()-4]);
            }
            
            v.push_back(( x-r.x)/r.width); v.push_back((ly-r.y)/r.height);
            v.push_back(texCoord.x+rc.width*texCoord.width-adj.x); v.push_back(texCoord.y+rc.height*texCoord.height-adj.y);
            
            v.push_back((lx-r.x)/r.width); v.push_back(( y-r.y)/r.height);
            v.push_back(texCoord.x+rc.x    *texCoord.width+adj.x); v.push_back(texCoord.y+rc.y     *texCoord.height+adj.y);
            
            v.push_back(( x-r.x)/r.width); v.push_back(( y-r.y)/r.height);
            v.push_back(texCoord.x+rc.width*texCoord.width-adj.x); v.push_back(texCoord.y+rc.y     *texCoord.height+adj.y);
        }
    }
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*v.size(), &v[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    params.vertexSize = 2;
    params.vertexOffset = 0;
    params.vertexStride = sizeof(GLfloat)*4;
    params.vertexType = GL_FLOAT;
    params.texCoordSize = 2;
    params.texCoordOffset = sizeof(GLfloat)*2;
    params.texCoordStride = sizeof(GLfloat)*4;
    params.texCoordType = GL_FLOAT;
    params.nElements = (int)(v.size()/4);
    params.renderMode = GL_TRIANGLE_STRIP;
}

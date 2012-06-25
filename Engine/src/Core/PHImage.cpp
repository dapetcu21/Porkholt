/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHImage.h>
#include <Porkholt/IO/PHFileManager.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/Core/PHAnimatedImage.h>
#include <Porkholt/Core/PHCurve.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLVertexBufferObject.h>
#include <Porkholt/Core/PHGameManager.h>

#ifdef PHIMAGE_ORDERED_LOADING
PHMutex * PHImage::loadingMutex = new PHMutex;
#endif

PHImage::PHImage(PHGameManager * gameManager) : loaded(false), gm(gameManager), _normalMap(NULL), loadMutex(new PHMutex(true)) { };

void PHImage::buildImageVAO(PHGLVertexArrayObject * vao, PHGLVertexBufferObject * vbo, const PHPoint & repeat, const PHRect & portion, const PHRect & texCoord)
{
    //TO REDO
    vector<GLfloat> v;
    PHRect r = portion*repeat;
    ph_float lx = r.x;
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
            v.push_back(texCoord.x+rc.x    *texCoord.width); v.push_back(texCoord.y+rc.height*texCoord.height);
            if (first)
            {
                first = true;
                for (int i=0; i<4; i++)
                    v.push_back(v[v.size()-4]);
            }
            
            v.push_back(( x-r.x)/r.width); v.push_back((ly-r.y)/r.height);
            v.push_back(texCoord.x+rc.width*texCoord.width); v.push_back(texCoord.y+rc.height*texCoord.height);
            
            v.push_back((lx-r.x)/r.width); v.push_back(( y-r.y)/r.height);
            v.push_back(texCoord.x+rc.x    *texCoord.width); v.push_back(texCoord.y+rc.y     *texCoord.height);
            
            v.push_back(( x-r.x)/r.width); v.push_back(( y-r.y)/r.height);
            v.push_back(texCoord.x+rc.width*texCoord.width); v.push_back(texCoord.y+rc.y     *texCoord.height);
        }
    }
    
    vbo->bindTo(PHGLVBO::arrayBuffer);
    vbo->setData(&v[0], sizeof(GLfloat)*v.size(), PHGLVBO::dynamicDraw);
    vao->bindToEdit();
    vao->vertexPointer(PHIMAGEATTRIBUTE_POS, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, 0, vbo);
    vao->vertexPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, sizeof(GLfloat)*2, vbo);
    vao->setDrawArrays(GL_TRIANGLE_STRIP, 0, (v.size()/4));
    vao->unbind();
    vbo->unbind();
}

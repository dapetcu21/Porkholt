/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Box2D/Box2D.h>
#include "IGWallCurve.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLVertexBufferObject.h>

IGWallCurve::IGWallCurve() : count(0), _offset(0), _width(1), _limit(0)
{
}

IGWallCurve::~IGWallCurve()
{
}

void IGWallCurve::rebuildVAO(PHGLVAO * vao, const PHRect & texCoord)
{
    size_t nVertices;
    GLfloat * r = vertexData(nVertices, texCoord);

    vao->bindToEdit(); 
    PHGLVBO * vbo = vao->attributeVBO(PHIMAGEATTRIBUTE_POS);
    if (!vbo)
        vbo = new PHGLVertexBufferObject(vao->gameManager());
    else
        vbo->retain();
    
    vbo->bindTo(PHGLVBO::arrayBuffer);
    vbo->setData(NULL, nVertices*5*sizeof(GLfloat), PHGLVBO::dynamicDraw);
    vbo->setSubData(r, 0, nVertices*5*sizeof(GLfloat));
    delete r;
    vao->vertexPointer(PHIMAGEATTRIBUTE_POS, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0, vbo);
    vao->vertexPointer(PHIMAGEATTRIBUTE_TXC, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 2*sizeof(GLfloat), vbo);
    vbo->unbind();
    vbo->release();
    
    vao->setElementArrayVBO(NULL);
    vao->setDrawArrays(GL_TRIANGLE_STRIP, 0, nVertices);
    
    vao->unbind();
}

GLfloat * IGWallCurve::vertexData(size_t & nvertices, const PHRect & txc)
{
    if (!count)
    {
        nvertices = 0;
        return NULL;
    }
    nvertices = count * 2;
    ph_float xf = points.front().x + _offset;
    ph_float xl = points.back().x + _offset;
    nvertices += 4*(size_t)(floor(xl/_width) - ceil(xf/_width) + 1);
    GLfloat * v = new GLfloat[nvertices * 5];
    GLfloat * p = v;
    ph_float tx = xf/_width - floor(xf/_width);
    PHPoint lp = points.front();

    for (list<PHPoint>::iterator i = points.begin(); i != points.end(); i++)
    {
        PHPoint pt = *i;
        ph_float ltx = tx;
        tx += (pt.x - lp.x) / _width;
        if (tx >= 1.0f)
        {
            ph_float f = (1.0f - ltx) / (tx - ltx);
            PHPoint pp = lp+(pt-lp)*f;
            ph_float q = (pp.y - _limit);
            p[0] = pp.x;
            p[1] = _limit;
            p[2] = (txc.x + txc.width) * q;
            p[3] = txc.y * q;
            p[4] = q;
            
            p[5] = pp.x;
            p[6] = pp.y;
            p[7] = (txc.x + txc.width) * q;
            p[8] = (txc.y + txc.height) * q;
            p[9] = q;

            p+= 10;
            
            p[0] = pp.x;
            p[1] = _limit;
            p[2] = txc.x * q;
            p[3] = txc.y * q;
            p[4] = q;
            
            p[5] = pp.x;
            p[6] = pp.y;
            p[7] = txc.x * q;
            p[8] = (txc.y + txc.height) * q;
            p[9] = q;

            p+= 10;
            tx-=1.0f;
        }

        ph_float q = (pt.y - _limit);

        p[0] = pt.x;
        p[1] = _limit;
        p[2] = (txc.x + txc.width * tx) * q;
        p[3] = txc.y * q;
        p[4] = q;
        
        p[5] = pt.x;
        p[6] = pt.y;
        p[7] = (txc.x + txc.width * tx) * q;
        p[8] = (txc.y + txc.height) * q;
        p[9] = q;

        p+= 10;
        lp = pt;
    }
    assert((p - v)/5 == nvertices);
    return v;
}

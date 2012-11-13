/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGWallCurve.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHGameManager.h>

#include <Box2D/Box2D.h>

IGWallCurve::IGWallCurve() : count(0), _offset(0), _width(1), _limit(0)
{
}

IGWallCurve::~IGWallCurve()
{
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
    GLfloat * v = new GLfloat[nvertices * 4];
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
            p[0] = pp.x;
            p[1] = _limit;
            p[2] = txc.x + txc.width;
            p[3] = txc.y;
            
            p[4] = pp.x;
            p[5] = pp.y;
            p[6] = txc.x + txc.width;
            p[7] = txc.y + txc.height;

            p+= 8;
            
            p[0] = pp.x;
            p[1] = _limit;
            p[2] = txc.x;
            p[3] = txc.y;
            
            p[4] = pp.x;
            p[5] = pp.y;
            p[6] = txc.x;
            p[7] = txc.y + txc.height;

            p+= 8;
            tx-=1.0f;
        }

        p[0] = pt.x;
        p[1] = _limit;
        p[2] = txc.x + txc.width * tx;
        p[3] = txc.y;
        
        p[4] = pt.x;
        p[5] = pt.y;
        p[6] = txc.x + txc.width * tx;
        p[7] = txc.y + txc.height;

        p+= 8;
        lp = pt;
    }
    assert((p - v)/4 == nvertices);
    return v;
}

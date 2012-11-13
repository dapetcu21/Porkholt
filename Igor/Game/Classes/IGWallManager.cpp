/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Box2D/Box2D.h>
#include "IGWallManager.h"
#include "IGWallCurve.h"
#include "IGWallView.h"
#include "IGWallCell.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHLua.h>

IGSCRIPTING_REGISTERCLASS("IGWallManager", IGWallManager)

IGWallManager::IGWallManager(IGWorld * w) : IGObject(w), upper(0.5), lower(0), velocity(1), width(0.5), epsilon(0.25), ftime(0), view(NULL), curve(NULL)
{
}

IGWallManager::~IGWallManager()
{
    for (list<IGWallCell*>::iterator i = cells.begin(); i != cells.end(); i++)
    {
        (*i)->removeFromWorld();
        (*i)->release();
    }
    view->release();
    curve->release();
}

PHDrawable * IGWallManager::loadDrawable()
{
    view = new IGWallView(world->view()->bounds()); 
    view->setImage(world->gameManager()->imageNamed("wall"));
    curve = new IGWallCurve();
    curve->setLimit(0);
    curve->setWidth(view->bounds().width);
    view->setShape(curve); 
    view->setConstrainCurveToFrame(false);
    return view;
}

void IGWallManager::freset()
{
    ftime = 0;
}

void IGWallManager::fadvance(ph_float elapsed)
{
    ftime += elapsed;
}

ph_float IGWallManager::f(ph_float x)
{
    return (sin(ftime + x) * (upper-lower) + (upper+lower))/2;
}

void IGWallManager::animate(ph_float elapsed)
{
    curve->beginCommitGrouping();
    list<PHPoint>::iterator j = curve->points.begin();
    for (list<IGWallCell*>::iterator i = cells.begin(); i != cells.end(); i++, j++)
    {
        IGWallCell * p = *i;
        PHVector2 v = p->desiredPosition();
        v.x -= velocity * elapsed;
        p->setPosition(v);
        (*j) = p->physicsBody()->GetWorldPoint(b2Vec2(width/2, 0));
    }

    while (!cells.empty())
    {
        IGWallCell * p = cells.front();
        if (p->position().x + width * 1.5 + epsilon < 0)
        {
            p->removeFromWorld();
            p->release();
            cells.pop_front();
            curve->count--;
            curve->points.pop_front();
        } else 
            break;
    }
    while (true)
    {
        IGWallCell * l = cells.empty() ? NULL : cells.back();
        PHVector2 pos;
        ph_float ag;
        if (l)
            pos = l->worldPoint(PHVector2(width/2, 0));
        else {
            ag = 0;
            freset();
            pos = PHVector2(-epsilon, f(0));
        }
        if (pos.x > world->view()->bounds().width)
            break;

        ph_float tx = width;
        ph_float ty = f(tx);
        ph_float dy = ty-pos.y;
        ph_float ln = sqrt(dy*dy + tx * tx);
        tx *= tx/ln;
        dy = f(tx)-pos.y;
        ln = sqrt(dy*dy + tx*tx);
        ag = asin(dy/ln);
        fadvance(tx);

        IGWallCell * cell = new IGWallCell(world);
        cell->setRestrictRotation(false);
        cell->setPosition(pos + PHVector2(width/2, 0).rotated(ag));
        cell->setRotation(ag);
        cell->attachToWorld(world);
        if (l)
            cell->weldToObject(l);
        cells.push_back(cell);

        curve->count++;
        curve->points.push_back(cell->worldPoint(PHVector2(width/2, 0)));
    }
    curve->endCommitGrouping();
}

//--- Lua Scripting ---

PHLuaNumberGetter(IGWallManager, upperMargin);
PHLuaNumberGetter(IGWallManager, lowerMargin);
PHLuaNumberGetter(IGWallManager, wallVelocity);
PHLuaNumberGetter(IGWallManager, cellWidth);
PHLuaNumberGetter(IGWallManager, leftEpsilon);

PHLuaNumberSetter(IGWallManager, setUpperMargin);
PHLuaNumberSetter(IGWallManager, setLowerMargin);
PHLuaNumberSetter(IGWallManager, setWallVelocity);
PHLuaNumberSetter(IGWallManager, setCellWidth);
PHLuaNumberSetter(IGWallManager, setLeftEpsilon);

void IGWallManager::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGWallManager");

    PHLuaAddMethod(IGWallManager, upperMargin);
    PHLuaAddMethod(IGWallManager, lowerMargin);
    PHLuaAddMethod(IGWallManager, wallVelocity);
    PHLuaAddMethod(IGWallManager, cellWidth);
    PHLuaAddMethod(IGWallManager, leftEpsilon);
    PHLuaAddMethod(IGWallManager, setUpperMargin);
    PHLuaAddMethod(IGWallManager, setLowerMargin);
    PHLuaAddMethod(IGWallManager, setWallVelocity);
    PHLuaAddMethod(IGWallManager, setCellWidth);
    PHLuaAddMethod(IGWallManager, setLeftEpsilon);

    lua_pop(L, 1);
}

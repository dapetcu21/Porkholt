/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PH2DCamera.h>
#include <Porkholt/Core/PHViewControllerHolder.h>
#include <Porkholt/IO/PHDirectory.h>
#include "IGLevelController.h"

void IGGameEntryPoint(PHGameManager * gm)
{    
    PH2DCamera * camera = new PH2DCamera();
    camera->setScreenSize(PHSize(0,4.0f));
    gm->setMainDrawable(camera);
    PHRect bounds = camera->realScreenBounds();
    PHViewControllerHolder * holder = new PHViewControllerHolder(bounds);
    camera->addChild(holder);
    
    PHNavigationController * nav = new PHNavigationController(gm);
    holder->setViewController(nav);
    
    camera->release();
    holder->release();
    nav->release();

    PHDirectory * dir = gm->resourceDirectory()->directoryAtPath("story");
    IGLevelController * vc = new IGLevelController(gm, dir);
    nav->pushViewController(vc);
}

PHMAIN_DEFINE
{
    srand(time(NULL));
    PHMAIN_RETURN(PHWMain(PHMAIN_ARGS, 
        PHWVideoMode(480*2, 320*2, PHWVideoMode::Windowed), 
        PHWVSync
#if defined(PH_SIMULATOR) || (defined (PH_DEBUG)) 
        | PHWRemote
        | PHWShowFPS
#endif
        ,
        &IGGameEntryPoint, 
        NULL));
}
 

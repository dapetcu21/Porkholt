/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include "PHMenuController.h"

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHMenuController * vc = new PHMenuController(gm);
    gm->setUpNavigationController()->pushViewController(vc);
    vc->release();
}

PHMAIN_DEFINE
{
    PHMAIN_RETURN(PHWMain(PHMAIN_ARGS, 
                   PHWVideoMode(800, 600, 60, PHWVideoMode::Windowed),
                   PHWGLES1 
#if defined(PH_DEBUG)
                   | PHWRemote
                   | PHWShowFPS
#endif
                   | PHWVSync
                   , &PHGameEntryPoint, NULL));
}

/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include "PHGameController.h"

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHGameController * vc = new PHGameController(gm);
    gm->setUpNavigationController()->pushViewController(vc);
}

PHMAIN_DEFINE
{
    srand(time(NULL));
    PHMAIN_RETURN(PHWMain(PHMAIN_ARGS, PHWVSync
#if defined(PH_SIMULATOR) || (defined(PH_DESKTOP) && defined (PH_DEBUG)) 
                   | PHWRemote
                   | PHWShowFPS
#endif
                   , &PHGameEntryPoint, NULL));
}
 

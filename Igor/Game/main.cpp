/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include "PHGameController.h"

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHGameController * vc = new PHGameController(gm);
    gm->setUpNavigationController()->pushViewController(vc);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    return PHWMain(argc, argv, PHWVSync
#if defined(PH_SIMULATOR) || (defined(PH_DESKTOP) && defined (PH_DEBUG)) 
                   | PHWRemote
                   | PHWShowFPS
#endif
                   , &PHGameEntryPoint, NULL);
}
 

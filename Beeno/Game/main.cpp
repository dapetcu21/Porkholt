/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include "PHMenuController.h"

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHMenuController * vc = new PHMenuController(gm);
    gm->setUpNavigationController()->pushViewController(vc);
    vc->release();
}

int main(int argc, char *argv[]) {
    return PHWMain(argc, 
                   argv,
                   PHWVideoMode(800, 600, 60, PHWVideoMode::Windowed),
                   PHWGLES1 
#if defined(PH_SIMULATOR) || (defined(PH_DESKTOP) && defined (PH_DEBUG)) 
                   | PHWRemote
                   | PHWShowFPS
#endif
                   | PHWVSync
                   , &PHGameEntryPoint, NULL);
}

//
//  main.cpp
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Porkholt Labs!. All rights reserved.
//

#include "PHWindowing.h"
#include "PHMenuController.h"

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHMenuController * vc = new PHMenuController();
	vc->init(gm);
    gm->navigationController()->pushViewController(vc);
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

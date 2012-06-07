/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "PHStartGame.h"
#include "PHGameController.h"

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHGameController * vc = new PHGameController();
	vc->init(gm);
    gm->navigationController()->pushViewController(vc);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    return PHStartGame(argc, argv, PHStartGame_GLES2 /*| PHStartGame_30FPS */, &PHGameEntryPoint,NULL);
}
 

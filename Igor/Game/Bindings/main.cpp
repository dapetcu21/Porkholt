//
//  main.cpp
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Porkholt Labs!. All rights reserved.
//

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
 
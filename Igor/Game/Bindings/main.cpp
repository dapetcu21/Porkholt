//
//  main.cpp
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Porkholt Labs!. All rights reserved.
//

#import "PHStartGame.h"
#include "PHTestViewController.h"

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHTestViewController * vc = new PHTestViewController();
	vc->init(gm);
    gm->navigationController()->pushViewController(vc);
}

int main(int argc, char *argv[]) {
    return PHStartGame(argc, argv, PHStartGame_GLES1 | PHStartGame_GLES2 | PHStartGame_frameAnimation,&PHGameEntryPoint,NULL);
}
 
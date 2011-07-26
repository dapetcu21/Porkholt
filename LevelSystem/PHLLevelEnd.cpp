//
//  PHLLevelEnd.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/26/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLLevelEnd.h"

PHLLevelEnd::PHLLevelEnd() 
{
    _class = "PHLLevelEnd"; 
}

void PHLLevelEnd::objectEntered(PHLObject * obj)
{
    PHLog("Level should have ended");
}
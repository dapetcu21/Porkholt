/*
 *  PHLevelSystem.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHLEVELSYSTEM_H
#define PHLEVELSYSTEM_H

#include "PHLevelController.h"
#include "PHWorld.h"
#include "PHJoint.h"
#ifndef INSIDE_PHJOINT_H
    #include "PHDistanceJoint.h"
    #include "PHMotorJoint.h"
    #ifndef INSIDE_PHMOTORJOINT_H
        #include "PHRevoluteJoint.h"
        #include "PHPrismaticJoint.h"
    #endif
#endif

#ifndef INSIDE_PHLOBJECT_H
	#include "PHLObject.h"
	#include "PHLCamera.h"
	#include "PHLPlayer.h"
#endif

#endif
/*
 *  PHTime.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHTIME_H
#define PHTIME_H

class PHTime
{
public:
	static double getTime();
	static void sleep(double time);
};

#endif
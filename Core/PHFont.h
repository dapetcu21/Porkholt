//
//  PHFont.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHFONT_H
#define PHFONT_H

#include "PHMain.h"

class PHFont : public PHObject
{
public:
	static PHFont* fontFromPath(const string & path);
	static PHFont* fontNamed(const string & name);
	static void collectGarbage();
	
protected:
	static map<string,PHFont*> fonts;
	PHFont(const string & path);

};

#endif
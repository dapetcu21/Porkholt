//
//  PHPoofView.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/28/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHPOOFVIEW_H
#define PHPOOFVIEW_H

#include "PHImageView.h"

class PHPoofView : public PHImageView
{
public:
    static PHImage * poofImage();
    static void poofImageRelease();
    PHPoofView(const PHRect & frame);
    PHPoofView();
private:
    void init();
    void destroy(PHObject * sender, void * ud);
    
    static PHImage * img;
};

#endif

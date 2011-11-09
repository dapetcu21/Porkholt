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
    
    enum images
    {
        poof = 0,
        boom
    };

    static PHImage * poofImage();
    static PHImage * boomImage();
    static void poofImageRelease();
    static void boomImageRelease();
    
    PHPoofView(const PHRect & frame, int image);
    PHPoofView(int image);
private:
    static PHImage * poofImg, * boomImg;
    
    void init(int image);
    void destroy(PHObject * sender, void * ud);
};

#endif

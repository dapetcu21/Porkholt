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

    static PHImage * poofImage(PHGameManager * man);
    static PHImage * boomImage(PHGameManager * man);
    
    PHPoofView(const PHRect & frame, int image);
    PHPoofView(int image);
private:
    int img;
    void destroy(PHObject * sender, void * ud);
    
    void attachedToGameManager();
};

#endif

//
//  PHShieldView.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/29/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHSHIELDVIEW_H
#define PHSHIELDVIEW_H

#include "PHView.h"

class PHImageView;
class PHImage;

class PHShieldView : public PHView
{
private:
    PHImageView * imv;
    
    void attachedToGameManager();
public:
    PHShieldView(const PHRect & fr);
    ~PHShieldView();
    
    void dismiss();
    
    static PHImage * shieldImage(PHGameManager * gm);
    static void shieldImageRelease();
    
};


#endif

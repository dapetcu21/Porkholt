/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSHIELDVIEW_H
#define PHSHIELDVIEW_H

#include <Porkholt/Core/PHView.h>

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

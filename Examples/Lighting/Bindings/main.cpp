//
//  main.cpp
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Porkholt Labs!. All rights reserved.
//

#import "PHStartGame.h"
#include "PHViewController.h"
#include "PHDeferredView.h"
#include "PHImageView.h"
#include "PHImage.h"


class PHIgorController : public PHViewController
{
    
protected:
#define numLights 6
    
    struct {
        PHGLPointLight * light;
        PH3DPoint pnt;
        ph_float rot;
        ph_float rad;
        ph_float speed;
    } a[numLights];
    
    PHView * loadView(const PHRect & frm)
    {
        PHView * v = new PHView(frm);
        v->setBackgroundColor(PHBlackColor);
        
        PHDeferredView * dv = new PHDeferredView(frm);
        dv->setBackgroundColor(PHWhiteColor);
        for (int i = 0; i<numLights; i++)
        {
            PHColor c = PHWhiteColor;
            switch (i&3)
            {
                case 1:
                    c = PHColor(1,0,0);
                    break;
                case 2:
                    c = PHColor(0,1,0);
                    break;
                case 3:
                    c = PHColor(0,0,1);
                    break;
            }
            a[i].light = dv->addPointLight(a[i].pnt = PH3DPoint(rand()%1024,rand()%756,100+rand()%100),c,150.0f);
            a[i].rot = (rand()/(ph_float)RAND_MAX)*M_PI*2;
            a[i].rad = 50+rand()%100;
            a[i].speed = ((rand()/(ph_float)RAND_MAX)-0.5)*8.0;
        }

        dv->setAmbientColor(PHColor(0.1,0.1,0.1,1.0));
        dv->setNormalMapping(true);
        
        PHImage * img = _gameManager->imageNamed("igor");
        ph_float height = img->height()/2;
        ph_float width = img->width()/2;
        
#define columns 2
#define rows 3
        PHImageView * tv = new PHImageView(PHRect(0,0,width*columns,height*rows));
        tv->setRepeatX(columns);
        tv->setRepeatY(rows);
        tv->setImage(img);
        dv->addSubview(tv);
        tv->release();
        
        v->addSubview(dv);
        dv->release();
        
        return v;
    }
    
    void updateScene(ph_float elapsed)
    {
        for (int i=0; i<numLights; i++)
        {
            a[i].rot += a[i].speed * elapsed;
            a[i].light->position = a[i].pnt + PH3DPoint(a[i].rad*sin(a[i].rot),a[i].rad*cos(a[i].rot),0);
        }
    }
};

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHIgorController * vc = new PHIgorController();
	vc->init(gm);
    gm->navigationController()->pushViewController(vc);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    return PHStartGame(argc, argv, PHStartGame_GLES2 /*| PHStartGame_30FPS */, &PHGameEntryPoint,NULL);
}
 

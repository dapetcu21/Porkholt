/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/UI/PHTestViewController.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/IO/PHFile.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>

const char * file;

class ViewController : public PHViewController, PHDrawableInputDelegate
{
    public:
    PHImage * img;
    ViewController(PHGameManager * gm) : PHViewController(gm)
    {
        img = gm->imageFromFile(PHFile::fileAtFSPath(file));
        PHWSetVideoMode(PHWVideoMode((float)img->width()/img->height() * 400, 400, PHWVideoMode::Windowed));
    }

    void drawableRecievedEvent(PHDrawable * drawable, PHEvent * event)
    {
        PHRect b = ((PHView*)drawable)->bounds();
        PHPoint pnt = event->drawableLocation()->pointInView(drawable);
        PHLog("%f %f  %f %f", pnt.x/b.width, pnt.y/b.height, (pnt.x/b.width-0.5)*2, (pnt.y/b.height-0.5)*2);
    }

    PHView * loadView(const PHRect & r)
    {
        PHImageView * iv = new PHImageView(r);
        iv->setImage(img);
        iv->setUserInput(true);
        iv->setInputDelegate(this);
        return iv;
    }
};

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHViewController * vc = new ViewController(gm);
    gm->setUpNavigationController()->pushViewController(vc);
    vc->release();
}

int main(int argc, char * argv[])
{
    if (argc < 2) return 1;
    file = argv[1];
    return PHWMain(argc, argv, PHWVideoMode(800,600, PHWVideoMode::Windowed), PHWVSync, &PHGameEntryPoint, NULL);
}
 

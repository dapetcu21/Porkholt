/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/Sound/PHSound.h>
#include <Porkholt/Sound/PHRawDecoder.h>
#include <Porkholt/Sound/PHAudioBuffer.h>

class PHPlayerController : public PHViewController
{
    PHSound * snd;

    void loadSound()
    {
        uint8_t * b = new uint8_t[441000];
        for (int i=0; i<441000; i++)
        {
            ph_float v = sin(M_PI*(ph_float(i)/100)) * (sin(M_PI*(ph_float(i)/44100))+1)/2;
            b[i] = uint8_t(v*127 + 127);
        }

        PHRawDecoder * dec = new PHRawDecoder(b, 441000, 44100, 1, PHDecoder::U8);
        PHAudioBuffer * buf = new PHAudioBuffer(dec);
        dec->release();
        snd = new PHSound(gm->soundManager());
        snd->setBuffer(buf);
        buf->release();
    }

    ~PHPlayerController()
    {
        snd->release();
    }

    PHView * playerView(const PHRect & frame)
    {
        PHView * pv = new PHView(frame);
        pv->setUserInput(true);
        pv->setBackgroundColor(PHColor(1.0, 1.0, 1.0, 0.3));
        return pv;
    }

#define playerborder 30
#define playerheight 100

    PHView * loadView(const PHRect & frame)
    {
        PHView * v = new PHView(frame);
        v->setUserInput(true);
        v->setAutoresizeMask(PHView::ResizeAll);

        loadSound();

        PHRect pvfr = v->bounds();
        pvfr.x += playerborder;
        pvfr.y += playerborder;
        pvfr.width -= 2*playerborder;
        pvfr.height = playerheight;
        PHView * pv = playerView(pvfr);
        pv->setAutoresizeMask(PHView::ResizeFixedDown | PHView::ResizeFixedLeft | PHView::ResizeFixedRight | PHView::ResizeFlexibleWidth);
        v->addChild(pv);
        pv->release();

        return v;
    }
};

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHViewController * vc = new PHPlayerController();
    vc->init(gm);
    gm->navigationController()->pushViewController(vc);
    vc->release();
}

int main(int argc, char *argv[]) {
    return PHWMain(argc, argv, PHWGLES1 | PHWVSync | PHWResizable, &PHGameEntryPoint,NULL);
}
 

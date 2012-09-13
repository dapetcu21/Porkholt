/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHViewController.h>
#include <Porkholt/Core/PHTextView.h>

class PHButtonTextView : public PHTextView
{
    void init()
    {
        setUserInput(true);
    }
    PHInvocation cb;
    void touchEvent(PHEvent * evt)
    {
        if (evt->type() == PHEvent::touchDown)
        {
            evt->setHandled(true);
            cb.call(this);
        }
    }
    public:
    PHButtonTextView() : PHTextView() { init(); }
    PHButtonTextView(const PHRect & r) : PHTextView(r) { init(); }
    void setCallback(const PHInvocation & i) { cb = i; }
};

class PHModeTextView : public PHTextView
{
    PHWVideoMode vm;
    public:
    PHModeTextView(const PHRect & r, const PHWVideoMode & v) : PHTextView(r), vm(v)
    {
        setUserInput(true);
        switch (vm.type) 
        {
            case PHWVideoMode::Windowed:
                setText("Windowed");
                break;
            case PHWVideoMode::FullscreenWindowed:
                setText("Full screen (Windowed)");
                break;
            case PHWVideoMode::Fullscreen:
                stringstream ss;
                ss<<"Full screen "<<v.width<<"x"<<v.height<<"@"<<v.refresh;
                setText(ss.str());
                break;
        }
    }
    private:
    void touchEvent(PHEvent * evt)
    {
        if (evt->type() == PHEvent::touchDown)
        {
            evt->setHandled(true);
            PHLog("%s", text().c_str());
            PHWSetVideoMode(vm);
        }
    };
};

class PHVideoModeViewController : public PHViewController
{
public:
    PHVideoModeViewController(PHGameManager * gm) : PHViewController(gm) {} 
protected:
    PHTextView * resView;

    void closeWindow()
    {
        PHWClose();
    }

    void toggleResizable()
    {
        PHWSetResizable(!PHWGetResizable());
    }

    void toggleVSync()
    {
        PHWSetVSync(!PHWGetVSync());
    }

    PHView * loadView(const PHRect & r)
    {
        PHView * v = new PHView(r);
        v->setAutoresizeMask(PHView::ResizeAll);
        v->setUserInput(true);
        v->setAutoresizesSubviews(true);
        
        const vector<PHWVideoMode> & modes = PHWSupportedModes();
        for (size_t i=0; i<modes.size(); i++)
        {
#define tHeight 20
#define tWidth 200
            PHTextView * tv = new PHModeTextView(PHRect(0, r.height - tHeight*(i+1), tWidth, tHeight), modes[i]);
            tv->setFont(gm->fontNamed("Helvetica"));
            tv->setFontColor(PHWhiteColor);
            tv->setFontSize(tHeight);
            tv->setBackgroundColor(PHColor(0.3,0.3,0.3,1.0));
            tv->setAutoresizeMask(PHView::ResizeFixedUp | PHView::ResizeFixedLeft);
            v->addChild(tv);
            tv->release();
        }

        resView = new PHTextView(PHRect(r.width - tWidth, r.height - tHeight, tWidth, tHeight));
        resView->setFont(gm->fontNamed("Helvetica"));
        resView->setFontColor(PHWhiteColor);
        resView->setFontSize(tHeight);
        resView->setBackgroundColor(PHColor(0.3,0.3,0.3,1.0));
        resView->setAutoresizeMask(PHView::ResizeFixedUp | PHView::ResizeFixedRight);
        v->addChild(resView);
 
        PHButtonTextView * tv = new PHButtonTextView(PHRect(r.width - tWidth, r.height - 2*tHeight, tWidth, tHeight));
        tv->setFont(gm->fontNamed("Helvetica"));
        tv->setFontColor(PHWhiteColor);
        tv->setFontSize(tHeight);
        tv->setBackgroundColor(PHColor(0.3,0.3,0.3,1.0));
        tv->setAutoresizeMask(PHView::ResizeFixedUp | PHView::ResizeFixedRight);
        tv->setCallback(PHInv(this, PHVideoModeViewController::toggleResizable, NULL));
        tv->setText("Toggle Resizable");
        v->addChild(tv);
        tv->release();

        tv = new PHButtonTextView(PHRect(r.width - tWidth, r.height - 3*tHeight, tWidth, tHeight));
        tv->setFont(gm->fontNamed("Helvetica"));
        tv->setFontColor(PHWhiteColor);
        tv->setFontSize(tHeight);
        tv->setBackgroundColor(PHColor(0.3,0.3,0.3,1.0));
        tv->setAutoresizeMask(PHView::ResizeFixedUp | PHView::ResizeFixedRight);
        tv->setCallback(PHInv(this, PHVideoModeViewController::toggleVSync, NULL));
        tv->setText("Toggle VSync");
        v->addChild(tv);
        tv->release();


        tv = new PHButtonTextView(PHRect(r.width - tWidth, r.height - 4*tHeight, tWidth, tHeight));
        tv->setFont(gm->fontNamed("Helvetica"));
        tv->setFontColor(PHWhiteColor);
        tv->setFontSize(tHeight);
        tv->setBackgroundColor(PHColor(0.3,0.3,0.3,1.0));
        tv->setAutoresizeMask(PHView::ResizeFixedUp | PHView::ResizeFixedRight);
        tv->setCallback(PHInv(this, PHVideoModeViewController::closeWindow, NULL));
        tv->setText("Close");
        v->addChild(tv);
        tv->release();
        
        return v;
    }
    void updateScene(ph_float elapsedTime)
    {
        stringstream ss;
        ss<<int(gm->screenWidth())<<"x"<<int(gm->screenHeight())<<"@"<<gm->framesPerSecond();
        resView->setText(ss.str());
    }
};

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHViewController * vc = new PHVideoModeViewController(gm);
    gm->setUpNavigationController()->pushViewController(vc);
}

int main(int argc, char *argv[]) {
    return PHWMain(argc, argv, PHWVideoMode(800, 600, 60, PHWVideoMode::Windowed), PHWGLES1 | PHWResizable | PHWShowFPS, &PHGameEntryPoint,NULL);
}
 

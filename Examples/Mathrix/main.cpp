/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/UI/PHTestViewController.h>
#include <Porkholt/UI/PHButtonView.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHImageAnimator.h>
#include <Porkholt/Core/PHTextView.h>
#include <Porkholt/Core/PHEvent.h>
#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/Sound/PHSound.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>
#include <sstream>

static const int width = 100;
static const int height = 50;

class PHMathrisViewController : public PHViewController
{
public:
    PHMathrisViewController(PHGameManager * gm) : PHViewController(gm) {} 

protected:
    int lines, cols;
    int num;
    bool dn;
    int score;

    vector< vector<int> > vec;
    vector< vector<PHImageView*> > img;
    PHView * pv;

    PHImageView * block;
    PHTextView * text;
    int lane;

    ph_float speed;

    void setLane(int l) { if ((l>=0) && (l<cols)) shift(l - lane); }

    class PHLaneView : public PHView
    {
    public:
        PHMathrisViewController * c;
        void touchEvent(PHEvent * evt)
        {
            if (evt->type() == PHEvent::touchDown)
            {
                evt->setHandled(true);
                c->setLane(evt->drawableLocation()->pointInView(this).x/width);
            }
        }
    };

    PHSound * snd;

    PHView * loadView(const PHRect & r)
    {
        PHImageView * v = new PHImageView(r);
        v->setImage(gm->imageNamed("bg"));
        v->setUserInput(true);
        
        cols = r.width / width;
        lines = r.height / height;

        PHView * c = new PHView(PHRect(
           (r.width - cols * width) / 2, 
           0,
           cols * width,
           lines * height));
        c->setUserInput(c);
        v->addChild(c);

        for (int i = 0; i<cols; i++)
        {
            vec.push_back(vector<int>());
            img.push_back(vector<PHImageView*>());
        }

        PHButtonView * lbut = new PHButtonView(PHRect(0,0,cols*width/3,height));
        lbut->setImage(gm->imageNamed("lb_up"));
        lbut->setPressedImage(gm->imageNamed("lb_down"));
        lbut->setDownCallback(PHInv(this, PHMathrisViewController::left, NULL));
        c->addChild(lbut);
        lbut->release();

        PHButtonView * mbut = new PHButtonView(PHRect(cols*width/3,0,cols*width/3,height));
        mbut->setImage(gm->imageNamed("mb_up"));
        mbut->setPressedImage(gm->imageNamed("mb_down"));
        mbut->setDownCallback(PHInv(this, PHMathrisViewController::down, NULL));
        mbut->setUpCallback(PHInv(this, PHMathrisViewController::downRel, NULL));
        c->addChild(mbut);
        mbut->release();

        PHButtonView * rbut = new PHButtonView(PHRect(cols*width/3*2,0,cols*width/3,height));
        rbut->setImage(gm->imageNamed("rb_up"));
        rbut->setPressedImage(gm->imageNamed("rb_down"));
        rbut->setDownCallback(PHInv(this, PHMathrisViewController::right, NULL));
        c->addChild(rbut);
        rbut->release();        
        c->release();
        
        PHRect rr = c->bounds();
        rr.y += height;
        rr.height -= height;
        pv = new PHLaneView();
        pv->setFrame(rr);
        pv->setUserInput(true);
        ((PHLaneView*)pv)->c = this;
        c->addChild(pv);

        text = new PHTextView(v->bounds());
        text->setFont(gm->fontNamed("Helvetica"));
        text->setFontColor(PHWhiteColor);
        text->setAlignment(PHTextView::justifyLeft | PHTextView::alignTop);
        text->setFontSize(40);
        text->setText("0");
        v->addChild(text);
        text->release();

        snd = gm->soundManager()->soundNamed("explosion");

        lines --;
        
        block = NULL;
        score = 0;
        speed = 200;
        dn = false;

        return v;
    }

    void updateScore()
    {
        ostringstream oss;
        oss<<score;
        text->setText(oss.str());
    }

    void removeBrick(int lane, int c)
    {
        int n = vec[lane].size();
        PHView * v = img[lane][c];
        for (int i = c+1; i<n; i++)
        {
            vec[lane][i-1] = vec[lane][i];
            img[lane][i-1] = img[lane][i];
            PHImageView * v = img[lane][i];
            v->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeInFunction);
            v->animateMove(PHPoint(0, -height));
            v->commitCinematicAnimation();
        }
        vec[lane].pop_back();
        img[lane].pop_back();
        
        PHImageView * iv = new PHImageView(v->frame());
        iv->setImage(gm->imageNamed("boom"));
        iv->animator()->animateSection("poof", PHInv(iv, PHView::removeFromParent, NULL));
        if (!snd->isPlaying())
            snd->play();
        pv->addChild(iv);
        iv->release();
        v->removeFromParent();
        score += 50;
        updateScore();
    }

    void updateScene(ph_float elapsed)
    {
        if (!block)
        {
            lane = rand()%cols;
            num = rand()%10+1;
            block = new PHImageView(PHRect(width * lane, height * lines, width, height)); 
            block->setImage(gm->imageNamed("brick"));
            PHColor clr(rand()%(255-50)+50,
                        rand()%(255-50)+50,
                        rand()%(255-50)+50,
                        255);
            clr *= 1/255.0f;
            block->setTintColor(clr);
            block->setShader(gm->shaderProgramNamed("rares_shader"));
            PHTextView * tv = new PHTextView(block->bounds());
            tv->setFont(gm->fontNamed("Helvetica"));
            tv->setFontColor(PHWhiteColor);
            tv->setAlignment(PHTextView::justifyCenter | PHTextView::alignCenter);
            tv->setFontSize(40);
            ostringstream oss;
            int dif;
            int sgn = rand()%5;
            if (sgn<3)
            {
                oss<<num;
            } else {
                if (sgn<4)
                {
                    dif = rand()%10;
                    oss<<num+dif<<'-'<<dif;
                } else {
                    dif = rand()%num;
                    oss<<num-dif<<'+'<<dif;
                }
            }
            tv->setText(oss.str());
            block->addChild(tv);
            tv->release();
            pv->addChild(block);
        }
        PHRect r = block->frame();
        r.y -= elapsed * (dn?(speed*3):speed);
        ph_float l = vec[lane].size() * height;
        bool b = false;
        if (r.y<=l)
        {
            r.y = l;
            vec[lane].push_back(num);
            img[lane].push_back(block);
            b = true;
        }
        block->setFrame(r);
        if (b)
        {
            block->release();
            block = NULL;
            int curp = vec[lane].size()-1;
            bool dd, dl, dr;
            dd = ((vec[lane].size()>=2) && (num == vec[lane][curp-1]));
            dl = ((lane>0) && (vec[lane-1].size()>curp) && (vec[lane-1][curp] == num));
            dr = ((lane<cols-1) && (vec[lane+1].size()>curp) && (vec[lane+1][curp] == num));
            if (dd || dl || dr)
                removeBrick(lane, curp);
            if (dd)
                removeBrick(lane, curp-1);
            if (dl)
                removeBrick(lane-1, curp);
            if (dr)
                removeBrick(lane+1, curp);

            if (vec[lane].size() >= lines)
            {
                for (int i = 0; i<cols; i++)
                {
                    while (vec[i].size())
                    {
                        PHView * v = img[i].back();
                        v->removeFromParent();
                        vec[i].pop_back();
                        img[i].pop_back();
                    }
                }
            }
        }
    }

    void shift(int off)
    {
        if (!block) return;
        PHRect r = block->frame();
        int ln = lane + off;
        if (ln<0 || ln>=cols)
            return;
        if (off>0)
        {   
            for (int i = lane+1; i<=lane+off; i++)
                if (r.y < vec[i].size() * height)
                {
                    ln = i-1;
                    break;
                }
        } else {
            for (int i = lane-1; i>=lane+off; i--)
                if (r.y < vec[i].size() * height)
                {
                    ln = i+1;
                    break;
                }            
        }
        if (ln != lane)
        {
            off = ln - lane;
            lane = ln;
            block->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeOutFunction);
            block->animateMove(PHVector2(off * width, 0));
            block->commitCinematicAnimation();
        }
    }

    void left()
    {
        shift(-1);
    }

    void right()
    {
        shift(1);
    }

    void down()
    {
        dn = true;
    }

    void downRel()
    {
        dn = false;
    }
};

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHViewController * vc = new PHMathrisViewController(gm);
    gm->setUpNavigationController()->pushViewController(vc);
    vc->release();
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    return PHWMain(argc, argv, PHWVideoMode(800,600,PHWVideoMode::Windowed), PHWGLES1 | PHWVSync, &PHGameEntryPoint,NULL);
}
 

/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/UI/PHTestViewController.h>
#include <Porkholt/UI/PHButtonView.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHTextView.h>
#include <sstream>

static const int width = 100;
static const int height = 50;

class PHMathrisViewController : public PHViewController
{
    int lines, cols;
    int num;
    bool dn;

    vector< vector<int> > vec;
    vector< vector<PHImageView*> > img;
    PHView * pv;

    PHImageView * block;
    int lane;

    ph_float speed;

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
        pv = new PHView(rr);
        c->addChild(pv);

        lines --;
        
        block = NULL;
        speed = 200;
        dn = false;

        return v;
    }


    void updateScene(ph_float elapsed)
    {
        if (!block)
        {
            lane = rand()%cols;
            num = rand()%10+1;
            block = new PHImageView(PHRect(width * lane, height * lines, width, height)); 
            block->setImage(gm->imageNamed("brick"));
            static const PHColor clrs[] = { PHColor(0,0,1), PHColor(1,0,0) };
            block->setTintColor(clrs[rand()%(sizeof(clrs)/sizeof(PHColor))]);
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
        r.y -= elapsed * (dn?(speed*2):speed);
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
            if ((vec[lane].size()>=2) && (num == vec[lane][vec[lane].size()-2]))
            {
                PHView * v;
                v = img[lane].back(); v->removeFromParent(); img[lane].pop_back();
                v = img[lane].back(); v->removeFromParent(); img[lane].pop_back();
                vec[lane].pop_back();
                vec[lane].pop_back();
            }
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
        if (r.y >= vec[ln].size() * height)
        {
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
    PHViewController * vc = new PHMathrisViewController();
    vc->init(gm);
    gm->navigationController()->pushViewController(vc);
    vc->release();
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    return PHWMain(argc, argv, PHWGLES1 | PHWVSync, &PHGameEntryPoint,NULL);
}
 

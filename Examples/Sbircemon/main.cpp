/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHTextView.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/UI/PHButtonView.h>
#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/Sound/PHSound.h>
#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/UI/PHTestViewController.h>

class PHMob : public PHObject
{
    PHImageView * v;
    PHRect r;
    float hp;
    bool side;
    PHGameManager * gm;
    public:
        virtual string nameOfMove(int i) = 0;
        virtual void executeMove(int i, PHMob * otherMob) 
        {
            PHView * ov = otherMob->view();
            PHPoint delta = ov->position() - v->position();
            float scale = ov->frame().width / v->frame().width;
            v->beginCinematicAnimation(0.5);
            v->animateMove(delta * 0.8);
            v->animateScale(PHSize(scale, scale));
            v->animationCallback(PHInv(this, PHMob::sound, NULL));
            v->chainCinematicAnimation(0.5);
            v->animateMove(delta * -0.8);
            v->animateScale(PHSize(1.0/scale, 1.0/scale));
            v->commitCinematicAnimation();

            otherMob->loseHP(5+rand()&0x1f);
        }

        void sound()
        {
            gm->soundManager()->soundNamed("tackle")->play();
        }

        virtual string name() = 0;
        virtual string imageName() = 0;
        void setRect(const PHRect & rr) { r = rr; }
        void setSide(bool s) { side = s; }
        void setGameManager(PHGameManager * _gm) { gm = _gm; }
        PHView * view()
        {
            if (!v)
            {
                PHRect s;
                if (side)
                {
                    s.x = r.width * 0.6;
                    s.y = r.height * 0.5;
                    s.width = r.width * 0.25;
                } else {
                    s.x = r.width * 0.1;
                    s.y = r.height * 0.3;
                    s.width = r.width * 0.35;
                }
                s.height = s.width;
                v = new PHImageView(s);
                v->setImage(gm->imageNamed(imageName()));
                if (!side)
                    v->setHorizontallyFlipped(true);
            }
            return v;
        }
        void loseHP(float f)
        {
            hp -= f;
            if (hp < 0)
                hp = 0;
        }
        float health() { return hp; }
        void setHealth(float h) { hp = h; }
        virtual float maxHealth() = 0;
};

class PHBattleController : public PHViewController
{
    protected:
        PHMob * player, * mob;
        PHImageView * statusbg;
        PHView * buttons;
        PHTextView * status;
        PHView * mobview, * playerview;
        PHImageView * playerbg, * mobbg;
        PHTextView * playerhp, * mobhp;

    public:
        PHBattleController(PHGameManager * gm, PHMob * p, PHMob * m) : PHViewController(gm), player(p), mob(m)
        {
            player->retain();
            mob->retain();
        }

        ~PHBattleController()
        {
            player->release();
            mob->release();
        }

#define statusr ((191-142) / 191.0)

        PHView * loadView(const PHRect & r)
        {
            PHImageView * v = new PHImageView(r);
            v->setUserInput(true);
            v->setImage(gm->imageNamed("bg"));
            v->setBackgroundColor(PHColor(1,1,1,1));

            float fontSize = 1.0/20.0 * r.height;

            PHRect sr = PHRect(r.width * 0.1, r.height * statusr * 0.1, r.width * 0.8, r.height * statusr * 0.8 );
            status = new PHTextView(sr);
            status->setFont(gm->fontNamed("PowerClear"));
            status->setAlignment(PHTextView::alignCenter | PHTextView::justifyCenter);
            status->setFontSize(fontSize);
            status->setFontColor(PHColor(0, 0, 0, 1));

            buttons = new PHView(sr);
            buttons->setUserInput(true);
            v->addChild(status);

            player->setHealth(player->maxHealth());
            mob->setHealth(mob->maxHealth());
            player->setRect(r);
            mob->setRect(r);

            PHView * vv;
            PHRect rrr;

            mob->setSide(true);
            mob->setGameManager(gm);
            vv = mob->view();
            rrr = vv->frame();
            rrr.x -= r.width;
            vv->setFrame(rrr);
            vv->beginCinematicAnimation(2.0f, PHCinematicAnimator::FadeOutFunction);
            vv->animateMove(PHPoint(r.width, 0));
            vv->commitCinematicAnimation();
            v->addChild(vv);

            player->setSide(false);
            player->setGameManager(gm);
            vv = player->view();
            rrr = vv->frame();
            rrr.x += r.width;
            vv->setFrame(rrr);
            vv->beginCinematicAnimation(2.0f, PHCinematicAnimator::FadeOutFunction);
            vv->animateMove(PHPoint(-r.width, 0));
            vv->commitCinematicAnimation();
            v->addChild(vv);


            PHRect php(0, sr.height * (1.0/0.8), r.width * 0.4, r.height * 0.1);
            playerbg = new PHImageView(php);
            playerbg->setImage(gm->imageNamed("hp"));
            v->addChild(playerbg);
            playerhp = new PHTextView(playerbg->bounds());
            playerhp->setFont(gm->fontNamed("PowerClear"));
            playerhp->setAlignment(PHTextView::alignCenter | PHTextView::justifyCenter);
            playerhp->setFontSize(fontSize);
            playerhp->setFontColor(PHColor(0, 0, 0, 1));
            playerhp->setText("meow");
            playerbg->addChild(playerhp);

            PHRect mhp(r.width - r.width * 0.4, r.height - r.height * 0.1, r.width * 0.4, r.height * 0.1);
            mobbg = new PHImageView(mhp);
            mobbg->setImage(gm->imageNamed("hp"));
            mobbg->setHorizontallyFlipped(true);
            v->addChild(mobbg);
            mobhp = new PHTextView(mobbg->bounds());
            mobhp->setHorizontallyFlipped(true);
            mobhp->setFont(gm->fontNamed("PowerClear"));
            mobhp->setAlignment(PHTextView::alignCenter | PHTextView::justifyCenter);
            mobhp->setFontSize(fontSize);
            mobhp->setFontColor(PHColor(0, 0, 0, 1));
            mobhp->setText("meow");
            mobbg->addChild(mobhp);

            updateHP();


            for (int i = 0; i < 2; i++)
                for (int j = 0; j < 2; j++)
                {
                    int n = i * 2 + j;
                    PHButtonView * b = new PHButtonView(PHRect(sr.width * (i*0.5), sr.height * (j*0.5), sr.width * 0.5, sr.height * 0.5));
                    b->setUpCallback(PHInv(this, PHBattleController::doMove, (void*)(size_t)n));
                    buttons->addChild(b);
                    PHTextView * tv = new PHTextView(b->bounds());
                    tv->setFont(gm->fontNamed("PowerClear"));
                    tv->setAlignment(PHTextView::alignCenter | PHTextView::justifyCenter);
                    tv->setText(player->nameOfMove(n));
                    tv->setFontSize(fontSize);
                    tv->setFontColor(PHColor(0, 0, 0, 1));

                    b->addChild(tv);
                }

            setStatus(string("A wild ") + mob->name() + " appeared", PHInv(this, PHBattleController::backToChoose, NULL));
            return v;
        }

        void setStatus(const string & s, const PHInvocation & inv)
        {
            status->setText(s);
            gm->animatorPool()->scheduleAction(inv, 2.0f, false, false);
        }

        void doMove(PHButtonView * sender, void * ud)
        {
            gm->soundManager()->soundNamed("select")->play();
            buttons->removeFromParent();
            getView()->addChild(status);

            int move = (int)(size_t)ud;
            player->executeMove(move, mob);
            setStatus(player->name() + " used " + player->nameOfMove(move), PHInv(this, PHBattleController::doMoveMob, NULL));
        }

        void doMoveMob(PHButtonView * sender, void * ud)
        {
            if (!updateHP()) return;
            int move = rand() & 3;
            mob->executeMove(move, player);
            setStatus(mob->name() + " used " + mob->nameOfMove(move), PHInv(this, PHBattleController::backToChoose, NULL));
        }

        void backToChoose(PHObject * sender, void * ud)
        {
            if (!updateHP()) return;
            status->removeFromParent();
            getView()->addChild(buttons);
        }

        bool updateHP()
        {
            stringstream ss;
            ss<<"HP: "<<player->health()<<"/"<<player->maxHealth();
            playerhp->setText(ss.str());
            ss.str("");
            ss<<"HP: "<<mob->health()<<"/"<<mob->maxHealth();
            mobhp->setText(ss.str());
            if (player->health() == 0)
            {
                setStatus(player->name() + " fainted", PHInv(this, PHBattleController::endGame, NULL));
                PHView * v = player->view();
                v->setScalingCenter(PHPoint(0, 0));
                v->beginCinematicAnimation(0.5f);
                v->animateScale(PHPoint(1, 0.00001));
                v->commitCinematicAnimation();
                return false;
            }
            if (mob->health() == 0)
            {
                setStatus(mob->name() + " fainted", PHInv(this, PHBattleController::endGame, NULL));
                PHView * v = mob->view();
                v->setScalingCenter(PHPoint(0, 0));
                v->beginCinematicAnimation(0.5f);
                v->animateScale(PHPoint(1, 0.00001));
                v->commitCinematicAnimation();
                return false;
            }
            return true;
        }

        void endGame(PHObject * sender, void * ud)
        {

        }
};

class PHOcti : public PHMob
{
    public:
        string imageName()
        {
            return "octi";
        }
        string nameOfMove(int i) 
        {
            switch(i)
            {
                case 0:
                    return "RUPE PATEU";
                case 1:
                    return "MOISE POWER";
                case 2:
                    return "PANARAMA FRENZY";
                case 3:
                    return "BUCATA";
                default:
                    return "UNKNOWN";
            }
        }

        string name()
        {
            return "Octi";
        }
        float maxHealth() { return 100; }
};

class PHSbircea : public PHMob
{
    public:
        string imageName()
        {
            return "sbircea";
        }

        string nameOfMove(int i) 
        {
            switch(i)
            {
                case 0:
                    return "(HEAD)BUTT";
                case 1:
                    return "MOARTEA BIZONILOR";
                case 2:
                    return "MIC SI FIRAV";
                case 3:
                    return "MUZICUTA";
            }
            return "UNKNOWN";
        }

        string name()
        {
            return "Sbircea";
        }
        float maxHealth() { return 100; }
};

class PHWhiteScreen : public PHViewController
{
    PHMob * p1, * p2;
    public:
        PHWhiteScreen(PHGameManager * gm, PHMob * pp1, PHMob * pp2) : PHViewController(gm), p1(pp1), p2(pp2) {}
        PHView * loadView(const PHRect & r)
        {
            p1->retain();
            p2->retain();
            PHView * v = new PHView(r);
            v->setBackgroundColor(PHColor(1, 1, 1, 1));
            gm->animatorPool()->scheduleAction(PHInv(this, PHWhiteScreen::timerFired, NULL), 1.0f);
            return v;
        }

        void timerFired()
        {
            PHBattleController * vc = new PHBattleController(gm, p1, p2);
            navigationController()->setFadeColor(PHColor(1, 1, 1, 1));
            navigationController()->pushViewController(vc, PHNavigationController::FadeToColor);
        }
};

void PHGameEntryPoint(PHGameManager * gm)
{    
    srand(time(NULL));
    PHOcti * player = new PHOcti();
    PHSbircea * mob = new PHSbircea();
    PHViewController * vc = new PHWhiteScreen(gm, player, mob);
    gm->soundManager()->setBackgroundMusic(gm->soundManager()->soundNamed("pokemon"));
    gm->setUpNavigationController()->pushViewController(vc);
    vc->release();
}

PHMAIN_DECLARE(PHWVideoMode(800, 600, PHWVideoMode::FullscreenWindowed), PHWGLES1 | PHWVSync, &PHGameEntryPoint,NULL);
 

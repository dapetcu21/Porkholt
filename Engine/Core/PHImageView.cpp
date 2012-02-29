/*
 *  PHImageView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHLua.h"
#include "PHImage.h"
#include "PHImageView.h"
#include "PHTrailImageView.h"
#include "PHFileManager.h"
#include "PHNormalImage.h"
#include "PHAnimatedImage.h"
#include "PHImageAnimator.h"
#include "PHAnimatorPool.h"
#include "PHBezierPath.h"
#include "PHParticleView.h"
#include "PHKeyframeAnimator.h"
#include "PHGLUniformStates.h"
#include "PHGLVertexArrayObject.h"
#include "PHGLVertexBufferObject.h"
#include "PHDeferredView.h"

#define PHIMAGEVIEW_INIT _image(NULL), _animator(NULL), coords(PHWholeRect), tint(PHInvalidColor), pool(PHAnimatorPool::currentAnimatorPool()), curve(NULL), VBOneedsRebuilding(true), constrain(true), _repeatX(1), _repeatY(1), lastAnimFrame(-1), animFrame(-1), _normalMapping(true), curveVAO(NULL), curveAttributeVBO(NULL), curveElementVBO(NULL), straightVAO1(NULL), straightVBO1(NULL), straightVAO2(NULL), straightVBO2(NULL)

bool PHImageView::supportsRenderMode(int rm)
{
    return (rm == PHGameManager::defaultRenderMode) || (rm == PHDeferredView::normalMapRenderMode);
}

PHImageView::PHImageView() : PHView(), PHIMAGEVIEW_INIT
{
    luaClass = "PHImageView";
}

PHImageView::PHImageView(const PHRect &frame) : PHView(frame), PHIMAGEVIEW_INIT
{
    luaClass = "PHImageView";
}

PHImageView::PHImageView(PHImage * image) : PHView(), PHIMAGEVIEW_INIT
{
    luaClass = "PHImageView";
	setImage(image);
}

void PHImageView::setAnimatorPool(PHAnimatorPool * p) 
{
    pool = p; 
    if (_animator)
        _animator->setAnimatorPool(p);
}

void PHImageView::setImage(PHImage * img)
{
    if (_animator)
    {
        _animator->release();
        _animator = NULL;
    }
    if (img)
    {
        img->retain();
        if (img->isAnimated())
            _animator = ((PHAnimatedImage*)img)->newAnimator(pool);
    }
    if (_image)
        _image->release();
    _image = img;
    VBOneedsRebuilding = true;
}

void PHImageView::destroyVAOs()
{
    destroyStraightVAO();
    destroyCurvedVAO();
}

void PHImageView::destroyStraightVAO()
{
    if (straightVBO1)
    {
        straightVBO1->release();
        straightVBO1 = NULL;
    }
    if (straightVBO2)
    {
        straightVBO2->release();
        straightVBO2 = NULL;
    }
    if (straightVAO1)
    {
        straightVAO1->release();
        straightVAO1 = NULL;
    }
    if (straightVAO2)
    {
        straightVAO2->release();
        straightVAO2 = NULL;
    }
}

void PHImageView::destroyCurvedVAO()
{
    if (curveVAO)
    {
        curveVAO->release();
        curveVAO = NULL;
    }
    if (curveElementVBO)
    {
        curveElementVBO->release();
        curveElementVBO = NULL;
    }
    if (curveAttributeVBO)
    {
        curveAttributeVBO->release();
        curveAttributeVBO = NULL;
    }
}

PHImageView::~PHImageView()
{
    if (curve)
        curve->release();
    curve = NULL;
    destroyVAOs();
    if (_animator)
        _animator->release();
	if (_image)
		_image->release();
}

void PHImageView::renderInFramePortionTint(const PHRect & fr, const PHRect & crd, const PHColor & clr)
{
    if (!_image) return;
    bool nrm =  (gm->renderMode() == PHDeferredView::normalMapRenderMode);
    PHImage * img = _image;
    if (nrm)
    {
        if (_normalMapping && (img->normalMap()))
            _image = img->normalMap();
        else
            gm->pushSpriteShader(gm->missingNormalSpriteShader());
    }

    if (_image->isNormal())
        ((PHNormalImage*)_image)->renderInFramePortionTint(gm,fr,crd,clr);
    
    if (_image->isAnimated())
        _animator->renderInFramePortionTint(gm,fr,crd,clr);
    
    if (nrm)
    {
        if (_normalMapping && (img->normalMap()))
            _image = img;
        else
            gm->popSpriteShader();
    }
}

void PHImageView::rebuildCurvedVBO()
{
    destroyStraightVAO();
    if (curve)
    {
        if (!curveVAO) 
            curveVAO = new PHGLVertexArrayObject(gm);
        
        PHNormalImage * img = (PHNormalImage*)_image;
        int _width = img->width();
        int _height = img->height();
        int actWidth = img->actualWidth();
        int actHeight = img->actualHeight();
        ph_float sX = ((ph_float)_width/(ph_float)actWidth)*repeatX();
        ph_float sY = ((ph_float)_height/(ph_float)actHeight)*repeatY();
        ph_float sx = 0.5f/actWidth;
        ph_float sy = 0.5f/actHeight;
        PHRect p = textureCoordinates();
        p.width*=sX;
        p.height*=sY;
        p.x*=sX;
        p.y*=sX;
        p.x+=sx;
        p.y+=sy;
        p.width-=sx*2;
        p.height-=sx*2;
        
        curveVAO->bindToEdit();
        size_t nVertices;
        GLfloat * arr = curve->vertexData(nVertices, p);
        if (!curveAttributeVBO)
            curveAttributeVBO = new PHGLVertexBufferObject(gm);
        
        curveAttributeVBO->bindTo(PHGLVBO::arrayBuffer);
        curveAttributeVBO->setData(NULL, nVertices*4*sizeof(GLfloat), PHGLVBO::dynamicDraw);
        curveAttributeVBO->setSubData(arr, 0, nVertices*4*sizeof(GLfloat));
        curveVAO->vertexPointer(PHIMAGEATTRIBUTE_POS, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0, curveAttributeVBO);
        curveVAO->vertexPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 2*sizeof(GLfloat), curveAttributeVBO);
        curveAttributeVBO->unbind();
        
        size_t nIndexes;
        GLushort * indexes = curve->indexData(arr, 4, nVertices, nIndexes);
        delete arr;
        
        if (indexes)
        {
            if (!curveElementVBO)
                curveElementVBO = new PHGLVertexBufferObject(gm);
            curveElementVBO->bindTo(PHGLVBO::elementArrayBuffer);
            curveElementVBO->setData(indexes, nIndexes*sizeof(GLushort), PHGLVBO::dynamicDraw);
            curveVAO->setDrawElements(GL_TRIANGLES, nIndexes, GL_UNSIGNED_SHORT, 0);
            
            delete indexes;
        } else {
            if (curveElementVBO)
            {
                curveElementVBO->release();
                curveElementVBO = NULL;
            }
            curveVAO->setDrawArrays(GL_TRIANGLE_STRIP, 0, nVertices);
        }
        
        curveVAO->unbind();
        
    } else {
        destroyCurvedVAO();
    }
}

void PHImageView::curveCallback(PHCurve *sender, void *ud)
{
    VBOneedsRebuilding = true;
}

void PHImageView::setShape(PHCurve *bp)
{
    if (curve)
    {
        curve->release();
        curve->removeCallback(this);
    }
    curve = NULL;
    if (bp)
    {
        bp->retain();
        bp->addCallback(PHInv(this,PHImageView::curveCallback,NULL));
    }
    curve = bp;
    VBOneedsRebuilding = true;
}

void PHImageView::renderCurved()
{
    if (!_image) return;
    if (image()->isAnimated()) return;
    image()->load();
    loadVBO();
    
    gm->setColor(tint);
    gm->setGLStates(PHGLTexture);
    ((PHNormalImage*)image())->bindToTexture(0);
    gm->applySpriteShader();
    
    if (constrain)
    {
        PHMatrix om = gm->modelViewMatrix();
        gm->setModelViewMatrix(om * PHMatrix::translation(_bounds.x, _bounds.y) * PHMatrix::scaling(_bounds.width,_bounds.height));
        gm->reapplyMatrixUniform();
        curveVAO->draw();
        gm->setModelViewMatrix(om);
    } else     
        curveVAO->draw();
}

void PHImageView::rebuildStraightVBO()
{
    destroyCurvedVAO();
    if (_image)
    {
        if (_image->isNormal())
        {
            ((PHNormalImage*)_image)->rebuildVAO(this,straightVAO1,straightVBO1);
            if (straightVBO2)
            {
                straightVBO2->release();
                straightVBO2 = NULL;
            }
            if (straightVAO2)
            {
                straightVAO2->release();
                straightVAO2 = NULL;
            }
        }
        if (_image->isAnimated())
        {
            _animator->rebuildVAOs(this,straightVAO1,straightVBO1,straightVAO2,straightVBO2);
            lastAnimFrame = _animator->lastRealFrame();
            animFrame = _animator->currentRealFrame();
        }
    } else
        destroyStraightVAO();
}

bool PHImageView::animatorNeedsVBORebuild()
{
    if (!_image || !_animator || !_image->isAnimated())
        return false;
    return ((_animator->lastRealFrame() != lastAnimFrame) || (_animator->currentRealFrame() != animFrame));
}

void PHImageView::renderStraight()
{
    if (!_image) return;
    _image->load();
    if (animatorNeedsVBORebuild())
        VBOneedsRebuilding = true;
    loadVBO();
    if (straightVAO1)
    {
        PHMatrix om = gm->modelViewMatrix();
        gm->setModelViewMatrix(om * PHMatrix::translation(_bounds.x, _bounds.y) * PHMatrix::scaling(_bounds.width,_bounds.height));
        if (_image->isNormal())
        {
            gm->setColor(tint);
            gm->setGLStates(PHGLTexture);
            ((PHNormalImage*)image())->bindToTexture(0);
            gm->applySpriteShader();
            straightVAO1->draw();
        }
        
        if (_image->isAnimated())
        {
            PHColor t = tint.isValid()?tint:PHWhiteColor;
            ph_float rem = straightVAO2?(_animator->remainingFrameTime()/_animator->currentFrameTime()):0;
            
            gm->setGLStates(PHGLTexture);
            
            if (straightVAO2)
            {
                gm->setColor(t*rem);
                _animator->bindLastFrameToTexture(0);
                gm->applySpriteShader();
                straightVAO1->bind();
                straightVAO2->draw();
            }
            
            gm->setColor(t*(1-rem));
            _animator->bindCurrentFrameToTexture(0);
            gm->applySpriteShader();
            straightVAO1->bind();
            straightVAO1->draw();
            
            gm->bindVAO(NULL);
        }
        gm->setModelViewMatrix(om);
    }
}

void PHImageView::draw()
{
    if (!_image) return;
    bool nrm =  (gm->renderMode() == PHDeferredView::normalMapRenderMode);
    PHImage * img = _image;
    if (nrm)
    {
        if (_normalMapping && (img->normalMap()))
            _image = img->normalMap();
        else
            gm->pushSpriteShader(gm->missingNormalSpriteShader());
    }
    if (curve)
        renderCurved();
    else
        renderStraight();
    if (nrm)
    {
        if (_normalMapping && (img->normalMap()))
            _image = img;
        else
            gm->popSpriteShader();
    }
}

void PHImageView::loadFromLua(lua_State *L)
{
    if (lua_istable(L, -1))
    {
        PHRect portion = PHWholeRect;
        PHRect frame = PHWholeRect;
        int tag = 0;
        ph_float rot = 0;
        bool flipHoriz = false;
        bool flipVert = false;
        bool constrain = true;
        ph_float repX=1,repY=1;
        PHColor tint = PHInvalidColor;

        PHLuaGetRectField(frame, "pos");
        PHLuaGetRectField(portion , "texCoord");
        PHLuaGetNumberField(tag, "tag");
        PHLuaGetAngleField(rot, "rotation");
        PHLuaGetBoolField(flipHoriz, "horizontallyFlipped");
        PHLuaGetBoolField(flipVert, "verticallyFlipped");
        PHLuaGetNumberField(repX, "repeatX");
        PHLuaGetNumberField(repY, "repeatY");
        PHLuaGetColorField(tint, "tint");
        PHLuaGetBoolField(constrain, "constrainCurveToFrame");
        
        lua_getfield(L, -1, "bezierPath");
        PHBezierPath * bp = PHBezierPath::fromLua(L);
        lua_pop(L,1);
        
        lua_pushstring(L, "alpha");
        lua_gettable(L, -2);
        if (lua_isnumber(L , -1))
        {
            if (tint == PHInvalidColor)
                tint = PHWhiteColor;
            tint.a *= lua_tonumber(L, -1);
        }
        lua_pop(L,1);
        
        lua_getfield(L, -1, "keyframeAnimations");
        if (lua_istable(L, -1))
        {
            PHLuaForEach(-1)
                PHKeyframeAnimator * anim = PHKeyframeAnimator::fromLua(L);
                if (anim)
                {
                    anim->setAnimatorPool(pool);
                    addCinematicAnimation(anim);
                    anim->playSection("default");
                    anim->release();
                }
                lua_pop(L,1);
            PHLuaForEach_
        }
        lua_pop(L,1);
        
        setTintColor(tint);
        setTextureCoordinates(portion);
        setFrame(frame);
        setRotation(rot);
        setTag(tag);
        setHorizontallyFlipped(flipHoriz);
        setVerticallyFlipped(flipVert);
        setRepeatX(repX);
        setRepeatY(repY);
        setShape(bp);
        setConstrainCurveToFrame(constrain);
        if (bp)
            bp->release();
    }
}

PHImageView * PHImageView::imageFromLua(lua_State * L, PHGameManager * man, const string & root)
{
    return imageFromLua(L, man, root, PHAnimatorPool::currentAnimatorPool());
}

PHImageView * PHImageView::imageFromLua(lua_State * L, PHGameManager * man, const string & root, PHAnimatorPool * pool)
{
    PHImageView * img = NULL;
    if (lua_istable(L, -1))
    {
        bool isV = true;
        string filename = root + "/"; 
        string fn;
        lua_pushstring(L, "filename");
        lua_gettable(L, -2);
        if ((isV = lua_isstring(L, -1)))
            fn = lua_tostring(L, -1);
        lua_pop(L, 1);
        
        if (fn[0]=='/')
        {
            filename = man->resourcePath() + "/img";
        }
        filename = filename + fn;
        
        if (isV)
        {
            string clss;
            PHLuaGetStringField(clss, "class");
            
            img = PHImageView::imageFromClass(clss);
            img->setGameManager(man);
            img->setAnimatorPool(pool);
            img->setImage(man->imageFromPath(filename));
            img->setOptimizations(true);
            img->loadFromLua(L);
        }
    }
    return img;
}

map<string, PHAllocator> * PHImageView::initMap = NULL;

PH_REGISTERIMAGEVIEW(PHImageView)

PHImageView * PHImageView::imageFromClass(const string & clss)
{
    map<string,PHAllocator>::iterator i = initMap->find(clss);
    if (i==initMap->end())
        return new PHImageView;
    return (PHImageView*)(i->second)();
}

#pragma mark -
#pragma mark scripting

static int PHImageView_width(lua_State * L)
{
    PHImageView * v = (PHImageView*)PHLuaThisPointer(L);
    if (!v->image())
        lua_pushnumber(L, 0);
    else
        lua_pushnumber(L, v->image()->width());
    return 1;
}

static int PHImageView_height(lua_State * L)
{
    PHImageView * v = (PHImageView*)PHLuaThisPointer(L);
    if (!v->image())
        lua_pushnumber(L, 0);
    else
        lua_pushnumber(L, v->image()->height());
    return 1;
}

PHLuaRectGetter(PHImageView, textureCoordinates);
PHLuaRectSetter(PHImageView, setTextureCoordinates);
PHLuaBoolGetter(PHImageView, constrainCurveToFrame);
PHLuaBoolSetter(PHImageView, setConstrainCurveToFrame);
PHLuaNumberGetter(PHImageView, repeatX);
PHLuaNumberSetter(PHImageView, setRepeatX);
PHLuaNumberGetter(PHImageView, repeatY);
PHLuaNumberSetter(PHImageView, setRepeatY);

static int PHImageView_tint(lua_State * L)
{
    PHImageView * v = (PHImageView*)PHLuaThisPointer(L);
    v->tintColor().saveToLua(L);
    return 1;
}

static int PHImageView_setTint(lua_State * L)
{
    PHImageView * v = (PHImageView*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    v->setTintColor(PHColor::fromLua(L, 2));
    return 0;
}

static int PHImageView_setImage(lua_State * L)
{
    PHImageView * v = (PHImageView*)PHLuaThisPointer(L);
    luaL_checkstring(L, 2);
    string s = lua_tostring(L, 2);
    PHImage * img;
    if (s.find('/')==s.npos)
        img = v->gameManager()->imageNamed(s);
    else
        img = v->gameManager()->imageFromPath(s);
    v->setImage(img);
    return 0;
}


void PHImageView::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHImageView");
    
    PHLuaAddMethod(PHImageView, width);
    PHLuaAddMethod(PHImageView, height);
    PHLuaAddMethod(PHImageView, setImage);
    PHLuaAddMethod(PHImageView, tint);
    PHLuaAddMethod(PHImageView, setTint);
    PHLuaAddMethod(PHImageView, textureCoordinates);
    PHLuaAddMethod(PHImageView, setTextureCoordinates);
    PHLuaAddMethod(PHImageView, constrainCurveToFrame);
    PHLuaAddMethod(PHImageView, setConstrainCurveToFrame);
    PHLuaAddMethod(PHImageView, repeatX);
    PHLuaAddMethod(PHImageView, setRepeatX);
    PHLuaAddMethod(PHImageView, repeatY);
    PHLuaAddMethod(PHImageView, setRepeatY);

    lua_pop(L,1);
}

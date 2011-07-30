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

#define PHIMAGEVIEW_INIT _image(NULL), _animator(NULL), coords(PHWholeRect), tint(PHInvalidColor), pool(PHAnimatorPool::mainAnimatorPool())

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
}

PHImageView::~PHImageView()
{
    if (_animator)
        _animator->release();
	if (_image)
		_image->release();
}

void PHImageView::renderInFramePortionTint(const PHRect & fr, const PHRect & crd, const PHColor & clr)
{
    if (_image)
    {
        if (_image->isNormal())
            ((PHNormalImage*)_image)->renderInFramePortionTint(fr,crd,clr);
        
        if (_image->isAnimated())
            _animator->renderInFramePortionTint(fr,crd,clr);
    }
}

void PHImageView::draw()
{
    renderInFramePortionTint(_bounds, coords, tint);
}

PHImageView * PHImageView::imageFromLua(lua_State * L,const string & root)
{
    return imageFromLua(L, root, PHAnimatorPool::mainAnimatorPool());
}

PHImageView * PHImageView::imageFromLua(lua_State * L,const string & root, PHAnimatorPool * pool)
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
            filename = PHFileManager::resourcePath() + "/img";
        }
        filename = filename + fn;
        
        if (isV)
        {
            PHRect portion = PHWholeRect;
            PHRect frame = PHWholeRect;
            string clss;
            int tag = 0;
            double rot = 0;
            bool flipHoriz = false;
            bool flipVert = false;
            PHColor tint = PHInvalidColor;
            PHLuaGetStringField(clss, "class");
            PHLuaGetRectField(frame, "pos");
            PHLuaGetRectField(portion , "texCoord");
            PHLuaGetNumberField(tag, "tag");
            PHLuaGetAngleField(rot, "rotation");
            PHLuaGetBoolField(flipHoriz, "horizontallyFlipped");
            PHLuaGetBoolField(flipVert, "verticallyFlipped");
            PHLuaGetColorField(tint, "tint");
            
            lua_pushstring(L, "alpha");
            lua_gettable(L, -2);
            if (lua_isnumber(L , -1))
            {
                if (tint == PHInvalidColor)
                    tint = PHWhiteColor;
                tint.a *= lua_tonumber(L, -1);
            }
            lua_pop(L,1);
            
            img = PHImageView::imageFromClass(clss);
            img->setAnimatorPool(pool);
            img->setImage(PHImage::imageFromPath(filename));
            img->setTintColor(tint);
            img->setTextureCoordinates(portion);
            img->setFrame(frame);
            img->setRotation(rot);
            img->setOptimizations(true);
            img->setTag(tag);
            img->setHorizontallyFlipped(flipHoriz);
            img->setVerticallyFlipped(flipVert);
        }
    }
    return img;
}

PHImageView * PHImageView::imageFromClass(const string & clss)
{
    if (clss=="PHTrailImageView")
        return new PHTrailImageView();
    return new PHImageView();
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
        img = PHImage::imageNamed(s);
    else
        img = PHImage::imageFromPath(s);
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

    lua_pop(L,1);
}

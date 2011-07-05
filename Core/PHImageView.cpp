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

#define PHIMAGEVIEW_INIT _image(NULL), coords(PHWholeRect), tint(PHInvalidColor), flipVert(false), flipHoriz(false)

PHImageView::PHImageView() : PHView(), PHIMAGEVIEW_INIT
{
}

PHImageView::PHImageView(const PHRect &frame) : PHView(frame), PHIMAGEVIEW_INIT
{
}

PHImageView::PHImageView(PHImage * image) : PHView(), PHIMAGEVIEW_INIT
{
	setImage(image);
}

PHImageView::~PHImageView()
{
	if (_image)
		_image->release();
}

void PHImageView::draw()
{
	if (_image)
    {
        PHRect frame = _bounds;
        if (flipHoriz)
        {
            frame.x+=frame.width;
            frame.width = -frame.width;
        }
        if (flipVert)
        {
            frame.y+=frame.height;
            frame.height = -frame.height;
        }
		_image->renderInFramePortionTint(frame,coords,tint);
    }
}

void PHImageView::setImage(PHImage * image) 
{ 
	if (image) image->retain(); 
	if (_image) _image->release(); 
	_image=image; 
}

PHImageView * PHImageView::imageFromLua(lua_State * L,const string & root)
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
            filename = PHFileManager::singleton()->resourcePath() + "/img";
        }
        filename = filename + fn;
        
        if (isV)
        {
            PHRect portion = PHWholeRect;
            PHRect frame = PHWholeRect;
            string clss;
            lua_pushstring(L, "class");
            lua_gettable(L, -2);
            if (lua_isstring(L, -1))
                clss = lua_tostring(L, -1);
            lua_pop(L, 1);
            
            lua_pushstring(L, "pos");
            lua_gettable(L, -2);
            if (lua_istable(L, -1))
                frame = PHRect::rectFromLua(L,-1);
            lua_pop(L, 1);
            
            lua_pushstring(L, "texCoord");
            lua_gettable(L, -2);
            if (lua_istable(L, -1))
                portion = PHRect::rectFromLua(L,-1);
            lua_pop(L, 1);
            
            int tag = 0;
            lua_pushstring(L, "tag");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                tag = lua_tonumber(L, -1);
            lua_pop(L, 1);

            int rot = 0;
            lua_pushstring(L, "rotation");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                rot = lua_tonumber(L, -1);
            lua_pop(L, 1);

            bool flipHoriz = false;
            lua_pushstring(L, "horizontallyFlipped");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                flipHoriz = lua_tonumber(L, -1);
            lua_pop(L, 1);
            
            bool flipVert = false;
            lua_pushstring(L, "verticallyFlipped");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                flipVert = lua_tonumber(L, -1);
            lua_pop(L, 1);
            
            lua_pushstring(L, "tint");
            lua_gettable(L, -2);
            PHColor tint = PHColor::colorFromLua(L,-1);
            lua_pop(L, 1);
            
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
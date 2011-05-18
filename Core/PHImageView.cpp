/*
 *  PHImageView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHLua.h"

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

PHImageView * PHImageView::imageFromLua(void * l,const string & root)
{
    lua_State * L = (lua_State*)l;
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
            
            lua_pushstring(L, "posX");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                frame.x = lua_tonumber(L, -1);
            lua_pop(L, 1);
            
            lua_pushstring(L, "posY");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                frame.y = lua_tonumber(L, -1);
            lua_pop(L, 1);
            
            lua_pushstring(L, "imgW");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                frame.width = lua_tonumber(L, -1);
            lua_pop(L, 1);
            
            lua_pushstring(L, "imgH");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                frame.height = lua_tonumber(L, -1);
            lua_pop(L, 1);
            
            lua_pushstring(L, "texX");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                portion.x = lua_tonumber(L, -1);
            lua_pop(L, 1);
            
            lua_pushstring(L, "texY");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                portion.y = lua_tonumber(L, -1);
            lua_pop(L, 1);
            
            lua_pushstring(L, "texW");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                portion.width = lua_tonumber(L, -1);
            lua_pop(L, 1);
            
            lua_pushstring(L, "texH");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                portion.height = lua_tonumber(L, -1);
            lua_pop(L, 1);
            
            int tag = 0;
            lua_pushstring(L, "tag");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                tag = lua_tonumber(L, -1);
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
            PHColor tint = PHColor::colorFromLua(L);
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
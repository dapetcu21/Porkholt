/*
 *  PHImageView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHImageView.h"
#include "PHLua.h"

#define PHIMAGEVIEW_INIT _image(NULL), coords(PHWholeRect), tint(PHInvalidColor)

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
		_image->renderInFramePortionTint(_bounds,coords,tint);
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
        lua_pushstring(L, "filename");
        lua_gettable(L, -2);
        if ((isV = lua_isstring(L, -1)))
            filename = filename + lua_tostring(L, -1);
        lua_pop(L, 1);
        
        if (isV)
        {
            PHRect portion = PHWholeRect;
            PHRect frame = PHWholeRect;
            
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
            
            img = new PHImageView();
            img->setImage(PHImage::imageFromPath(filename));
            img->setTintColor(tint);
            img->setTextureCoordinates(portion);
            img->setFrame(frame);
            img->setOptimizations(true);
        }
    }
    return img;
}
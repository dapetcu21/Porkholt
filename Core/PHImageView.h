/*
 *  PHImageView.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHIMAGEVIEW_H
#define PHIMAGEVIEW_H

#include "PHView.h"
#include "PHImage.h"

class PHImage;
class PHImageAnimator;
class PHAnimatorPool;

class PHImageView : public PHView
{
protected:
	PHImage * _image;
    PHImageAnimator * _animator;
	PHRect coords;
    PHColor tint;
    
    void renderInFramePortionTint(const PHRect & fr, const PHRect & coords, const PHColor & clr);
    
    PHAnimatorPool * pool;
    
public:
    PHAnimatorPool * animatorPool() { return pool; }
    void setAnimatorPool(PHAnimatorPool * p);
	PHImage * image() { return _image; };
    PHImageAnimator * animator() { return _animator; }
	void setImage(PHImage * image);
	PHImageView();
	PHImageView(const PHRect &frame);
	PHImageView(PHImage * image);
	const PHRect & textureCoordinates() { return coords; };
	void setTextureCoordinates(const PHRect & r) { coords = r; };
	virtual ~PHImageView();
	virtual void draw();
    virtual void render() { PHView::render();};
    
    PHColor & tintColor() { return tint; };
    void setTintColor(const PHColor & clr) { tint = clr; };
    
    virtual const PHColor & animatedColor() { return tint; }
    virtual void setAnimatedColor(const PHColor & c) { setTintColor(c); }
    
    static PHImageView * imageFromLua(lua_State * L,const string & rootPath);
    static PHImageView * imageFromLua(lua_State * L,const string & root, PHAnimatorPool * pool);
    static PHImageView * imageFromClass(const string & clss);
    
    static void registerLuaInterface(lua_State * L);
};

#endif
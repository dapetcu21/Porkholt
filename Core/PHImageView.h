/*
 *  PHImageView.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef INSIDE_PHVIEW_H
#ifndef PHIMAGEVIEW_H
#define PHIMAGEVIEW_H
#define INSIDE_PHIMAGEVIEW_H
#include "PHMain.h"

class PHImage;

class PHImageView : public PHView
{
protected:
	PHImage * _image;
	PHRect coords;
    PHColor tint;
public:
	PHImage * image() { return _image; };
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
    
    static PHImageView * imageFromLua(void * L,const string & rootPath);
    static PHImageView * imageFromClass(const string & clss);
};
#undef INSIDE_PHIMAGEVIEW_H
#endif
#endif
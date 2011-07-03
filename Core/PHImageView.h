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
class PHImageView : public PHView
{
protected:
	PHImage * _image;
	PHRect coords;
    PHColor tint;
    bool flipHoriz,flipVert;
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
    bool horizontallyFlipped() { return flipHoriz; }
    bool verticallyFlipped() { return flipVert; }
    void setHorizontallyFlipped(bool b) { flipHoriz = b; }
    void setVerticallyFlipped(bool b) { flipVert = b; }
    
    static PHImageView * imageFromLua(lua_State * L,const string & rootPath);
    static PHImageView * imageFromClass(const string & clss);
};

#endif
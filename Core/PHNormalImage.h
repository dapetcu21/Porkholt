//
//  PHNormalImage.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/9/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHNORMALIMAGE_H
#define PHNORMALIMAGE_H

#include "PHImage.h"

class PHNormalImage : public PHImage
{
private:
    int actHeight;
	int actWidth;
	bool antialiasing;
	uint8_t bit_depth;
	uint8_t color_type;
	GLint format;
    
    PHThread * thread;
    uint8_t * buffer;
    FILE * fp;
    
    unsigned int texid;
public:
    
    PHNormalImage(const string & path);
    virtual ~PHNormalImage();
    
    void bindToTexture();
    
    void renderInFrame(const PHRect & frm) { renderInFramePortionTint(frm,PHWholeRect,PHInvalidColor); }
	void renderInFramePortion(const PHRect & frm,const PHRect & cnstr) { renderInFramePortionTint(frm, cnstr,PHInvalidColor); }
    void renderInFramePortionTint(const PHRect & frm,const PHRect & cnstr,const PHColor & tint);
	
    void loadFromFile(PHObject * sender, void * ud);
	void loadToTexture(PHObject * sender, void * ud); //don't use
    
    virtual bool isNormal() { return true; }
};

#endif
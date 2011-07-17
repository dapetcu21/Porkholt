//
//  PHTextView.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHTEXTVIEW_H
#define PHTEXTVIEW_H

#include "PHView.h"
#include "PHFont.h"

class PHTextView : public PHView
{
protected:
    PHFont * _font;
    double size;
    double lineSpace;
    int _alignment;
    string _text;
    PHColor color;
public:
	PHTextView();
	PHTextView(const PHRect &frame);
    ~PHTextView();
    
    enum Alignments
    {
        alignTop = 1,
        alignBottom = 2,
        alignCenter = 3,
        justifyLeft = 1<<2,
        justifyRight = 2<<2,
        justifyCenter = 3<<2
    };
    
	PHFont * font() { return _font; }
    void setFont(PHFont * font) { if (font) font->retain(); if (_font) _font->release(); _font = font; }
    double fontSize() { return size; }
    void setFontSize(double s) { size = s; }
    double lineSpacing() { return lineSpace; }
    void setLineSpacing(double s) {  lineSpace = s; }
    int alignment() { return _alignment; }
    void setAlignment(int a) { _alignment = a; }
    void setFontColor(const PHColor & c) { color = c; }
    PHColor fontColor() { return color; }
    
    void setText(const string & s) { _text = s; }
    const string & text() { return _text; };
protected:
	virtual void draw();
};

#endif
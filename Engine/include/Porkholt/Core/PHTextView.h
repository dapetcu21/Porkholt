/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHTEXTVIEW_H
#define PHTEXTVIEW_H

#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHFont.h>

class PHGLVertexArrayObject;
class PHGLVertexBufferObject;
class PHTextView : public PHView
{
protected:
    PHFont * _font;
    ph_float size;
    ph_float lineSpace;
    int _alignment;
    string _text;
    PHColor color;
    
    bool needsReload;
    
    PHGLVertexArrayObject * vao;
    GLfloat * vbuffer;
    GLushort * indices;
    
    PHGLVertexBufferObject * indicesVBO;
    PHGLVertexBufferObject * arraysVBO;
    
    unsigned int nGlyphs;
    
    
    void recalculatePositions();
    void loadVBOs();
    ph_float lengthForInterval(int st, int en);
    
    PHSize sz;
    
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
    
    void setBounds(const PHRect & b);
	PHFont * font() { return _font; }
    void setFont(PHFont * font) { if (font) font->retain(); if (_font) _font->release(); _font = font; needsReload = true;}
    ph_float fontSize() { return size; }
    void setFontSize(ph_float s) { needsReload = true; size = s; }
    ph_float lineSpacing() { return lineSpace; }
    void setLineSpacing(ph_float s) {  needsReload = true; lineSpace = s; }
    int alignment() { return _alignment; }
    void setAlignment(int a) { needsReload = true; _alignment = a; }
    void setFontColor(const PHColor & c) { color = c; }
    PHColor fontColor() { return color; }
    PHSize textSize() { if (needsReload) recalculatePositions(); return sz; }
    
    virtual PHColor cinematicCustomColor() { return color; }
    virtual void setCinematicCustomColor(const PHColor & c) { setFontColor(c); }
    
    void setText(const string & s) { needsReload = true; _text = s; }
    const string & text() { return _text; };
    
    void adjustFontSizeToFit(int precision);
protected:
	virtual void draw();
};

#endif

//
//  PHTextView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHTextView.h"

#define PHTEXTVIEW_INIT _font(NULL), size(1.0f), _alignment(alignCenter | justifyLeft), _text(""), color(PHWhiteColor), needsReload(true), nGlyphs(0), indicesVBO(0), arraysVBO(0), lineSpace(0.5)

PHTextView::PHTextView() : PHView(), PHTEXTVIEW_INIT
{
};

PHTextView::PHTextView(const PHRect &frame) : PHView(frame), PHTEXTVIEW_INIT 
{
};

PHTextView::~PHTextView()
{
    if (indicesVBO)
        glDeleteBuffers(1, &indicesVBO);
    if (arraysVBO)
        glDeleteBuffers(1, &arraysVBO);
    if (vbuffer)
        delete [] vbuffer;
    if (indices)
        delete [] indices;
    if (_font)
        _font->release();
}

void PHTextView::loadVBOs()
{
    if (!indices || !vbuffer) return;
    if (!arraysVBO)
        glGenBuffers(1, &arraysVBO);
    if (!indicesVBO)
        glGenBuffers(1, &indicesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, arraysVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*nGlyphs*4*4, vbuffer, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*nGlyphs*6, indices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    delete [] vbuffer;
    delete [] indices;
    vbuffer = NULL;
    indices = NULL;
}

void PHTextView::draw()
{
    if (!_font) return;
    if (needsReload)
        recalculatePositions();
    loadVBOs();
    
    glBindBuffer(GL_ARRAY_BUFFER, arraysVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
    
    PHGLSetStates(PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture);
    glBindTexture(GL_TEXTURE_2D, _font->textureID());
	glVertexPointer(  2, GL_FLOAT, sizeof(GLfloat)*4, (GLfloat*)NULL);
    glTexCoordPointer(2, GL_FLOAT, sizeof(GLfloat)*4, ((GLfloat*)NULL)+2);
    PHGLSetColor(color);
    
    glDrawElements(GL_TRIANGLES, 6*nGlyphs, GL_UNSIGNED_SHORT, NULL);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

inline bool PHIsBreakCharacter(char c)
{
    return c==' ' || c=='\t' || c=='\n' ;
}

inline bool PHIsMandatoryBreakCharacter(char c)
{
    return c=='\n';
}

double PHTextView::lengthForInterval(int st, int en)
{
    double len = 0;
    for (int i=st; i<=en; i++)
    {
        const PHFont::glyph & glyph = _font->glyphForCharacter(_text[i]);
        len+=size*glyph.aspectRatio;
    }
    return len;
}

struct PHTextViewLine
{
    int first,second;
    double length;
};

static PHTextViewLine makeLine(int f, int s, double l)
{
    PHTextViewLine tmp;
    tmp.first = f;
    tmp.second = s;
    tmp.length = l;
    return tmp;
}

void PHTextView::recalculatePositions()
{
    if (!_font) return;
    needsReload = false;
    
    vector< pair<int,int> > words;
    vector<PHTextViewLine> lines;
    double width = bounds().width;
    int p = 0;
    int st = 0;
    int n = (int)_text.size();
    while (p<=n)
    {
        bool isBreak = p>=n || PHIsBreakCharacter(_text[p]);
        if (isBreak)
        {
            if (p==st)
            {
                st++; p++;
                continue;
            }
            words.push_back(make_pair<int,int>(st,p-1));
            st = p+1;
        }
        p++;
    }
    if (words.empty()) return;
    
    /*PHLog("words:");
    for (int i =0; i<words.size(); i++)
        PHLog("%d %d \"%s\"",words[i].first,words[i].second,_text.substr(words[i].first,words[i].second-words[i].first+1).c_str());*/
    
    
    st = words[0].first;
    int last = st;
    int stW = 0;
    double lineLen = 0;
    n = (int)words.size();
    p = 0;
    
    int totalGlyphs = 0;
    
    while (p<n)
    {
        int wStart = words[p].first;
        bool mandatoryBreak = false;
        for (int i=last; i<wStart; i++)
            if (PHIsMandatoryBreakCharacter(_text[i]))
            {
                mandatoryBreak = true;
                break;
            }
        double wordLen = lengthForInterval(last,words[p].second);
        if (mandatoryBreak || ((lineLen+wordLen)>width && p!=stW))
        {
            lines.push_back(makeLine(st,last-1,lineLen));
            totalGlyphs+=last-st;
            wordLen-= lengthForInterval(last, wStart-1);
            stW = p;
            st = wStart;
            last = st;
            lineLen = 0;
        }
        lineLen+=wordLen;
        last = words[p].second + 1;
        p++;
    }
    lines.push_back(makeLine(st,last-1,lineLen));
    totalGlyphs+=last-st;
    
    /*PHLog("lines:");
    for (int i =0; i<lines.size(); i++)
        PHLog("%d %d %f \"%s\"",lines[i].first,lines[i].second,lines[i].length,_text.substr(lines[i].first,lines[i].second-lines[i].first+1).c_str());*/
    
    n = (int)(lines.size());
    double blockHeight = n*size + (n>=1?(n-1)*lineSpace*size:0);
    double startY;
    if ((_alignment & 3) == alignBottom)
        startY = blockHeight-size;
    else
    if ((_alignment & 3) == alignTop)
        startY = bounds().height-size;
    else
        startY = bounds().height/2+blockHeight/2-size;
    
    if (vbuffer)
        delete [] vbuffer;
    if (indices)
        delete [] indices;
    
    nGlyphs = totalGlyphs;
    vbuffer = new GLfloat[nGlyphs*4*4];
    indices = new GLushort[nGlyphs*6];
    
    GLfloat *txc = vbuffer+2;
    GLfloat *v = vbuffer;
    GLushort *idx = indices;
    
    GLushort index = 0;
    
    double maxLen = 0;
    
    for (int i=0; i<n; i++)
    {
        if (lines[i].length>maxLen)
            maxLen = lines[i].length;
        double startX = 0;
        if ((_alignment & 12) == justifyRight)
            startX = width-lines[i].length;
        if ((_alignment & 12) == justifyCenter)
            startX = width/2-lines[i].length/2;
        int st = lines[i].first;
        int en = lines[i].second;
        for (int j=st; j<=en; j++)
        {
            const PHFont::glyph & glyph = _font->glyphForCharacter(_text[j]);
            txc[0] = glyph.minX; txc[1] = glyph.minY; txc+=4;
            txc[0] = glyph.maxX; txc[1] = glyph.minY; txc+=4;
            txc[0] = glyph.minX; txc[1] = glyph.maxY; txc+=4;
            txc[0] = glyph.maxX; txc[1] = glyph.maxY; txc+=4;
            v[0] = startX; v[1] = startY; v+=4;
            v[0] = startX+size*glyph.aspectRatio; v[1] = startY; v+=4;
            v[0] = startX; v[1] = startY+size; v+=4;
            v[0] = startX+size*glyph.aspectRatio; v[1] = startY+size; v+=4;
            startX+=size*glyph.aspectRatio;
            idx[0] = index+0;
            idx[1] = index+1;
            idx[2] = index+2;
            idx[3] = index+2;
            idx[4] = index+1;
            idx[5] = index+3;
            idx+=6;
            index+=4;
        }
        startY-=(1+lineSpace)*size;
    }
    
    sz.x = maxLen;
    sz.y = blockHeight;
    
}

void PHTextView::adjustFontSizeToFit(int precision)
{
    double st = 0;
    double en = fontSize();
    for (int i=0; i<precision; i++)
    {
        double middle = (st+en)/2;
        setFontSize(middle);
        PHSize size = textSize();
        if (size.width>_frame.width || size.height>_frame.height)
        {
            en = middle;
        } else 
            st = middle;
    }
    setFontSize(en);
}
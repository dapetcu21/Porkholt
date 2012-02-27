//
//  PHTextView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHTextView.h"
#include "PHGLVertexBufferObject.h"
#include "PHGLVertexArrayObject.h"

#define PHTEXTVIEW_INIT _font(NULL), size(1.0f), _alignment(alignCenter | justifyLeft), _text(""), color(PHWhiteColor), needsReload(true), nGlyphs(0), indicesVBO(NULL), arraysVBO(NULL), lineSpace(0.5), vbuffer(NULL), indices(NULL), vao(NULL)

PHTextView::PHTextView() : PHView(), PHTEXTVIEW_INIT
{
};

PHTextView::PHTextView(const PHRect &frame) : PHView(frame), PHTEXTVIEW_INIT 
{
};

PHTextView::~PHTextView()
{
    if (indicesVBO)
        indicesVBO->release();
    if (arraysVBO)
        arraysVBO->release();
    if (vao)
        vao->release();
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
    if (!vao)
        vao = new PHGLVertexArrayObject(_gameManager);
    if (!arraysVBO)
        arraysVBO = new PHGLVertexBufferObject(_gameManager);
    if (!indicesVBO)
        indicesVBO = new PHGLVertexBufferObject(_gameManager);

    vao->bindToEdit();
    arraysVBO->bindTo(PHGLVBO::arrayBuffer);
    arraysVBO->setData(vbuffer, sizeof(GLfloat)*nGlyphs*4*4, PHGLVBO::dynamicDraw);
    indicesVBO->bindTo(PHGLVBO::elementArrayBuffer);
    indicesVBO->setData(indices, sizeof(GLushort)*nGlyphs*6, PHGLVBO::dynamicDraw);
    vao->vertexPointer(PHIMAGEATTRIBUTE_POS, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, 0, arraysVBO);
    vao->vertexPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, sizeof(GLfloat)*2, arraysVBO);
    arraysVBO->unbind();
    vao->setDrawElements(GL_TRIANGLES, 6*nGlyphs, GL_UNSIGNED_SHORT, 0);
    vao->unbind();
    
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
    
    PHGLSetStates(PHGLTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _font->textureID());
    PHGLSetColor(color);
    _gameManager->applyShader(_gameManager->textShader());
    vao->draw();
}

inline bool PHIsBreakCharacter(char c)
{
    return c==' ' || c=='\t' || c=='\n' ;
}

inline bool PHIsMandatoryBreakCharacter(char c)
{
    return c=='\n';
}

ph_float PHTextView::lengthForInterval(int st, int en)
{
    ph_float len = 0;
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
    ph_float length;
};

static PHTextViewLine makeLine(int f, int s, ph_float l)
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
    ph_float width = bounds().width;
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
    ph_float lineLen = 0;
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
        ph_float wordLen = lengthForInterval(last,words[p].second);
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
    ph_float blockHeight = n*size + (n>=1?(n-1)*lineSpace*size:0);
    ph_float startY;
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
    
    ph_float maxLen = 0;
    
    for (int i=0; i<n; i++)
    {
        if (lines[i].length>maxLen)
            maxLen = lines[i].length;
        ph_float startX = 0;
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
    ph_float st = 0;
    ph_float en = fontSize();
    for (int i=0; i<precision; i++)
    {
        ph_float middle = (st+en)/2;
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
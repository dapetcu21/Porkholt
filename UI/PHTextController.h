//
//  PHTextController.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHTEXTCONTROLLER_H
#define PHTEXTCONTROLLER_H

#include "PHViewController.h"

class PHTextView;
class PHLevelController;
class PHTextController : public PHViewController
{
protected:
	virtual PHView * loadView(const PHRect & frame);
    PHTextView * textView;
    PHView * blackoutView;
    PHColor bColor;
    PHColor fColor;
    void advance();
    friend class PHTextControllerView;
    vector<string> * strings;
    bool canAdvance;
    int pos;
    
    PHCallback cb;
    PHObject * trg;
    void * ud;
    
    void viewDidAppear();
    virtual PHColor fadeToColor() { return bColor; }
    
    void beginBlackout();
    void middleBlackout();
    void endBlackout();
    
public:
    PHTextController(vector<string> * strings);
    
    PHColor backgroundColor() { return bColor; }
    PHColor foregroundColor() { return fColor; }
    void setBackgroundColor(const PHColor & c);
    void setForegroundColor(const PHColor & c);
    
    void setDoneCallback(PHObject * target, PHCallback callback, void * userdata) { cb = callback; trg = target; ud = userdata; }
};

#endif
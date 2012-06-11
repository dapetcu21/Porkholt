/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHTEXTCONTROLLER_H
#define PHTEXTCONTROLLER_H

#include <Porkholt/Core/PHViewController.h>

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
    
    PHInvocation invocation;
    
    void viewDidAppear();
    virtual PHColor fadeToColor() { return bColor; }
    
    void beginBlackout();
    void middleBlackout();
    void endBlackout();
    
public:
    PHTextController(vector<string> * strings);
    ~PHTextController();
    
    PHColor backgroundColor() { return bColor; }
    PHColor foregroundColor() { return fColor; }
    void setBackgroundColor(const PHColor & c);
    void setForegroundColor(const PHColor & c);
    
    void setDoneCallback(const PHInvocation & inv) { invocation = inv; }
};

#endif

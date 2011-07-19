//
//  PHFDocument.h
//  PHFontCreator
//
//  Created by Marius Petcu on 7/18/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include <vector>
using namespace std;

struct character
{
    char c;
    double minX,maxX,minY,maxY,aspectRatio;
};

@class ImageView;

@interface PHFDocument : NSDocument {
    CGContextRef image, imageHD;
    NSGraphicsContext * context, * contextHD;
    vector<character> characters;
    
    IBOutlet NSTextField * textField;
    IBOutlet ImageView * imageView;
    IBOutlet NSPopUpButton * fontList;
    IBOutlet NSButton * hdButton;
    IBOutlet NSTextField * fontSize;
    
    BOOL loadedFromFile;
    NSString * loadedFontName;
    double loadedFontSize;
    NSMutableString * loadedDescriptorString;
}

-(IBAction)rebuildData:(id)sender;
-(IBAction)changeHD:(id)sender;

@end

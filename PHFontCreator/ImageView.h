//
//  ImageView.h
//  PHFontCreator
//
//  Created by Marius Petcu on 7/19/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface ImageView : NSView {
@private
    CGContextRef bitmapContext;
}

@property(nonatomic) IBOutlet CGContextRef bitmapContext;

@end

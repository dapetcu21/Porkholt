//
//  SolidColorView.h
//  Level Designer
//
//  Created by Marius Petcu on 6/4/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface SolidColorView : NSView {
@private
    NSColor * color;
}

@property(nonatomic,retain) NSColor * color;

@end

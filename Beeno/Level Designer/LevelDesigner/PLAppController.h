//
//  PHAppController.h
//  Level Designer
//
//  Created by Marius Petcu on 4/5/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface PLAppController : NSObject<NSApplicationDelegate> {
    NSString * resourcePath;
    IBOutlet NSView * resourcePathAccesoryView;
    BOOL loadingResourcePath;
}

-(IBAction)resourcePathDialog:(id)sender;
-(BOOL)isResourcePathValid:(NSString*)path;
-(void)loadResourcePath;


@property(nonatomic,retain) NSString * resourcePath;

@end

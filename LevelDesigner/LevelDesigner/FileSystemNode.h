//
//  FileSystemNode.h
//  Level Designer
//
//  Created by Marius Petcu on 4/6/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface FileSystemNode : NSObject {
	NSImage * icon;
	NSString * title;
	NSURL * url;
	BOOL antiAliasing;
}

@property(nonatomic,retain) NSImage * icon;
@property(nonatomic,retain) NSString * title;
@property(nonatomic,retain) NSURL * url;
@property(nonatomic,assign)	BOOL antiAliasing;

-(void)toggleAntialiasing:(id)sender;

@end

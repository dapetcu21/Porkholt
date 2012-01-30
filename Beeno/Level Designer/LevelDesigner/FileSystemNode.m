//
//  FileSystemNode.m
//  Level Designer
//
//  Created by Marius Petcu on 4/6/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "FileSystemNode.h"


@implementation FileSystemNode
@synthesize title;
@synthesize icon;
@synthesize url;
@synthesize antiAliasing;

-(id)init
{
	if (self = [super init])
	{
	}
	return self;
}

-(void)dealloc
{
	[title release];
	[icon release];
	[url release];
	[super dealloc];
}

- (NSComparisonResult)compare:(FileSystemNode*)aNode
{
	return [[[self title] lowercaseString] compare:[[aNode title] lowercaseString]];
}

- (void)toggleAntialiasing:(id)sender
{
	antiAliasing=!antiAliasing;
	NSFileManager * man = [NSFileManager defaultManager];
	NSString * path = [[url URLByAppendingPathExtension:@"aa"] path];
	BOOL fileExists = [man fileExistsAtPath:path];
	if (!antiAliasing&&fileExists)
		[man removeItemAtPath:path error:nil];
	if (antiAliasing&&!fileExists)
		[man createFileAtPath:path contents:[NSData data] attributes:nil];
}
@end

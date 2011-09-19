//
//  PLDocumentController.m
//  Level Designer
//
//  Created by Marius Petcu on 6/7/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "PLDocumentController.h"


@implementation PLDocumentController

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (NSInteger)runModalOpenPanel:(NSOpenPanel *)openPanel forTypes:(NSArray *)extensions
{
    [openPanel setCanChooseFiles:NO];
    [openPanel setCanChooseDirectories:YES];
    return [super runModalOpenPanel:openPanel forTypes:[NSArray array]];
}

- (NSString *)typeForContentsOfURL:(NSURL *)inAbsoluteURL error:(NSError **)outError
{
    NSString * path = [inAbsoluteURL path];
    NSFileManager * man = [NSFileManager defaultManager];
    BOOL dir = NO;
    BOOL isValid = [man fileExistsAtPath:path isDirectory:&dir] && dir && [man fileExistsAtPath:[path stringByAppendingPathComponent:@"init.lua"] isDirectory:&dir] && !dir;
    if (isValid)
        return @"PHLevel";
    else
        return [super typeForContentsOfURL:inAbsoluteURL error:outError];
}

- (void)dealloc
{
    [super dealloc];
}

@end

//
//  PHDocumentController.m
//  Level Designer
//
//  Created by Marius Petcu on 6/7/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "PHDocumentController.h"


@implementation PHDocumentController

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

- (void)dealloc
{
    [super dealloc];
}

@end

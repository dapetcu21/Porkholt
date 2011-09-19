//
//  BrowserTreeNode.m
//  Level Designer
//
//  Created by Marius Petcu on 4/6/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "BrowserTreeNode.h"


@implementation BrowserTreeNode

-(NSComparisonResult)compare:(BrowserTreeNode*)othr
{
	return [self.representedObject compare:othr.representedObject];
}

@end

//
//  InterfaceController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/19/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MultipleListController.h"

@interface InterfaceController : MultipleListController
{
}

@property(nonatomic,retain) IBOutlet PLTableView * objectView;
@property(nonatomic,retain) IBOutlet PLTableView * jointView;

@end

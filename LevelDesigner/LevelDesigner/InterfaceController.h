//
//  InterfaceController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/19/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MultipleListController.h"
#import "PLPorkholtViewDelegate.h"

@class PLEntity;
@class PropertyController;
@class JointController;
@interface InterfaceController : MultipleListController<PLPorkholtViewDelegate>
{
    IBOutlet NSView * jointDetailView;    
    IBOutlet NSView * objectDetailView;
    IBOutlet NSView * detailView;
    PLEntity * currentEntity;
    IBOutlet PropertyController * propertyController;
    IBOutlet MultipleListController * subentitiesController;
    IBOutlet JointController * jointController;
}

@property(nonatomic,retain) IBOutlet PLTableView * objectView;
@property(nonatomic,retain) IBOutlet PLTableView * jointView;

@end

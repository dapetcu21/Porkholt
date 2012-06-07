/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>
#import "MultipleListController.h"
#import "PLPorkholtViewDelegate.h"
#import "PLMatching.h"

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
    
    PLEntity<PLMatching> * matchObject;
}

@property(nonatomic,retain) IBOutlet PLTableView * objectView;
@property(nonatomic,retain) IBOutlet PLTableView * jointView;
@property(nonatomic,retain) IBOutlet PLTableView * curveView;

@end

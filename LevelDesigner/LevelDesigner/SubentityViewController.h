//
//  SubentityViewController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "MultipleListController.h"

@class PLTableView;
@interface SubentityViewController : MultipleListController
@property(nonatomic,retain) IBOutlet PLTableView * imagesView;
@property(nonatomic,retain) IBOutlet PLTableView * fixturesView;
@end

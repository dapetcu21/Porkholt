//
//  PLObjectSelector.h
//  LevelDesigner
//
//  Created by Marius Petcu on 11/2/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//


@class ObjectController;
@class PLObject;
@interface PLObjectSelector : NSPopUpButton
{
    ObjectController * model;
}
@property(nonatomic,assign) IBOutlet ObjectController * model;
@property(nonatomic,assign) PLObject * selectedObject;
@end

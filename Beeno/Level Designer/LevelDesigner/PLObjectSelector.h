/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */


@class ObjectController;
@class PLObject;
@interface PLObjectSelector : NSPopUpButton
{
    ObjectController * model;
}
@property(nonatomic,assign) IBOutlet ObjectController * model;
@property(nonatomic,assign) PLObject * selectedObject;
@end

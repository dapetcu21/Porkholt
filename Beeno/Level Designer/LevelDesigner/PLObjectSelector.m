/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "PLObjectSelector.h"
#import "ObjectController.h"
#import "PLObject.h"

@implementation PLObjectSelector

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

-(ObjectController*)model 
{
    return model;
}

-(PLObject*)selectedObject
{
    int tg = (int)[[self selectedItem] tag];
    if (tg>=[[model objects] count]-[model numberOfReadOnlyEntitiesInArray:0] || tg<0)
        return NULL;
    tg+=[model numberOfReadOnlyEntitiesInArray:0];
    return (PLObject*)[model entityForIndex:tg inArray:0];
}

-(void)setSelectedObject:(PLObject *)s
{
    if (![model containsEntity:s inArray:0]||s.readOnly)
        s = NULL;
    NSInteger tag = -1;
    if (s)
        tag = [model indexForEntity:s inArray:0]-[model numberOfReadOnlyEntitiesInArray:0];
    [self selectItemWithTag:tag];
}

-(void)setModel:(ObjectController *)oc
{
    PLObject * sel = [self selectedObject];
    model = oc;
    NSArray * obj = [model objects];
    NSMenu * m = [[[NSMenu alloc] init] autorelease];
    NSMenuItem * it;
    it = [[[NSMenuItem alloc] init] autorelease];
    it.title = @"NULL";
    [it setTag:-1];
    [m addItem:it];
    
    int i = 0;
    for (PLObject * o in obj)
        if (!o.readOnly)
        {
            it = [[[NSMenuItem alloc] init] autorelease];
            it.title = [o description];
            [it setTag:i];
            i++;
            [m addItem:it];
        }
    [self setMenu:m];
    
    [self setSelectedObject:sel];
}

@end

//
//  PLTableView.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/19/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLTableView.h"
#import "InterfaceController.h"

@implementation PLTableView

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

-(void)new:(id)sender
{
    [(InterfaceController*)self.delegate new:self];
}

-(void)delete:(id)sender
{
    [(InterfaceController*)self.delegate delete:self];
}

-(void)copy:(id)sender
{
    [(InterfaceController*)self.delegate copy:self];    
}

-(void)paste:(id)sender
{
    [(InterfaceController*)self.delegate paste:self];    
}

-(void)duplicate:(id)sender
{
    [(InterfaceController*)self.delegate duplicate:self];    
}

-(BOOL)validateMenuItem:(NSMenuItem*)menuItem
{
    return [(InterfaceController*)self.delegate validateMenuItem:menuItem sentFrom:self];
}

@end

/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "PLTableView.h"
#import "MultipleListController.h"

@implementation PLTableView

-(void)new:(id)sender
{
    [(MultipleListController*)self.delegate new:self];
}

-(void)delete:(id)sender
{
    [(MultipleListController*)self.delegate delete:self];
}

-(void)copy:(id)sender
{
    [(MultipleListController*)self.delegate copy:self];    
}

-(void)paste:(id)sender
{
    [(MultipleListController*)self.delegate paste:self];    
}

-(void)duplicate:(id)sender
{
    [(MultipleListController*)self.delegate duplicate:self];    
}

-(BOOL)validateMenuItem:(NSMenuItem*)menuItem
{
    return [(MultipleListController*)self.delegate validateMenuItem:menuItem sentFrom:self];
}

@end

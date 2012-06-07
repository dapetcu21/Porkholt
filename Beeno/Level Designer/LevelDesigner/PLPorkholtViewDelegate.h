/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>

@protocol PLPorkholtViewDelegate <NSObject>
-(void)new:(id)sender;
-(void)copy:(id)sender;
-(void)paste:(id)sender;
-(void)duplicate:(id)sender;
-(void)delete:(id)sender;
-(void)selectAll:(id)sender;
-(BOOL)validateMenuItem:(NSMenuItem*)menuItem sentFrom:(id)sender;
-(IBAction)toggleShowMarkers:(id)sender;
-(IBAction)toggleShowImages:(id)sender;
-(IBAction)toggleShowFixtures:(id)sender;
-(IBAction)toggleShowJoints:(id)sender;
-(IBAction)toggleObjectMode:(id)sender;
-(IBAction)resetAspectRatio:(id)sender;
-(IBAction)match:(id)sender;
@end

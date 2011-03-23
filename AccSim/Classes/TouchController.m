//
//  TouchController.m
//  AccSim
//
//  Created by Marius Petcu on 3/23/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "TouchController.h"
#import "NetworkView.h"

@implementation TouchController
@synthesize parent;
// The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
/*
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization.
    }
    return self;
}
*/

/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations.
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}

- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}

-(void)processEvent:(UITouch*)touch state:(int)state X:(double)x Y:(double)y
{
	static int time = 0;
	time++;
	const char * msg = [[NSString stringWithFormat:@"TCH: ,%u,%d,%d,%.2f,%.2f",touch,time,state,x,y] UTF8String];
	[parent sendPacket:msg length:strlen(msg)];
}

@end

//
//  FlipsideViewController.m
//  PHRemote
//
//  Created by Marius Petcu on 6/17/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "FlipsideViewController.h"
#import "MainViewController.h"

@implementation FlipsideViewController

@synthesize delegate=_delegate;

- (void)dealloc
{
    [super dealloc];
}

-(MainViewController*)delegate
{
    return delegate;
}

-(void)setDelegate:(MainViewController *)del
{
    delegate = del;
    
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    accswitch.on = self.delegate.sendAcc;
    touchswitch.on = self.delegate.sendTouch;
    hostfield.text = self.delegate.host;
    portfield.text = self.delegate.port;
    // Release any cached data, images, etc. that aren't in use.
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
    accswitch.on = self.delegate.sendAcc;
    touchswitch.on = self.delegate.sendTouch;
    hostfield.text = self.delegate.host;
    portfield.text = self.delegate.port;
    self.view.backgroundColor = [UIColor viewFlipsideBackgroundColor];  
}

- (IBAction)settingsChanged:(id)sender
{
    if (sender == hostfield)
        self.delegate.host = hostfield.text;
    if (sender == portfield)
        self.delegate.port = portfield.text;
    if (sender == accswitch)
        self.delegate.sendAcc = accswitch.on;
    if (sender == touchswitch)
        self.delegate.sendTouch = touchswitch.on;
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma mark - Actions

- (IBAction)done:(id)sender
{
    [self.delegate flipsideViewControllerDidFinish:self];
}

@end

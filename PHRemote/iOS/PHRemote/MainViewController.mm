//
//  MainViewController.m
//  PHRemote
//
//  Created by Marius Petcu on 6/17/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "FlipsideViewController.h"
#import "MainViewController.h"
#include <stdint.h>
@implementation MainViewController
@synthesize sendAcc;
@synthesize sendTouch;
@synthesize groupPackets;

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad
{
    [[UIAccelerometer sharedAccelerometer] setDelegate:self];
    [UIAccelerometer sharedAccelerometer].updateInterval=1.0f/15;
    [super viewDidLoad];
    self.port = @"2221";
    self.host = @"255.255.255.255";
    self.sendAcc = YES;
    self.sendTouch = YES;
    [self start];
}

- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration
{
    if (!(sendAcc && acceleration) && (!sendTouch || queue.empty())) return;
    remote.beginPacket(0xAC);
    URField accfield;
    if (sendAcc && acceleration)
    {
        int64_t acc[3];
        acc[0] = (int64_t)([acceleration x]*1048576); //this is ugly
        acc[1] = (int64_t)([acceleration y]*1048576);
        acc[2] = (int64_t)([acceleration z]*1048576);
        accfield.setTag(0x01);
        accfield.setInt64s((uint64_t*)acc, 3);
        remote.addField(accfield);
    }
    
    URField * x = NULL;
    URField * y = NULL;
    URField * ide = NULL;
    URField * phase = NULL;
    
    if (sendTouch && !queue.empty())
    {
        URField send;
        send.setTag(0x08);
        URField wi;
        wi.setTag(0x06);
        wi.setInt32(self.view.bounds.size.width);
        remote.addField(wi);
        URField he;
        he.setTag(0x07);
        he.setInt32(self.view.bounds.size.height);
        remote.addField(he);
        
        int n = queue.size();
        
        x = new URField[n];
        y = new URField[n];
        ide = new URField[n];
        phase = new URField[n];
        
        for(int i=0; i<n; i++)
        {
            touch t = queue.front();
            queue.pop_front();
            
            ide[i].setTag(0x02);
            ide[i].setInt32((uint32_t)t.id);
            remote.addField(ide[i]);
            
            phase[i].setTag(0x03);
            phase[i].setInt8((uint8_t)t.state);
            remote.addField(phase[i]);
            
            x[i].setInt32(t.x);
            x[i].setTag(0x04);
            remote.addField(x[i]);
            
            y[i].setInt32(t.y);
            y[i].setTag(0x05);
            remote.addField(y[i]);
            
            remote.addField(send);
            
        }
        
    }
    
    status.text = @"";
    try {remote.endPacket(0);}
    catch (std::string ex) {status.text = [NSString stringWithUTF8String:ex.c_str()];};
    
    delete[] x;
    delete[] y;
    delete[] ide;
    delete[] phase;
}

- (void)sendTouchPack:(int)ph withTouch:(UITouch*)tt inView:(UIView*)view
{
    if (!sendTouch) return;
    touch t;
    t.state = ph;
    t.id = (void*)tt;
    t.x = [tt locationInView:view].x;
    t.y = [tt locationInView:view].y;
    queue.push_back(t);
    if (!groupPackets)
        [self accelerometer:nil didAccelerate:nil];
}

- (void)start
{
    status.text = @"";
    try {remote.start();}
    catch (std::string ex) {status.text = [NSString stringWithUTF8String:ex.c_str()];};
}

- (void)stop
{
    status.text = @"";
    remote.stop();
}

- (void)setHost:(NSString *)h
{
    [h retain];
    [host release];
    host = h;
    try {remote.setHostname([host UTF8String]);}
    catch (std::string ex) {status.text = [NSString stringWithUTF8String:ex.c_str()];}
}

- (NSString*)host
{
    return host;
}

- (void)setPort:(NSString *)p
{
    [p retain];
    [port release];
    port = p;
    status.text = @"";
    try {remote.setPort([port UTF8String]);}
    catch (std::string ex) {status.text = [NSString stringWithUTF8String:ex.c_str()];}
}

- (NSString*)port
{
    return port;
}

- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller
{
    [[UIApplication sharedApplication] setStatusBarHidden:YES];
    [self dismissModalViewControllerAnimated:YES];
    [self start];
}

- (IBAction)showInfo:(id)sender
{    
    FlipsideViewController *controller = [[FlipsideViewController alloc] initWithNibName:@"FlipsideView" bundle:nil];
    controller.delegate = self;
    
    controller.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
    [[UIApplication sharedApplication] setStatusBarHidden:NO];
    [self presentModalViewController:controller animated:YES];
    [self stop];
    
    [controller release];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations.
    return (interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}

- (void)viewDidUnload
{
    [super viewDidUnload];

    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (void)dealloc
{
    [host release];
    [port release];
    [super dealloc];
}

@end

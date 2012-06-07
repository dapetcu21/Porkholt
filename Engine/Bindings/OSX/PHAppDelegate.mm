/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHWindowing.h"
#import "PHAppDelegate.h"
#import "PHWindow.h"

PHWindow * PHWMainWindow = NULL;

@interface NSApplication (NiblessAdditions)
-(void) setAppleMenu:(NSMenu *)aMenu;
@end

@implementation PHAppDelegate

-(id)initWithVM:(const PHWVideoMode &) _vm flags:(int)_flags entryPoint:(void (*)(PHGameManager *))_entryPoint ud:(void*)_ud
{
    if ((self = [super init]))
    {
        flags = _flags;
        ud = _ud;
        vm = _vm;
        entryPoint = _entryPoint;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (void)quitApp:(id)sender
{
    PHWClose();
}

- (void)windowWillClose:(NSNotification *)notification
{
    window.delegate = nil;
    window = nil;
    PHWMainWindow = nil;
    [NSApp stop:self];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSString * title = [[[NSBundle mainBundle] infoDictionary]   objectForKey:@"CFBundleName"];
    window = PHWMainWindow = [[PHWindow alloc] initWithVM:vm flags:flags entryPoint:entryPoint ud:ud];
    window.title = title;
    window.delegate = self;
    
    NSMenu * mainMenu = [[[NSMenu alloc] initWithTitle:@"MainMenu"] autorelease];
    
	NSMenuItem * item;
	NSMenu * submenu;
	
    item = [mainMenu addItemWithTitle:@"Apple" action:NULL keyEquivalent:@""];
	submenu = [[[NSMenu alloc] initWithTitle:@"Apple"] autorelease];
    [NSApp setAppleMenu:submenu];    
    [submenu addItemWithTitle:@"Quit" action:@selector(quitApp:) keyEquivalent:@"q"];
	[mainMenu setSubmenu:submenu forItem:item];
    
    [NSApp setMainMenu:mainMenu];
    
    [window makeKeyAndOrderFront:self];
}

@end

//
//  PHAppDelegate.mm
//  Porkholt
//
//  Created by Marius Petcu on 4/10/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#import "PHAppDelegate.h"
#include "PHStartGame.h"

void * PHMainWindow = NULL;

@implementation PHAppDelegate

-(id)initWithResX:(unsigned int)_resX resY:(unsigned int)_resY flags:(int)_flags entryPoint:(void (*)(PHGameManager *))_entryPoint ud:(void*)_ud
{
    if ((self = [super init]))
    {
        flags = _flags;
        ud = _ud;
        resX = _resX;
        resY = _resY;
        entryPoint = _entryPoint;
    }
    return self;
}

- (void)dealloc
{
    PHMainWindow = nil;
    [super dealloc];
}

- (void)windowWillClose:(NSNotification *)notification
{
    [NSApp terminate:self];
}

@interface NSApplication (NiblessAdditions)
-(void) setAppleMenu:(NSMenu *)aMenu;
@end

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSString * title = [[[NSBundle mainBundle] infoDictionary]   objectForKey:@"CFBundleName"];
    PHMainWindow = window = (NSWindow*)PHCreateWindow(string([title UTF8String]), resX, resY, flags, entryPoint, ud);
    window.delegate = self;
    
    NSMenu * mainMenu = [[[NSMenu alloc] initWithTitle:@"MainMenu"] autorelease];
    
	NSMenuItem * item;
	NSMenu * submenu;
	
    item = [mainMenu addItemWithTitle:@"Apple" action:NULL keyEquivalent:@""];
	submenu = [[[NSMenu alloc] initWithTitle:@"Apple"] autorelease];
    [NSApp setAppleMenu:submenu];    
    [submenu addItemWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
	[mainMenu setSubmenu:submenu forItem:item];
    
    if (flags & (PHStartGame_Resizable | PHStartGame_FullScreen))
    {
        item = [mainMenu addItemWithTitle:@"View" action:NULL keyEquivalent:@""];
        submenu = [[[NSMenu alloc] initWithTitle:@"View"] autorelease];   
        [submenu addItemWithTitle:@"Full Screen" action:@selector(toggleFullScreen:) keyEquivalent:@"f"];
        [mainMenu setSubmenu:submenu forItem:item];
    }
    
    [NSApp setMainMenu:mainMenu];
}

@end

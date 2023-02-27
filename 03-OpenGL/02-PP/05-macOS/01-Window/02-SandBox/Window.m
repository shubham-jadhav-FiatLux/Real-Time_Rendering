
// Header Files
#import <Foundation/Foundation.h>                       // #import == #pragma once
#import <cocoa/cocoa.h>

// Global Variable Declarations

// Interface Declarations
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

// Entry-Point Function
int main(int argc, char* argv[])
{
    // Code
    // Create AutoreleasePool
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc]init];

    // Create Application Object
    NSApp = [NSApplication sharedApplication];

    // Create AppDelegate Object
    AppDelegate* appDelegate = [[AppDelegate alloc]init];

    // give Our AppDelegate To NSApp
    [NSApp setDelegate : appDelegate];

    // Start RunLoop
    [NSApp run];

    // Tell AutoreleasePool To Release
    [pool release];

    return 0;
}

// Implementation Of AppDelegate
@implementation AppDelegate
{
    @private
    NSWindow *window;
}
- (void)applicationDidFinishLaunching : (NSNotification*)notification
{
    // Code
    NSLog(@"Application Started Successfully..\n");
    NSRect rect = NSMakeRect(0.0, 0.0, 800.0, 600.0);
    window = [[NSWindow alloc]initWithContentRect : rect
                                        styleMask : NSWindowStyleMaskTitled |                         NSWindowStyleMaskClosable |                       NSWindowStyleMaskMiniaturizable |                 NSWindowStyleMaskResizable
                                          backing : NSBackingStoreBuffered
                                            defer : NO];
    [window setTitle : @"macOS Window : SBJ"];
    NSColor* backgroundColor = [NSColor blackColor];
    [window setBackgroundColor : backgroundColor];
    [window center];
    
    // Set window
    [window setDelegate : self];
    [window makeKeyAndOrderFront : self];
}

- (void)applicationWillTerminate:(NSNotification*)notification
{
    // Code
    NSLog(@"Application Finished Successfully..\n");
}

- (void)windowWillClose:(NSNotification*)notification
{
    // Code
    [NSApp terminate : self];
}

- (void)dealloc
{
    // Code
    if (window)
    {
        [window release];
        window = nil;
    }

    [super dealloc];
}

@end


// Commands
// mkdir -p Window.app/Contents/macOS
// clang -o Window.app/Contents/macOS/Window Window.m -framwork Cocoa
// ./Window

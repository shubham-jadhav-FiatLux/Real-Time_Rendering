
// Header Files
#import <Foundation/Foundation.h>                       // #import == #pragma once
#import <cocoa/cocoa.h>

// Global Variable Declarations
FILE* gpFile = NULL;

// Interface Declarations
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface MyView : NSView
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
    MyView *view;
}
- (void)applicationDidFinishLaunching : (NSNotification*)notification
{
    // Code
    NSBundle* appBundle = [NSBundle mainBundle];
    NSString* appDirPath = [appBundle bundlePath];
    NSString* parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    NSString* logFileNameWithPath = [NSString stringWithFormat : @"%@/Log.txt", parentDirPath];
    if (logFileNameWithPath == nil)
    {
        [self release];
        [NSApp terminate : self];
    }
    //const char* pszLogFileNameWithPath = [logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
    const char* pszLogFileNameWithPath = [logFileNameWithPath UTF8String];
    gpFile = fopen(pszLogFileNameWithPath, "w");
    if (gpFile == NULL)
    {
        [self release];
        [NSApp terminate : self];
    }
    fprintf(gpFile,"Program Started Successfully..\n");
    
    NSRect rect = NSMakeRect(0.0, 0.0, 800.0, 600.0);
    window = [[NSWindow alloc]initWithContentRect : rect
                                        styleMask : NSWindowStyleMaskTitled |                         NSWindowStyleMaskClosable |                       NSWindowStyleMaskMiniaturizable |                 NSWindowStyleMaskResizable
                                          backing : NSBackingStoreBuffered
                                            defer : NO];
    [window setTitle : @"macOS Window : SBJ"];
    NSColor* backgroundColor = [NSColor blackColor];
    [window setBackgroundColor : backgroundColor];
    [window center];
    
    view = [[MyView alloc]initWithFrame : rect];
    [window setContentView : view];
    
    // Set window
    [window setDelegate : self];
    [window makeKeyAndOrderFront : self];
}

- (void)applicationWillTerminate:(NSNotification*)notification
{
    // Code
    if (gpFile)
    {
        fprintf(gpFile, "Program Finished Successfully..\n");
        fclose(gpFile);
        gpFile = NULL;
    }
}

- (void)windowWillClose:(NSNotification*)notification
{
    // Code
    [NSApp terminate : self];
}

- (void)dealloc
{
    // Code
    if (view)
    {
        [view release];
        view = nil;
    }
    
    if (window)
    {
        [window release];
        window = nil;
    }

    [super dealloc];
}

@end

// Implementation Of MyView
@implementation MyView
{
    @private
    NSString* string;                                // Cocoa consider NSString as special class and there is no need of explicit releasing needed in dealloc
}

- (id)initWithFrame: (NSRect)frame
{
    // Code
    self = [super initWithFrame : frame];
    if (self)
    {
        string = @"Hello, Wolrd !!!";
    }
    return self;
}

- (void)drawRect : (NSRect)dirtyRect
{
    // Code
    NSFont *font = [NSFont fontWithName:@"Helvetica" size:32];
    NSColor* textColor = [NSColor greenColor];
    NSDictionary *dictionary = [NSDictionary dictionaryWithObjectsAndKeys:                                              font,NSFontAttributeName,
                                  textColor, NSForegroundColorAttributeName,
                                  nil];
    
    NSSize textSize = [string sizeWithAttributes:dictionary];
    NSPoint point;
    point.x = dirtyRect.size.width / 2 - textSize.width / 2;
    point.y = dirtyRect.size.height / 2 - textSize.height / 2;        // y + 12 For doc height

    [string drawAtPoint:point withAttributes:dictionary];
}

- (BOOL)acceptsFirstResponder
{
    // Code
    [[self window]makeFirstResponder : self];
    return YES;
}

- (void)keyDown: (NSEvent *) event
{
    // Code
    int key = (int) [[event characters] characterAtIndex:0];

    switch (key)
    {
        case 27:
            [self release];
            [NSApp terminate : self];
            break;
            
        case 'F':
        case 'f':
            string = @"F Key Is Pressed";
            [[self window]toggleFullScreen:self];
            break;

        default:
            break;
    }
}

- (void)mouseDown: (NSEvent *) event
{
    // Code
    string = @"Mouse Is Clicked";
    [self setNeedsDisplay : YES];                // Re-paint
}

- (void)dealloc
{
    // Code
    [super dealloc];
}

@end

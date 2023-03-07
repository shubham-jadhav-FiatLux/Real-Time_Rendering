//
//  AppDelegate.m
//  Window
//
//  Created by mac on 27/02/23.
//

#import "AppDelegate.h"
#import "ViewController.h"
#import "GLESView.h"

@implementation AppDelegate
{
    @private
    UIWindow* window;
    ViewController* viewController;
    GLESView* view;
}

- (BOOL)application:(UIApplication*)application willFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
    // Code
    // Create Window
    window = [UIWindow new];
    
    // Set background color
    [window setBackgroundColor:[UIColor blackColor]];
    
    viewController = [[ViewController alloc]init];
    [window setRootViewController:viewController];
    
    CGRect rect = window.screen.bounds;      // CGRect rect = [[window screen]bounds];
    view = [[GLESView alloc]initWithFrame:rect];
    
    [viewController setView:view];
    [view release];
    
    [window makeKeyAndVisible];
    
    [view startAnimation];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Code
    [view stopAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Code
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Code
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Code
    [view startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Code
    [view stopAnimation];
}

- (void)dealloc
{
    // Code
    [super dealloc];
    
    if (view)
    {
        [view release];
        view = nil;
    }
    
    if (viewController)
    {
        [viewController release];
        viewController = nil;
    }
    
    if (window)
    {
        [window release];
        window = nil;
    }
}

@end

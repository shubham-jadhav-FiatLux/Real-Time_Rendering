//
//  AppDelegate.m
//  Window
//
//  Created by mac on 27/02/23.
//

#import "AppDelegate.h"
#import "ViewController.h"
#import "MyView.h"

@implementation AppDelegate
{
    @private
    UIWindow* window;
    ViewController* viewController;
    MyView* view;
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
    view = [[MyView alloc]initWithFrame:rect];
    
    [viewController setView:view];
    [view release];
    
    [window makeKeyAndVisible];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Code
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
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Code
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

//
//  main.m
//  Window
//
//  Created by mac on 27/02/23.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"


int main(int argc, char* argv[])
{
    // Code
    // Create AutoreleasePool
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc]init];
    
    NSString* delegateClassName = NSStringFromClass([AppDelegate class]);
    
    int result = UIApplicationMain(argc, argv, nil, delegateClassName);
    
    // Tell AutoreleasePool To Release
    [pool release];
    
    return result;
}

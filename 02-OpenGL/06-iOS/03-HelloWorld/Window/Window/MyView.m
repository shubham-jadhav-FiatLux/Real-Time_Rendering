//
//  MyView.m
//  Window
//
//  Created by mac on 27/02/23.
//

#import "MyView.h"

@implementation MyView
{
    @private
    NSString* string;
}

- (id)initWithFrame:(CGRect)frame
{
    // Code
    self = [super initWithFrame:frame];
    if (self)
    {
        string = @"Hello, World !";
        
        // Gesture Recognizer Code
        // Single Tap
        UITapGestureRecognizer* singleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
        [singleTapGestureRecognizer setNumberOfTapsRequired:1];
        [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [singleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        
        // Double Tap
        UITapGestureRecognizer* doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [doubleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        // Differentiate Between Single Tap And Double Tap
        [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
        
        
        // Swipe
        UISwipeGestureRecognizer* swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        
        // Long Press
        UILongPressGestureRecognizer* longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        [self addGestureRecognizer:longPressGestureRecognizer];
    }
    
    return self;
}

- (void)drawRect : (CGRect)dirtyRect
{
    // Code
    UIColor* backgroundColor = [UIColor blackColor];
    [backgroundColor set];
    UIRectFill(dirtyRect);
    
    UIFont *font = [UIFont fontWithName:@"Helvetica" size:24];
    UIColor* textColor = [UIColor greenColor];
    NSDictionary *dictionary = [NSDictionary dictionaryWithObjectsAndKeys:                                              font,NSFontAttributeName,
                                  textColor, NSForegroundColorAttributeName,
                                  nil];
    
    CGSize textSize = [string sizeWithAttributes:dictionary];
    CGPoint point;
    point.x = (dirtyRect.size.width / 2) - (textSize.width / 2);
    point.y = (dirtyRect.size.height / 2) - (textSize.height / 2);  // y+12 For doc height

    [string drawAtPoint:point withAttributes:dictionary];
}

- (BOOL)acceptsFirstResponder
{
    // Code
    return YES;
}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    // Code
    
}

- (void)onSingleTap:(UITapGestureRecognizer*)tgr
{
    // Code
    string = @"Single Tap";
    [self setNeedsDisplay];
}

- (void)onDoubleTap:(UITapGestureRecognizer*)tgr
{
    // Code
    string = @"Double Tap";
    [self setNeedsDisplay];
}

- (void)onSwipe:(UISwipeGestureRecognizer*)sgr
{
    // Code
    [self release];
    exit(0);
}

- (void)onLongPress:(UILongPressGestureRecognizer*)lpgr
{
    // Code
    string = @"Long Press";
    [self setNeedsDisplay];
}

- (void)dealloc
{
    // Code
    [super dealloc];
}

@end

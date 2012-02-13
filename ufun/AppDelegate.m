//
//  AppDelegate.m
//  ufun
//
//  Created by  on 12-2-9.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "AppDelegate.h"

#import "FirstViewController.h"

#import "SecondViewController.h"
#import "ASIHTTPRequest.h"
#import "GCDAsyncSocket.h"

@implementation AppDelegate

@synthesize window = _window;
@synthesize tabBarController = _tabBarController;

- (void)dealloc
{
    [_window release];
    [_tabBarController release];
    [super dealloc];
}
#define HTTPMETHOD @"GET"
#define HTTPVERSION @"HTTP/1.1"
#define HTTPHOST @"Host"

#define KENTER @"\r\n"
#define KBLANK @" "

#define HTTP_HEADER   0
#define HTTP_BODY     1

- (NSMutableString*) makeHttpHeader:(NSString*) hostName
{
    NSMutableString *header = [[NSMutableString alloc] init];
    
    [header appendFormat:HTTPMETHOD];
    [header appendFormat:KBLANK];
    [header appendFormat:@"/index.html"];
    [header appendFormat:KBLANK];
    [header appendFormat:HTTPVERSION];
    [header appendFormat:KENTER];
    
    [header appendFormat:HTTPHOST];
    [header appendFormat:@":"];
    [header appendFormat:hostName];
    [header appendFormat:KENTER];
    [header appendFormat:KENTER];
    
    return [header autorelease];
}
- (void)socket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port
{
//    [sock readDataWithTimeout:3 tag:1];
//    [sock writeData:[@"GET / HTTP/1.0\r\nHost: www.baidu.com\r\n\r\n" dataUsingEncoding:NSUTF8StringEncoding] withTimeout:3 tag:1];
    
    NSString *requestStr = @"GET / HTTP/1.1\r\nHost: www.baidu.com\r\n\r\n";
	NSData *requestData = [requestStr dataUsingEncoding:NSUTF8StringEncoding];
    
    NSData *httpRequest = [[self makeHttpHeader:@"www.baidu.com"] dataUsingEncoding:NSUTF8StringEncoding];
	
	[sock writeData:httpRequest withTimeout:-1.0 tag:0];
    
    
    NSData *responseTerminatorData = [@"\r\n\r\n" dataUsingEncoding:NSASCIIStringEncoding];
	
    [sock readDataWithTimeout:3 tag:1];
	//[sock readDataToData:responseTerminatorData withTimeout:-1.0 tag:HTTP_HEADER];
}

- (int)parseHttpHeader:(NSData *)data
{
    return 0;
}

- (void)processHttpBody:(NSData *)data
{
    
}

- (void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag
{
    if (tag == HTTP_HEADER)
    {
        NSString *httpResponse = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
        NSLog(@"httpResponse=%@", httpResponse);
        
        int bodyLength = [self parseHttpHeader:data];
        [sock readDataToLength:7701 withTimeout:-1 tag:HTTP_BODY];
    }
    else if (tag == HTTP_BODY)
    {
        // Process response
        NSString *httpResponse = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
        [self processHttpBody:data];
        
        // Read header of next response
        NSData *term = [@"\r\n\r\n" dataUsingEncoding:NSUTF8StringEncoding];
        [sock readDataToData:term withTimeout:-1 tag:HTTP_HEADER];
    }
}

- (void)socketDidDisconnect:(GCDAsyncSocket *)sock withError:(NSError *)err
{
    NSLog(@"disconnect");
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    GCDAsyncSocket *asyncSocket = [[GCDAsyncSocket alloc] initWithDelegate:self delegateQueue:dispatch_get_main_queue()];
    NSError *error = nil;
	NSString *host = @"192.168.1.17";
    [asyncSocket connectToHost:host onPort:8080 error:&error];
    
    
    
//    NSString *url = @"http://www.baidu.com";
//    __block ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:[NSURL URLWithString:url]];
//    [request setCompletionBlock:^{
//        // Use when fetching text data
//        NSString *responseString = [request responseString];
//        
//        NSLog(@"responseString=%@", responseString);
//        
//        // Use when fetching binary data
//    }];
//    [request setFailedBlock:^{
//        NSError *error = [request error];
//        NSLog(@"error = %@", [error localizedDescription]);
//    }];
//    [request startAsynchronous];
    
//    DataHandleMgr *tt = [DataHandleMgr sharedInstance];
//    [tt dispatchData];
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    // Override point for customization after application launch.
    UIViewController *viewController1, *viewController2;
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        viewController1 = [[[FirstViewController alloc] initWithNibName:@"FirstViewController_iPhone" bundle:nil] autorelease];
        viewController2 = [[[SecondViewController alloc] initWithNibName:@"SecondViewController_iPhone" bundle:nil] autorelease];
        
    } else {
        viewController1 = [[[FirstViewController alloc] initWithNibName:@"FirstViewController_iPad" bundle:nil] autorelease];
        viewController2 = [[[SecondViewController alloc] initWithNibName:@"SecondViewController_iPad" bundle:nil] autorelease];
    }
    
    UINavigationController *nav1 = [[[UINavigationController alloc] initWithRootViewController:viewController1] autorelease];
    UINavigationController *nav2 = [[[UINavigationController alloc] initWithRootViewController:viewController2] autorelease];
    
    self.tabBarController = [[[UITabBarController alloc] init] autorelease];
    self.tabBarController.viewControllers = [NSArray arrayWithObjects:nav1, nav2, nil];
    self.window.rootViewController = self.tabBarController;
    [self.window makeKeyAndVisible];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
     */
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    /*
     Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
     */
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    /*
     Called when the application is about to terminate.
     Save data if appropriate.
     See also applicationDidEnterBackground:.
     */
}

/*
// Optional UITabBarControllerDelegate method.
- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController
{
}
*/

/*
// Optional UITabBarControllerDelegate method.
- (void)tabBarController:(UITabBarController *)tabBarController didEndCustomizingViewControllers:(NSArray *)viewControllers changed:(BOOL)changed
{
}
*/

@end

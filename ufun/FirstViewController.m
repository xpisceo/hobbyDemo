//
//  FirstViewController.m
//  ufun
//
//  Created by  on 12-2-9.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "FirstViewController.h"
#import "testViewController.h"
#import "DispatchDataOnRunLoop.h"

@implementation FirstViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        self.title = NSLocalizedString(@"First", @"First");
        self.tabBarItem.image = [UIImage imageNamed:@"first"];
    }
    return self;
}
							
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
	[super viewDidDisappear:animated];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
    } else {
        return YES;
    }
}
DispatchDataOnRunLoop *dispach;

- (IBAction)fireData:(id)sender {
    RunLoopMgr *mgr = [RunLoopMgr sharedInstance];
    [mgr fireSource:dispach];
}

- (IBAction)testShow:(id)sender {
//    testViewController *tt = [[testViewController alloc] init];
//    //[self.view addSubview:tt.view];
//    //[self presentModalViewController:tt animated:YES];
//    tt.hidesBottomBarWhenPushed = YES;
//    [self.navigationController pushViewController:tt animated:YES];
    
    
//    DataHandleMgr *tt = [DataHandleMgr sharedInstance];
//    [tt dispatchData];
    
    dispach = [[DispatchDataOnRunLoop alloc] init];
    [dispach addToCurrentRunLoop];
    [dispach release];
    
}


@end

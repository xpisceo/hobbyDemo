//
//  CustomInputSource.m
//  runloopios4
//
//  Created by Haven on 11/2/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "RunLoopInputSource.h"
#import "ConstDefine.h"

/************************************************************************************************
 class     : RunLoopSource
 data      : 2012-02-10
 mark      : Manage the Custom RunLoop Source
 
 ***********************************************************************************************/
@implementation RunLoopSource

@synthesize rl;

- (id)init
{
    CFRunLoopSourceContext    context = {0, self, NULL, NULL, NULL, NULL, NULL,
		&RunLoopSourceScheduleRoutine,
		&RunLoopSourceCancelRoutine,
		&RunLoopSourcePerformRoutine};
	
    runLoopSource = CFRunLoopSourceCreate(NULL, 0, &context);
	
    return self;
}

- (void)addToCurrentRunLoop
{
    CFRunLoopRef runLoop = CFRunLoopGetCurrent();
    rl = runLoop;
    CFRunLoopAddSource(runLoop, runLoopSource, kCFRunLoopDefaultMode);
}

- (void)removeFromRunLoop
{
    CFRunLoopRemoveSource(rl, runLoopSource, kCFRunLoopDefaultMode);
}

- (void)invalidate
{
	CFRunLoopSourceInvalidate(runLoopSource);
}

// Handler method
- (void)sourceFired
{
	if ([NSThread currentThread] == [NSThread mainThread]) 
	{
		NSLog(@"custom source fired in main thread");
	}
	else
	{
		NSLog(@"custom source fired in other thread");
	}
}

- (void)fireCommandsOnRunLoop
{
    CFRunLoopSourceSignal(runLoopSource);
    CFRunLoopWakeUp(rl);
}

- (void)dealloc
{
    SAFE_CFRELEASE(runLoopSource);
}

@end


void RunLoopSourceScheduleRoutine (void *info, CFRunLoopRef rl, CFStringRef mode)
{
    RunLoopSource *obj   = (RunLoopSource*)info;
    RunLoopMgr    *rlMgr = [RunLoopMgr sharedInstance];
	
    [rlMgr registerSource:obj];
    
}

void RunLoopSourceCancelRoutine (void *info, CFRunLoopRef rl, CFStringRef mode)
{
    RunLoopSource *obj   = (RunLoopSource*)info;
    RunLoopMgr    *rlMgr = [RunLoopMgr sharedInstance];
	
    [rlMgr performSelectorOnMainThread:@selector(unregistSource:)
                            withObject:obj waitUntilDone:YES];
}


void RunLoopSourcePerformRoutine (void *info)
{
    RunLoopSource*  obj = (RunLoopSource*)info;
    [obj sourceFired];
}





/************************************************************************************************
 class     : RunLoopMgr
 data      : 2012-02-10
 mark      : Manage the Custom RunLoop Source
 
 ***********************************************************************************************/


@implementation RunLoopMgr

@synthesize runLoopSources;

+ (RunLoopMgr *)sharedInstance
{
    return (RunLoopMgr *)[super sharedInstance];
}


- (id)init
{
    if (self = [super init]) 
    {
        self.runLoopSources = [[[NSMutableArray alloc] init] autorelease];
    }
    
    return self;
}


- (void)registerSource:(RunLoopSource *)context
{
    [runLoopSources addObject:context];
}


- (void)unregistSource:(RunLoopSource *)context
{
    id    objToRemove = nil;
	
    for (RunLoopSource* resource in runLoopSources)
    {
        if ([context isEqual:context])
        {
            objToRemove = context;
            break;
        }
    }
	
    if (objToRemove)
        [runLoopSources removeObject:objToRemove];
}


- (void)fireSource:(RunLoopSource *)source
{
  
    if ([self.runLoopSources containsObject:source]) 
    {
        [source fireCommandsOnRunLoop];
    }
		
}

- (void)removeSource:(RunLoopSource *)source
{
    if ([self.runLoopSources containsObject:source]) 
    {
        [source removeFromRunLoop];
    }
}


- (void)dealloc
{
    SAFE_RELEASE(runLoopSources);
}

@end
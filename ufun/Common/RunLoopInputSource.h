//
//  CustomInputSource.h
//  runloopios4
//
//  Created by Haven on 11/2/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "LockableObjectWrapper.h"
#import "Singleton.h"


@interface RunLoopSource : LockableObjectWrapper
{
    CFRunLoopSourceRef runLoopSource;
    
    CFRunLoopRef rl;
}

@property (readonly) CFRunLoopRef rl;

- (id)init;
- (void)addToCurrentRunLoop;
- (void)removeFromRunLoop;
- (void)invalidate;

// Handler method
- (void)sourceFired;

@end



// These are the CFRunLoopSourceRef callback functions.
void RunLoopSourceScheduleRoutine (void *info, CFRunLoopRef rl, CFStringRef mode);
void RunLoopSourceCancelRoutine (void *info, CFRunLoopRef rl, CFStringRef mode);
void RunLoopSourcePerformRoutine (void *info);



@interface RunLoopMgr : Singleton 
{
    NSMutableArray *runLoopSources;
}

+(RunLoopMgr *)sharedInstance;

@property (nonatomic, retain) NSMutableArray *runLoopSources;


- (void)registerSource:(RunLoopSource *)context;
- (void)unregistSource:(RunLoopSource *)context;


- (void)fireSource:(RunLoopSource *)source;
- (void)removeSource:(RunLoopSource *)source;
@end

#import "LockableObjectWrapper.h"

@implementation LockableObjectWrapper

- (void) dealloc {
	[object release];
	[lock release];
	[super dealloc];
}

- (id) init {
	return [self initWithObject:nil];
}

- (id) initWithObject:(id)anObject {
    if ([super init] == nil) return nil;
    if (anObject == nil) {
    	[NSException raise:NSInternalInconsistencyException
	                format:@"[ %@ %s] -- Must provide a non-nil object.",
		 [self class], sel_getName(_cmd)];
    }
    object = [anObject retain];
    lock = [[NSLock alloc] init];
    return self;
}

- (BOOL) tryLock {
    return [lock tryLock];
}

- (void) lock {
    [lock lock];
}

- (void) unlock {
    [lock unlock];
}

// See documentation for -[NSObject respondsToSelector:]
- (BOOL) respondsToSelector:(SEL)aSelector {
    return [object respondsToSelector:aSelector];
}

// See documentation for -[NSObject methodSignatureForSelector:]
- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector {
    return [object methodSignatureForSelector:aSelector];
}

// See documentation for -[NSObject forwardInvocation:]
- (void) forwardInvocation:(NSInvocation *)anInvocation {
    [anInvocation invokeWithTarget:object];
}

@end
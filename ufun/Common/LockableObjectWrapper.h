#import <Foundation/Foundation.h>

@interface LockableObjectWrapper : NSObject <NSLocking>
{
    id object;
    NSLock* lock;
}

- (id) initWithObject:(id)anObject;

- (BOOL) tryLock;
// -lock and -unlock declared in NSLocking protocol

@end
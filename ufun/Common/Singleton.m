//
//  Singleton.m
//  ufun
//
//  Created by  on 12-2-10.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "Singleton.h"

static Singleton * instance = nil;

@interface Singleton(privateMethods)

-(void)realRelease;

@end

@implementation Singleton
//获取单例
+(Singleton *)sharedInstance
{
    @synchronized(self) {
        if (instance == nil) {
            [[self alloc] init];
        }
    }
    return instance;
}

+(void)cleanup
{
    [instance realRelease];
}

//唯一一次alloc单例，之后均返回nil
+ (id)allocWithZone:(NSZone *)zone
{
    @synchronized(self) {
        if (instance == nil) {
            instance = [super allocWithZone:zone];
            return instance;
        }
    }
    return nil;
}

//copy返回单例本身
- (id)copyWithZone:(NSZone *)zone
{
    return self;
}

//retain返回单例本身
- (id)retain
{
    return self;
}

//引用计数总是为1
- (unsigned)retainCount
{
    return 1;
}

//autorelease返回单例本身
- (id)autorelease
{
    return self;
}

//真release私有接口
-(void)realRelease
{
    [super release];
}

//
-(void)dealloc
{
    //⋯⋯
    printf("举例:在此处做一些单例结束时的收尾处理/n"); 
    [super dealloc];
}
@end

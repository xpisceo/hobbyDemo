//
//  Singleton.h
//  ufun
//
//  Created by  on 12-2-10.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Singleton : NSObject

+(Singleton *)sharedInstance;
+(void)cleanup;
@end

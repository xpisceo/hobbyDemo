//
//  HackerNewsLoginController.h
//  newsyc
//
//  Created by Grant Paul on 4/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "HNKit.h"
#import "LoginController.h"

@interface HackerNewsLoginController : LoginController <HNSessionAuthenticatorDelegate> {
}

@end

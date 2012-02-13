//
//  SessionProfileController.h
//  newsyc
//
//  Created by Grant Paul on 3/29/11.
//  Copyright 2011 Xuzz Productions, LLC. All rights reserved.
//

#import "HNKit.h"

#import "ProfileController.h"
#import "LoginController.h"

@interface SessionProfileController : ProfileController <LoginControllerDelegate, UIActionSheetDelegate> {
    UIView *loginContainer;
    UIButton *loginButton;
    UIImageView *loginImage;
    UIBarButtonItem *logoutItem;
    BOOL isVisible;
}

@end

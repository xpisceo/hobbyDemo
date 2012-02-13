//
//  ProfileController.h
//  newsyc
//
//  Created by Grant Paul on 3/4/11.
//  Copyright 2011 Xuzz Productions, LLC. All rights reserved.
//

#import "LoadingController.h"

@class ProfileHeaderView;
@interface ProfileController : LoadingController <UITableViewDelegate, UITableViewDataSource> {
    UITableView *tableView;
    ProfileHeaderView *header;
}

@end

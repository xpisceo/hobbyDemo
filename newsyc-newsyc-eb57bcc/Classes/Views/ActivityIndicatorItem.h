//
//  ActivityIndicatorItem.h
//  newsyc
//
//  Created by Grant Paul on 4/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#define kActivityIndicatorItemStandardSize CGSizeMake(20, 20)

@interface ActivityIndicatorItem : UIBarButtonItem {
    UIActivityIndicatorView *spinner;
    UIView *container;
}

@property (nonatomic, readonly) UIActivityIndicatorView *spinner;

- (id)initWithSize:(CGSize)size;

@end

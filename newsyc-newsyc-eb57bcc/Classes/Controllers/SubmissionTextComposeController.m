//
//  SubmissionTextComposeController.m
//  newsyc
//
//  Created by Grant Paul on 3/31/11.
//  Copyright 2011 Xuzz Productions, LLC. All rights reserved.
//

#import "SubmissionTextComposeController.h"
#import "PlaceholderTextView.h"

@implementation SubmissionTextComposeController

- (BOOL)includeMultilineEditor {
    return YES;
}

- (NSString *)multilinePlaceholder {
    return @"Post Body";
}

- (NSString *)title {
    return @"Submit Text";
}

- (NSArray *)inputEntryCells {
    UITableViewCell *cell = [self generateTextFieldCell];
    [[cell textLabel] setText:@"Title:"];
    titleField = [self generateTextFieldForCell:cell];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textDidChange:) name:UITextFieldTextDidChangeNotification object:nil];
    [cell addSubview:titleField];
    
    return [NSArray arrayWithObject:cell];
}

- (UIResponder *)initialFirstResponder {
    return titleField;
}

- (void)submissionSucceededWithNotification:(NSNotification *)notification {
    [self sendComplete];
}

- (void)submissionFailedWithNotification:(NSNotification *)notification {
    [self sendFailed];
}

- (void)performSubmission {
    if (![self ableToSubmit]) {
        [self sendFailed];
    } else {
        HNSubmission *submission = [[HNSubmission alloc] initWithSubmissionType:kHNSubmissionTypeSubmission];
        [submission setTitle:[titleField text]];
        [submission setBody:[textView text]];
        [[HNSession currentSession] performSubmission:submission];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(submissionSucceededWithNotification:) name:kHNSubmissionSuccessNotification object:submission];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(submissionFailedWithNotification:) name:kHNSubmissionFailureNotification object:submission];
        [submission release];
    }
}

- (BOOL)ableToSubmit {
    return !([[titleField text] length] == 0 || [[textView text] length] == 0);
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [super dealloc];
}

AUTOROTATION_FOR_PAD_ONLY

@end

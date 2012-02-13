//
//  HNEntry.m
//  newsyc
//
//  Created by Grant Paul on 3/4/11.
//  Copyright 2011 Xuzz Productions, LLC. All rights reserved.
//

#import "NSURL+Parameters.h"

#import "HNKit.h"
#import "HNEntry.h"

@implementation HNEntry
@synthesize points, children, submitter, body, posted, parent, title, destination, entries;

+ (id)identifierForURL:(NSURL *)url_ {
    if (![self isValidURL:url_]) return NO;
    
    NSDictionary *parameters = [url_ parameterDictionary];
    return [NSNumber numberWithInt:[[parameters objectForKey:@"id"] intValue]];
}

+ (NSString *)pathForURLWithIdentifier:(id)identifier_ infoDictionary:(NSDictionary *)info {
    return @"item";
}

+ (NSDictionary *)parametersForURLWithIdentifier:(id)identifier_ infoDictionary:(NSDictionary *)info {
    return [NSDictionary dictionaryWithObject:identifier_ forKey:@"id"];
}

+ (id)entryWithIdentifier:(id)identifier_ {
    return [self objectWithIdentifier:identifier_];
}

- (BOOL)isComment {
    return title == nil;
}

- (BOOL)isSubmission {
    return title != nil;
}

- (void)loadFromDictionary:(NSDictionary *)response {
    if ([response objectForKey:@"url"] != nil) [self setDestination:[NSURL URLWithString:[response objectForKey:@"url"]]];
    if ([response objectForKey:@"user"] != nil) [self setSubmitter:[HNUser userWithIdentifier:[response objectForKey:@"user"]]];
    if ([response objectForKey:@"body"] != nil) [self setBody:[response objectForKey:@"body"]];
    if ([response objectForKey:@"date"] != nil) [self setPosted:[response objectForKey:@"date"]];
    if ([response objectForKey:@"title"] != nil) [self setTitle:[response objectForKey:@"title"]];
    if ([response objectForKey:@"points"] != nil) [self setPoints:[[response objectForKey:@"points"] intValue]];
    if ([response objectForKey:@"parent"] != nil) [self setParent:[HNEntry entryWithIdentifier:[response objectForKey:@"parent"]]];
        
    if ([response objectForKey:@"children"] != nil) {
        NSMutableArray *comments = [NSMutableArray array];
        for (NSDictionary *child in [response objectForKey:@"children"]) {
            HNEntry *entry = [HNEntry entryWithIdentifier:[child objectForKey:@"identifier"]];
            [entry loadFromDictionary:child];
            [entry setParent:self];
            
            if ([child objectForKey:@"children"] != nil) {
                [entry setIsLoaded:YES];
            } else {
                [entry setIsLoaded:NO];
            }
            
            [comments addObject:entry];
        }
         
        if ([[response objectForKey:@"append"] boolValue]) {
            [self setEntries:[[self entries] arrayByAddingObjectsFromArray:comments]];
        } else {
            [self setEntries:comments];
        }
    }
    
    if ([response objectForKey:@"numchildren"] != nil) {
        int count = [[response objectForKey:@"numchildren"] intValue];
        [self setChildren:count];
    } else {
        int count = [[self entries] count];
        for (HNEntry *child in [self entries])
            count += [child children];
        [self setChildren:count];
    }
}

- (void)finishLoadingWithResponse:(NSDictionary *)response error:(NSError *)error {
    if (error == nil) {
        [self loadFromDictionary:response];
    }
}

@end

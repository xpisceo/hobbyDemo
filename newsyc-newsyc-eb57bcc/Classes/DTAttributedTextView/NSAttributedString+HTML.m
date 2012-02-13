//
//  NSAttributedString+HTML.m
//  CoreTextExtensions
//
//  Created by Oliver Drobnik on 1/9/11.
//  Copyright 2011 Drobnik.com. All rights reserved.
//

#import <CoreText/CoreText.h>
#import <UIKit/UIKit.h>

#import "NSAttributedString+HTML.h"
#import "NSString+HTML.h"
#import "UIColor+HTML.h"
#import "NSScanner+HTML.h"
#import "NSAttributedStringRunDelegates.h"
#import "DTTextAttachment.h"

#import "DTCoreTextFontDescriptor.h"

// Allows variations to cater for different behavior on iOS than OSX to have similar visual output
#define ALLOW_IPHONE_SPECIAL_CASES 1

/* Known Differences:
 - OSX has an entire attributes block for an UL block
 - OSX does not add extra space after UL block
 */

// TODO: Decode HTML Entities
// TODO: make attributes case independent (currently lowercase)

#define UNICODE_OBJECT_PLACEHOLDER @"\ufffc"
#define UNICODE_LINE_FEED @"\u2028"


NSString *NSBaseURLDocumentOption = @"BaseURL";
NSString *NSTextEncodingNameDocumentOption = @"TextEncodingName";

CTParagraphStyleRef createDefaultParagraphStyle()
{
	CTTextAlignment alignment = kCTNaturalTextAlignment;
	CGFloat firstLineIndent = 0.0;
	CGFloat defaultTabInterval = 36.0;
	CFArrayRef tabStops = NULL;
	
	CTParagraphStyleSetting settings[] = {
		{kCTParagraphStyleSpecifierAlignment, sizeof(alignment), &alignment},
		{kCTParagraphStyleSpecifierFirstLineHeadIndent, sizeof(firstLineIndent), &firstLineIndent},
		{kCTParagraphStyleSpecifierDefaultTabInterval, sizeof(defaultTabInterval), &defaultTabInterval},
		{kCTParagraphStyleSpecifierTabStops, sizeof(tabStops), &tabStops}
		
	};	
	
	return CTParagraphStyleCreate(settings, 4);
}

CTParagraphStyleRef createParagraphStyle(CGFloat paragraphSpacingBefore, CGFloat paragraphSpacing, CGFloat headIndent, NSArray *tabStops, CTTextAlignment alignment)
{
	CGFloat firstLineIndent = 0.0;
	CGFloat defaultTabInterval = 36.0;
	
	NSMutableArray *textTabs = nil;
	
	NSInteger numTabs = [tabStops count];
	if (numTabs)
	{
		textTabs = [NSMutableArray array];
		
		// Convert from NSNumber to CTTextTab
		for (NSNumber *num in tabStops)
		{
			CTTextTabRef tab = CTTextTabCreate(kCTLeftTextAlignment, [num floatValue], NULL);
			[textTabs addObject:(id)tab];
			CFRelease(tab);
		}
	}
	
	CTParagraphStyleSetting settings[] = {
		{kCTParagraphStyleSpecifierAlignment, sizeof(alignment), &alignment},
		{kCTParagraphStyleSpecifierFirstLineHeadIndent, sizeof(firstLineIndent), &firstLineIndent},
		{kCTParagraphStyleSpecifierDefaultTabInterval, sizeof(defaultTabInterval), &defaultTabInterval},
		{kCTParagraphStyleSpecifierTabStops, sizeof(textTabs), &textTabs},
		{kCTParagraphStyleSpecifierParagraphSpacing, sizeof(paragraphSpacing), &paragraphSpacing},
		{kCTParagraphStyleSpecifierParagraphSpacingBefore, sizeof(paragraphSpacingBefore), &paragraphSpacingBefore},
		{kCTParagraphStyleSpecifierHeadIndent, sizeof(headIndent), &headIndent}
	};	
	
	return CTParagraphStyleCreate(settings, 7);
}


@implementation NSAttributedString (HTML)

- (id)initWithHTML:(NSData *)data documentAttributes:(NSDictionary **)dict
{
	return [self initWithHTML:data options:nil documentAttributes:dict];
}


- (id)initWithHTML:(NSData *)data baseURL:(NSURL *)base documentAttributes:(NSDictionary **)dict
{
	NSDictionary *optionsDict = nil;
	
	if (base)
	{
		optionsDict = [NSDictionary dictionaryWithObject:base forKey:NSBaseURLDocumentOption];
	}
	
	return [self initWithHTML:data options:optionsDict documentAttributes:dict];
}


- (void)remapCSSStylesOnTagDictionary:(NSMutableDictionary *)currentTag
{
	NSMutableDictionary *currentTagAttributes  = [currentTag objectForKey:@"Attributes"];
	
	DTCoreTextFontDescriptor *fontDescriptor = [currentTag objectForKey:@"FontDescriptor"];
	
	NSString *styleString = [currentTagAttributes objectForKey:@"style"];
	
	if (styleString)
	{
		NSDictionary *styles = [styleString dictionaryOfCSSStyles];
		
		NSString *fontSize = [styles objectForKey:@"font-size"];
		if (fontSize)
		{
			fontDescriptor.pointSize = [fontSize CSSpixelSize];
		}
		
		NSString *color = [styles objectForKey:@"color"];
		if (color)
		{
			[currentTagAttributes setObject:color forKey:@"color"];
			
		}
		
		// TODO: better mapping from font families to available families
		NSString *fontFamily = [[styles objectForKey:@"font-family"] lowercaseString];
		if (fontFamily)
		{
			if ([fontFamily rangeOfString:@"helvetica"].length || [fontFamily rangeOfString:@"arial"].length || [fontFamily rangeOfString:@"geneva"].length)
			{
				fontDescriptor.fontFamily = @"Helvetica";
			}
			else if ([fontFamily rangeOfString:@"courier"].length)
			{
				fontDescriptor.fontFamily = @"Courier";
			}
			else if ([fontFamily rangeOfString:@"cursive"].length)
			{
				fontDescriptor.stylisticClass = kCTFontScriptsClass;
				fontDescriptor.fontFamily = nil;
			}
			else if ([fontFamily rangeOfString:@"sans-serif"].length)
			{
				// too many matches (24)
				// fontDescriptor.stylisticClass = kCTFontSansSerifClass;
				fontDescriptor.fontFamily = @"Helvetica";
			}
			else if ([fontFamily rangeOfString:@"serif"].length)
			{
				// kCTFontTransitionalSerifsClass = Baskerville
				// kCTFontClarendonSerifsClass = American Typewriter
				// kCTFontSlabSerifsClass = Courier New
				// 
				// strangely none of the classes yields Times
				fontDescriptor.fontFamily = @"Times New Roman";
			}
			else if ([fontFamily rangeOfString:@"fantasy"].length)
			{
				fontDescriptor.fontFamily = @"Papyrus"; // only available on iPad
			}
			else if ([fontFamily rangeOfString:@"monospace"].length) 
			{
				fontDescriptor.monospaceTrait = YES;
				fontDescriptor.fontFamily = nil;
			}
			else
			{
				// probably something special or custom-font?
				fontDescriptor.fontName = [styles objectForKey:@"font-family"];
			}
		}
		
		NSString *fontStyle = [[styles objectForKey:@"font-style"] lowercaseString];
		if (fontStyle)
		{
			if ([fontStyle isEqualToString:@"normal"])
			{
				fontDescriptor.italicTrait = NO;
			}
			else if ([fontStyle isEqualToString:@"italic"] || [fontStyle isEqualToString:@"oblique"])
			{
				fontDescriptor.italicTrait = YES;
			}
			else if ([fontStyle isEqualToString:@"inherit"])
			{
				// nothing to do
			}
		}
		
		NSString *fontWeight = [[styles objectForKey:@"font-weight"] lowercaseString];
		if (fontWeight)
		{
			if ([fontWeight isEqualToString:@"normal"])
			{
				fontDescriptor.boldTrait = NO;
			}
			else if ([fontWeight isEqualToString:@"bold"])
			{
				fontDescriptor.boldTrait = YES;
			}
			else if ([fontWeight isEqualToString:@"bolder"])
			{
				fontDescriptor.boldTrait = YES;
			}
			else if ([fontWeight isEqualToString:@"lighter"])
			{
				fontDescriptor.boldTrait = NO;
			}
			else 
			{
				// can be 100 - 900
				
				NSInteger value = [fontWeight intValue];
				
				if (value<=600)
				{
					fontDescriptor.boldTrait = NO;
				}
				else 
				{
					fontDescriptor.boldTrait = YES;
				}
			}
		}
		
		
		NSString *decoration = [[styles objectForKey:@"text-decoration"] lowercaseString];
		if (decoration)
		{
			if ([decoration isEqualToString:@"underline"])
			{
				[currentTag setObject:[NSNumber numberWithInt:kCTUnderlineStyleSingle] forKey:@"UnderlineStyle"];
			}
			else if ([decoration isEqualToString:@"line-through"])
			{
				[currentTag setObject:[NSNumber numberWithBool:YES] forKey:@"_StrikeOut"];	
			}
			else if ([decoration isEqualToString:@"none"])
			{
				// remove all
				[currentTag removeObjectForKey:@"UnderlineStyle"];
				[currentTag removeObjectForKey:@"_StrikeOut"];
			}
			else if ([decoration isEqualToString:@"overline"])
			{
				//TODO: add support for overline decoration
			}
			else if ([decoration isEqualToString:@"blink"])
			{
				//TODO: add support for blink decoration
			}
			else if ([decoration isEqualToString:@"inherit"])
			{
				// nothing to do
			}
		}
		
		NSString *alignment = [[styles objectForKey:@"text-align"] lowercaseString];
		if (alignment)
		{
			if ([alignment isEqualToString:@"left"])
			{
				[currentTag setObject:[NSNumber numberWithInt:kCTLeftTextAlignment] forKey:@"TextAlignment"];
			}
			else if ([alignment isEqualToString:@"right"])
			{
				[currentTag setObject:[NSNumber numberWithInt:kCTRightTextAlignment] forKey:@"TextAlignment"];
			}
			else if ([alignment isEqualToString:@"center"])
			{
				[currentTag setObject:[NSNumber numberWithInt:kCTCenterTextAlignment] forKey:@"TextAlignment"];
			}
			else if ([alignment isEqualToString:@"justify"])
			{
				[currentTag setObject:[NSNumber numberWithInt:kCTJustifiedTextAlignment] forKey:@"TextAlignment"];
			}
			else if ([alignment isEqualToString:@"inherit"])
			{
				// nothing to do
			}
		}
		
		NSString *shadow = [styles objectForKey:@"text-shadow"];
		if (shadow)
		{
			NSString *currentColorString = [currentTag objectForKey:@"color"];
			UIColor *color = [UIColor colorWithHTMLName:currentColorString];
			NSArray *shadows = [shadow arrayOfCSSShadowsWithCurrentTextSize:fontDescriptor.pointSize currentColor:color];
			[currentTag setObject:shadows forKey:@"_Shadows"];
		}
		
	}
	
}
- (id)initWithHTML:(NSData *)data options:(NSDictionary *)options documentAttributes:(NSDictionary **)dict
{
	// Specify the appropriate text encoding for the passed data, default is UTF8 
	NSString *textEncodingName = [options objectForKey:NSTextEncodingNameDocumentOption];
	NSStringEncoding encoding = NSUTF8StringEncoding; // default
	
	if (textEncodingName)
	{
		CFStringEncoding cfEncoding = CFStringConvertIANACharSetNameToEncoding((CFStringRef)textEncodingName);
		encoding = CFStringConvertEncodingToNSStringEncoding(cfEncoding);
	}
	
	// use baseURL from options if present
	NSURL *baseURL = [options objectForKey:NSBaseURLDocumentOption];
	
	
	// Make it a string
	NSString *_htmlString = [[NSString alloc] initWithData:data encoding:encoding];
	
	// trim whitespace
	NSString *htmlString = [_htmlString stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
	
	[_htmlString release];
  
	NSMutableAttributedString *tmpString = [[[NSMutableAttributedString alloc] init] autorelease];
	
	NSMutableArray *tagStack = [NSMutableArray array];
	
	CGFloat nextParagraphAdditionalSpaceBefore = 0.0;
	BOOL seenPreviousParagraph = NO;
	NSInteger listCounter = 0;  // Unordered, set to 1 to get ordered list
	BOOL needsListItemStart = NO;
	BOOL needsNewLineBefore = NO;
	
	
	// we cannot skip any characters, NLs turn into spaces and multi-spaces get compressed to singles
	NSScanner *scanner = [NSScanner scannerWithString:htmlString];
	scanner.charactersToBeSkipped = nil;

	// base tag with font defaults
	DTCoreTextFontDescriptor *defaultFontDescriptor = [[[DTCoreTextFontDescriptor alloc] initWithFontAttributes:nil] autorelease];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSNumber *small = [defaults objectForKey:@"interface-small-text"];
    if (small == nil || [small boolValue]) {
        defaultFontDescriptor.pointSize = 12.0;
    } else {
        defaultFontDescriptor.pointSize = 14.0;
    }
	defaultFontDescriptor.fontFamily = @"Helvetica";
	NSDictionary *bodyTag = [NSDictionary dictionaryWithObjectsAndKeys:defaultFontDescriptor, @"FontDescriptor", @"black", @"color", nil];
	[tagStack addObject:bodyTag];
	
	NSMutableDictionary *currentTag = [tagStack lastObject];
	NSDictionary *previousAttributes = NULL;
	
	// skip initial whitespace
	[scanner scanCharactersFromSet:[NSCharacterSet whitespaceAndNewlineCharacterSet] intoString:NULL];

	
	
	while (![scanner isAtEnd]) 
	{
		NSString *tagName = nil;
		NSDictionary *tagAttributesDict = nil;
		BOOL tagOpen = YES;
		BOOL immediatelyClosed = NO;

		// default font
		if ([scanner scanHTMLTag:&tagName attributes:&tagAttributesDict isOpen:&tagOpen isClosed:&immediatelyClosed] && tagName)
		{
			if (![tagName isInlineTag])
			{
				// next text needs a NL
				needsNewLineBefore = YES;
			}
			
			NSDictionary *previousTag = currentTag;
			DTCoreTextFontDescriptor *currentFontDescriptor = nil;
			
			currentTag = [NSMutableDictionary dictionaryWithObject:tagName forKey:@"Tag"];
			[currentTag setDictionary:[tagStack lastObject]];
			[currentTag setObject:tagName forKey:@"_tag"];
			
			if (tagOpen)
			{
				NSDictionary *previousAttributes = [previousTag objectForKey:@"Attributes"];
				
				NSMutableDictionary *mutableAttributes = [NSMutableDictionary dictionaryWithDictionary:previousAttributes];
				
				for (NSString *oneKey in [tagAttributesDict allKeys])
				{
					[mutableAttributes setObject:[tagAttributesDict objectForKey:oneKey] forKey:oneKey];
				}
				
				tagAttributesDict = mutableAttributes;
				
				[currentTag setObject:mutableAttributes forKey:@"Attributes"];
				
				DTCoreTextFontDescriptor *parentFontDescriptor = [previousTag objectForKey:@"FontDescriptor"];
				
				if (parentFontDescriptor)
				{
					currentFontDescriptor = [[parentFontDescriptor copy] autorelease];  // inherit

					// never inherit font name
					currentFontDescriptor.fontName = nil;
				}
				else 
				{
					currentFontDescriptor = [DTCoreTextFontDescriptor fontDescriptorWithFontAttributes:nil];
					
					// set default
					currentFontDescriptor.pointSize = 12;
					currentFontDescriptor.fontFamily = @"Times New Roman";
				}
				
				[currentTag setObject:currentFontDescriptor forKey:@"FontDescriptor"];
				
				// copy color from parent
				NSString *color = [previousTag objectForKey:@"color"];
				if (color)
				{
					[currentTag setObject:color forKey:@"color"];
				}
			}
			
			NSMutableDictionary *currentTagAttributes = [currentTag objectForKey:@"Attributes"];
			
			
			// ---------- Processing
			
			if ([tagName isEqualToString:@"img"] && tagOpen)
			{
				immediatelyClosed = YES;
				
				NSString *src = [tagAttributesDict objectForKey:@"src"];
				CGFloat width = [[tagAttributesDict objectForKey:@"width"] intValue];
				CGFloat height = [[tagAttributesDict objectForKey:@"height"] intValue];
				
				// assume it's a relative file URL
				NSString *path = [[NSBundle mainBundle] pathForResource:src ofType:nil];
				UIImage *image = [UIImage imageWithContentsOfFile:path];
				
				if (image)
				{
					if (!width)
					{
						width = image.size.width;
					}
					
					if (!height)
					{
						height = image.size.height;
					}
				}
				
				DTTextAttachment *attachment = [[[DTTextAttachment alloc] init] autorelease];
				attachment.contents = image;
				attachment.size = CGSizeMake(width, height);

				CTRunDelegateRef embeddedObjectRunDelegate = createEmbeddedObjectRunDelegate(attachment);
				
				CTParagraphStyleRef paragraphStyle = createParagraphStyle(0, 0, 0, 0, 0);
				
				NSMutableDictionary *localAttributes = [NSMutableDictionary dictionaryWithObjectsAndKeys:attachment, @"DTTextAttachment",
														(id)embeddedObjectRunDelegate, kCTRunDelegateAttributeName, 
														(id)paragraphStyle, kCTParagraphStyleAttributeName, nil];
				CFRelease(embeddedObjectRunDelegate);
        CFRelease(paragraphStyle);
				
				id link = [currentTag objectForKey:@"DTLink"];
				if (link)
				{
					[localAttributes setObject:link forKey:@"DTLink"];
				}
				
				if (needsNewLineBefore)
				{
					if ([tmpString length] && ![[tmpString string] hasSuffix:@"\n"])
					{
						NSAttributedString *string = [[[NSAttributedString alloc] initWithString:@"\n" attributes:previousAttributes] autorelease];
						[tmpString appendAttributedString:string];
					}
					
					needsNewLineBefore = NO;
				}
				
				NSAttributedString *string = [[[NSAttributedString alloc] initWithString:UNICODE_OBJECT_PLACEHOLDER attributes:localAttributes] autorelease];
				[tmpString appendAttributedString:string];
			}
			else if ([tagName isEqualToString:@"video"] && tagOpen)
			{
				// hide contents of recognized tag
				[currentTag setObject:[NSNumber numberWithBool:YES] forKey:@"_tagContentsInvisible"];
				
				
				CGFloat width = [[tagAttributesDict objectForKey:@"width"] intValue];
				CGFloat height = [[tagAttributesDict objectForKey:@"height"] intValue];
				
				if (width==0 || height==0)
				{
					width = 300;
					height = 225;
				}
				
				DTTextAttachment *attachment = [[[DTTextAttachment alloc] init] autorelease];
				attachment.contents = currentTag;
				attachment.size = CGSizeMake(width, height);
				
				CTRunDelegateRef embeddedObjectRunDelegate = createEmbeddedObjectRunDelegate(attachment);
				
				CTParagraphStyleRef paragraphStyle = createParagraphStyle(0, 0, 0, 0, 0);
				
				NSMutableDictionary *localAttributes = [NSMutableDictionary dictionaryWithObjectsAndKeys:attachment, @"DTTextAttachment",
														(id)embeddedObjectRunDelegate, kCTRunDelegateAttributeName, 
														(id)paragraphStyle, kCTParagraphStyleAttributeName, nil];
				CFRelease(embeddedObjectRunDelegate);
        CFRelease(paragraphStyle);
				
				if (needsNewLineBefore)
				{
					if ([tmpString length] && ![[tmpString string] hasSuffix:@"\n"])
					{
						NSAttributedString *string = [[[NSAttributedString alloc] initWithString:@"\n" attributes:previousAttributes] autorelease];
						[tmpString appendAttributedString:string];
					}
					
					needsNewLineBefore = NO;
				}
				
				NSAttributedString *string = [[[NSAttributedString alloc] initWithString:UNICODE_OBJECT_PLACEHOLDER attributes:localAttributes] autorelease];
				[tmpString appendAttributedString:string];
				
			}
			else if ([tagName isEqualToString:@"a"])
			{
				if (tagOpen)
				{
					[currentTag setObject:[NSNumber numberWithInt:kCTUnderlineStyleSingle] forKey:@"UnderlineStyle"];
					[currentTagAttributes setObject:@"#0000EE" forKey:@"color"];
					
					// remove line breaks and whitespace in links
					NSString *cleanString = [[tagAttributesDict objectForKey:@"href"] stringByReplacingOccurrencesOfString:@"\n" withString:@""];
					cleanString = [cleanString stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
					
					NSURL *link = [NSURL URLWithString:cleanString];
					
					// deal with relative URL
					if (![link scheme])
					{
						link = [NSURL URLWithString:cleanString relativeToURL:baseURL];
					}
					
					
					if (link)
					{
						[currentTag setObject:link forKey:@"DTLink"];
					}
				}
			}
			else if ([tagName isEqualToString:@"b"] || [tagName isEqualToString:@"strong"])
			{
				currentFontDescriptor.boldTrait = YES;
			}
			else if ([tagName isEqualToString:@"i"] || [tagName isEqualToString:@"em"])
			{
				currentFontDescriptor.italicTrait = YES;
			}
			else if ([tagName isEqualToString:@"li"]) 
			{
				if (tagOpen)
				{
					needsListItemStart = YES;
					[currentTag setObject:[NSNumber numberWithFloat:0.0] forKey:@"ParagraphSpacing"];
					
					
					
#if ALLOW_IPHONE_SPECIAL_CASES						
					[currentTag setObject:[NSNumber numberWithFloat:25.0] forKey:@"HeadIndent"];
					[currentTag setObject:[NSArray arrayWithObjects:[NSNumber numberWithFloat:11.0],
										   [NSNumber numberWithFloat:25.0], nil] forKey:@"TabStops"];
#else
					[currentTag setObject:[NSNumber numberWithFloat:25.0] forKey:@"HeadIndent"];
					[currentTag setObject:[NSArray arrayWithObjects:[NSNumber numberWithFloat:11.0],
										   [NSNumber numberWithFloat:36.0], nil] forKey:@"TabStops"];
					
#endif
				}
				else 
				{
					needsListItemStart = NO;
					
					if (listCounter)
					{
						listCounter++;
					}
				}
				
			}
			else if ([tagName isEqualToString:@"left"])
			{
				if (tagOpen)
				{
					[currentTag setObject:[NSNumber numberWithInt:kCTLeftTextAlignment] forKey:@"TextAlignment"];
				}
#if ALLOW_IPHONE_SPECIAL_CASES
				else 
				{
					nextParagraphAdditionalSpaceBefore = 12.0;
				}
#endif
			}
			else if ([tagName isEqualToString:@"center"] && tagOpen)
			{
				[currentTag setObject:[NSNumber numberWithInt:kCTCenterTextAlignment] forKey:@"TextAlignment"];
#if ALLOW_IPHONE_SPECIAL_CASES						
				nextParagraphAdditionalSpaceBefore = 12.0;
#endif
			}
			else if ([tagName isEqualToString:@"right"] && tagOpen)
			{
				[currentTag setObject:[NSNumber numberWithInt:kCTRightTextAlignment] forKey:@"TextAlignment"];
#if ALLOW_IPHONE_SPECIAL_CASES						
				nextParagraphAdditionalSpaceBefore = 12.0;
#endif
			}
			else if ([tagName isEqualToString:@"del"]) 
			{
				if (tagOpen)
				{
					[currentTag setObject:[NSNumber numberWithBool:YES] forKey:@"_StrikeOut"];
				}
			}
			else if ([tagName isEqualToString:@"ol"]) 
			{
				if (tagOpen)
				{
					listCounter = 1;
				} 
				else 
				{
#if ALLOW_IPHONE_SPECIAL_CASES						
					nextParagraphAdditionalSpaceBefore = 12.0;
#endif
				}
			}
			else if ([tagName isEqualToString:@"ul"]) 
			{
				if (tagOpen)
				{
					listCounter = 0;
				}
				else 
				{
#if ALLOW_IPHONE_SPECIAL_CASES						
					nextParagraphAdditionalSpaceBefore = 12.0;
#endif
				}
			}

			else if ([tagName isEqualToString:@"u"])
			{
				if (tagOpen)
				{
					[currentTag setObject:[NSNumber numberWithInt:kCTUnderlineStyleSingle] forKey:@"UnderlineStyle"];
				}
			}
			else if ([tagName isEqualToString:@"sup"])
			{
				if (tagOpen)
				{
					[currentTag setObject:[NSNumber numberWithInt:1] forKey:@"Superscript"];
				}
			}
			else if ([tagName isEqualToString:@"sub"])
			{
				if (tagOpen)
				{
					[currentTag setObject:[NSNumber numberWithInt:-1] forKey:@"Superscript"];
				}
			}
			else if ([tagName hasPrefix:@"h"])
			{
				if (tagOpen)
				{
					NSInteger headerLevel = 0;
					NSScanner *scanner = [NSScanner scannerWithString:tagName];
					
					// Skip h
					[scanner scanString:@"h" intoString:NULL];
					
					if ([scanner scanInteger:&headerLevel])
					{
						[currentTag setObject:[NSNumber numberWithInteger:headerLevel] forKey:@"HeaderLevel"];	
						currentFontDescriptor.boldTrait = YES;
						
						switch (headerLevel) 
						{
							case 1:
							{
								[currentTag setObject:[NSNumber numberWithFloat:16.0] forKey:@"ParagraphSpacing"];
								currentFontDescriptor.pointSize = 24.0;
								break;
							}
							case 2:
							{
								[currentTag setObject:[NSNumber numberWithFloat:14.0] forKey:@"ParagraphSpacing"];	
								currentFontDescriptor.pointSize = 18.0;
								break;
							}
							case 3:
							{
								[currentTag setObject:[NSNumber numberWithFloat:14.0] forKey:@"ParagraphSpacing"];
								currentFontDescriptor.pointSize = 14.0;
								break;
							}
							case 4:
							{
								[currentTag setObject:[NSNumber numberWithFloat:15.0] forKey:@"ParagraphSpacing"];	
								currentFontDescriptor.pointSize = 12.0;
								break;
							}
							case 5:
							{
								[currentTag setObject:[NSNumber numberWithFloat:16.0] forKey:@"ParagraphSpacing"];	
								currentFontDescriptor.pointSize = 10.0;
								break;
							}
							case 6:
							{
								[currentTag setObject:[NSNumber numberWithFloat:20.0] forKey:@"ParagraphSpacing"];	
								currentFontDescriptor.pointSize = 9.0;
								break;
							}
							default:
								break;
						}
					}
					
					// First paragraph after a header needs a newline to not stick to header
					seenPreviousParagraph = NO;
				}
			}
			else if ([tagName isEqualToString:@"font"])
			{
				if (tagOpen)
				{
					NSInteger size = [[tagAttributesDict objectForKey:@"size"] intValue];
					
					switch (size) 
					{
						case 1:
							currentFontDescriptor.pointSize = 9.0;
							break;
						case 2:
							currentFontDescriptor.pointSize = 10.0;
							break;
						case 4:
							currentFontDescriptor.pointSize = 14.0;
							break;
						case 5:
							currentFontDescriptor.pointSize = 18.0;
							break;
						case 6:
							currentFontDescriptor.pointSize = 24.0;
							break;
						case 7:
							currentFontDescriptor.pointSize = 37.0;
							break;	
						case 3:
						default: {
                            NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
                            NSNumber *small = [defaults objectForKey:@"interface-small-text"];
                            if (small == nil || [small boolValue]) {
                                currentFontDescriptor.pointSize = 12.0;
                            } else {
                                currentFontDescriptor.pointSize = 14.0;
                            }
							
							break;
                        }
					}
					
					NSString *face = [tagAttributesDict objectForKey:@"face"];
					
					if (face)
					{
						currentFontDescriptor.fontFamily = face;
					}
				}
			}
			else if ([tagName isEqualToString:@"p"])
			{
                if (tagOpen)
				{
					//[currentTag setObject:[NSNumber numberWithFloat:12.0] forKey:@"ParagraphSpacing"];
                    nextParagraphAdditionalSpaceBefore = 12.0;

					seenPreviousParagraph = YES;
				}
				
			}
			else if ([tagName isEqualToString:@"br"])
			{
				immediatelyClosed = YES; 
			}
            else if ([tagName isEqualToString:@"pre"] || [tagName isEqualToString:@"code"])
            {
                if (tagOpen)
				{
					[currentTag setObject:[NSNumber numberWithFloat:12.0] forKey:@"ParagraphSpacing"];
                    [currentTag setObject:[NSNumber numberWithBool:YES] forKey:@"PreserveNewlines"];
                    currentFontDescriptor.fontFamily = @"Courier";
                    
					seenPreviousParagraph = YES;
				}
            }
			
			// convert CSS Styles into our own style
			[self remapCSSStylesOnTagDictionary:currentTag];
			
			
			// --------------------- push tag on stack if it's opening
			if (tagOpen&&!immediatelyClosed)
			{
				[tagStack addObject:currentTag];
			}
			else if (!tagOpen)
			{
				// block items have to have a NL at the end.
				if (![tagName isInlineTag] && ![[tmpString string] hasSuffix:@"\n"] && ![[tmpString string] hasSuffix:UNICODE_OBJECT_PLACEHOLDER])
				{
					// remove extra space
					previousAttributes = nil;
					
					NSAttributedString *string = [[[NSAttributedString alloc] initWithString:@"\n" attributes:previousAttributes] autorelease];
					[tmpString appendAttributedString:string];
				}
				
				needsNewLineBefore = NO;
				
				
				if ([tagStack count])
				{
					// check if this tag is indeed closing the currently open one
					NSDictionary *topStackTag = [tagStack lastObject];
					NSString *topTagName = [topStackTag objectForKey:@"_tag"];
					
					if ([tagName isEqualToString:topTagName])
					{
						[tagStack removeLastObject];
						currentTag = [tagStack lastObject];
					}
					else 
					{
						NSLog(@"Ignoring non-open tag %@", topTagName);
					}
					
				}
				else 
				{
					currentTag = nil;
				}
			}
			else if (immediatelyClosed)
			{
				// If it's immediately closed it's not relevant for following body
				currentTag = [tagStack lastObject];
			}
		}
		else 
		{
			//----------------------------------------- TAG CONTENTS -----------------------------------------
			NSString *tagContents = nil;
			
			if ([scanner scanUpToString:@"<" intoString:&tagContents] && ![[currentTag objectForKey:@"_tagContentsInvisible"] boolValue])
			{
				//				NSLog(@"tag: -%@-", tagContents);
				
				if ([[tagContents stringByTrimmingCharactersInSet:[NSCharacterSet newlineCharacterSet]] length])
				{
					tagContents = [tagContents stringByNormalizingWhitespace];
					tagContents = [tagContents stringByReplacingHTMLEntities];
					
					NSDictionary *currentTagAttributes = [currentTag objectForKey:@"Attributes"];
					DTCoreTextFontDescriptor *currentFontDescriptor = [currentTag objectForKey:@"FontDescriptor"];
					
					NSMutableDictionary *attributes = [NSMutableDictionary dictionary];
					
					NSNumber *superscriptStyle = [currentTag objectForKey:@"Superscript"];
					if ([superscriptStyle intValue])
					{
						[attributes setObject:superscriptStyle forKey:(id)kCTSuperscriptAttributeName];
					}
					
					NSArray *shadows = [currentTag objectForKey:@"_Shadows"];
					if (shadows)
					{
						[attributes setObject:shadows forKey:@"_Shadows"];
					}
					
					
					id runDelegate = [currentTag objectForKey:@"_RunDelegate"];
					if (runDelegate)
					{
						[attributes setObject:runDelegate forKey:(id)kCTRunDelegateAttributeName];
					}
					
					id link = [currentTag objectForKey:@"DTLink"];
					if (link)
					{
						[attributes setObject:link forKey:@"DTLink"];
					}

					// if we don't know a font name yet, find it
					if (!currentFontDescriptor.fontName)
					{
						[currentFontDescriptor normalize];
					}

					// create font
					NSDictionary *fontAttributes = [currentFontDescriptor fontAttributes];
					CTFontDescriptorRef fontDesc = CTFontDescriptorCreateWithAttributes((CFDictionaryRef)fontAttributes);
					CTFontRef font = CTFontCreateWithFontDescriptor(fontDesc, currentFontDescriptor.pointSize, NULL);
					CFRelease(fontDesc);
					
					CGFloat paragraphSpacing = [[currentTag objectForKey:@"ParagraphSpacing"] floatValue];
					CGFloat paragraphSpacingBefore = [[currentTag objectForKey:@"ParagraphSpacingBefore"] floatValue];
					
#if ALLOW_IPHONE_SPECIAL_CASES				
					if (tagOpen && ![tagName isInlineTag] && ![tagName isEqualToString:@"li"])
					{
						if (nextParagraphAdditionalSpaceBefore>0)
						{
							// FIXME: add extra space properly
							// this also works, but breaks UnitTest for lists
							tagContents = [UNICODE_LINE_FEED stringByAppendingString:tagContents];
							
							// this causes problems on the paragraph after a List
							//paragraphSpacingBefore += nextParagraphAdditionalSpaceBefore;
							nextParagraphAdditionalSpaceBefore = 0;
						}
					}
#endif
					
					
					CGFloat headIndent = [[currentTag objectForKey:@"HeadIndent"] floatValue];
					
					NSArray *tabStops = [currentTag objectForKey:@"TabStops"];
					
					
					NSNumber *textAlignmentNum = [currentTag objectForKey:@"TextAlignment"];
					CTTextAlignment textAlignment = kCTNaturalTextAlignment;
					
					if (textAlignmentNum)
					{
						textAlignment = [textAlignmentNum intValue];
					}
					
					CTParagraphStyleRef paragraphStyle = createParagraphStyle(paragraphSpacingBefore, paragraphSpacing, headIndent, tabStops, textAlignment);
					
					[attributes setObject:(id)font forKey:(id)kCTFontAttributeName];
					[attributes setObject:(id)paragraphStyle forKey:(id)kCTParagraphStyleAttributeName];

					CFRelease(font);
					CFRelease(paragraphStyle);
					
					NSString *fontColor = [currentTagAttributes objectForKey:@"color"];
					
					if (fontColor)
					{
						UIColor *color = [UIColor colorWithHTMLName:fontColor];
						
						if (color)
						{
							[attributes setObject:(id)[color CGColor] forKey:(id)kCTForegroundColorAttributeName];
						}
					}
					
					NSNumber *underlineStyle = [currentTag objectForKey:@"UnderlineStyle"];
					if (underlineStyle)
					{
						[attributes setObject:underlineStyle forKey:(id)kCTUnderlineStyleAttributeName];
					}
					
					NSNumber *strikeOut = [currentTag objectForKey:@"_StrikeOut"];
					
					if (strikeOut)
					{
						[attributes setObject:strikeOut forKey:@"_StrikeOut"];
					}
					
					// HTML ignores newlines
                    if (![[currentTag objectForKey:@"PreserveNewlines"] boolValue]) {
                        tagContents = [tagContents stringByReplacingOccurrencesOfString:@"\n" withString:@" "];
                    }
					
					if (needsListItemStart)
					{
						if (listCounter)
						{
							NSString *prefix = [NSString stringWithFormat:@"\x09%d.\x09", listCounter];
							
							tagContents = [prefix stringByAppendingString:tagContents];
						}
						else
						{
							// Ul li prefixes bullet
							tagContents = [@"\x09\u2022\x09" stringByAppendingString:tagContents];
						}
						
						needsListItemStart = NO;
					}
					
					
					// Add newline after block contents if a new block follows
					NSString *nextTag = [scanner peekNextTagSkippingClosingTags:YES];
					
					if ([nextTag isEqualToString:@"br"])
					{
						// Add linefeed
						tagContents = [tagContents stringByAppendingString:UNICODE_LINE_FEED];
					}
					else
					{
						
						// add paragraph break if this is the end of paragraph
						if (nextTag && ![nextTag isInlineTag])
						{
							if ([tagContents length])
							{
								tagContents = [tagContents stringByAppendingString:@"\n"];
							}
						}
					}
					
					if (needsNewLineBefore)
					{
						if ([tagContents hasPrefix:@" "])
						{
							tagContents = [tagContents substringFromIndex:1];
						}
						
						if ([tmpString length])
						{
							if (![[tmpString string] hasSuffix:@"\n"])
							{
								tagContents = [@"\n" stringByAppendingString:tagContents];
							}
						}
						needsNewLineBefore = NO;
					}
					else // might be a continuation of a paragraph, then we might need space before it
					{
						NSString *stringSoFar = [tmpString string];
						
						// prevent double spacing
						if ([stringSoFar hasSuffixCharacterFromSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]] && [tagContents hasPrefix:@" "])
						{
							tagContents = [tagContents substringFromIndex:1];
						}
					}
					
					NSAttributedString *tagString = [[NSAttributedString alloc] initWithString:tagContents attributes:attributes];
					[tmpString appendAttributedString:tagString];
					[tagString release];
					
					previousAttributes = attributes;
				}
				
			}
		}
		
	}
	
	return [self initWithAttributedString:tmpString];
}


@end

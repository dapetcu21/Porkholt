/*
 VFP math library for the iPhone / iPod touch
 
 Copyright (c) 2007-2008 Wolfgang Engel and Matthias Grundmann
 http://code.google.com/p/vfpmathlibrary/
 
 This software is provided 'as-is', without any express or implied warranty.
 In no event will the authors be held liable for any damages arising
 from the use of this software.
 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it freely,
 subject to the following restrictions:
 
 1. The origin of this software must not be misrepresented; you must
 not claim that you wrote the original software. If you use this
 software in a product, an acknowledgment in the product documentation
 would be appreciated but is not required.
 
 2. Altered source versions must be plainly marked as such, and must
 not be misrepresented as being the original software.
 
 3. This notice may not be removed or altered from any source distribution.
 */

#import "TestSuite.h"

@interface MethodInfo : NSObject {
  SEL method_;
  NSString* desc_;
}

- (id)initWithMethod:(SEL)method andDescription:(NSString*)desc;

@property (nonatomic, assign) SEL method;
@property (nonatomic, retain) NSString* desc;

@end

@implementation MethodInfo

@synthesize method = method_, desc = desc_;

- (id)initWithMethod:(SEL)method andDescription:(NSString*)desc {
  if (self = [super init]) {
    method_ = method;
    desc_ = [desc retain];
  }
  return self;
}

- (void)dealloc {
  [desc_ release];
  [super dealloc];
}

@end


@implementation TestSuite

- (id)initWithTextView:(UITextView*)text_view {
  if (self = [super init]) {
    output_ = [text_view retain];
    test_methods_ = [[NSMutableArray alloc] initWithCapacity:0];
    //test_desc_ = [[NSMutableArray alloc] initWithCapacity:0];
  }
  return self;
}

- (void)registerTest:(SEL)method showDescription:(NSString*)desc {
  
  MethodInfo* mi = [[[MethodInfo alloc] initWithMethod:method andDescription:desc] autorelease];
  [test_methods_ addObject:mi];
}

- (void)runTests {
  for (int i = 0; i < [test_methods_ count]; ++i) {
    MethodInfo* mi = (MethodInfo*) [test_methods_ objectAtIndex:i];
    BOOL result = (BOOL)[self performSelector: mi.method];
    NSString* desc = [mi.desc stringByPaddingToLength:35 withString:@" " startingAtIndex:0];
    NSString* result_string = (result == YES ? @"PASSED\n" : @"FAILED\n");

    output_.text = [output_.text stringByAppendingString: 
                       [desc stringByAppendingString:result_string]];
  }
}

- (void)dealloc {
  [output_ release];
  [test_methods_ release];
  [super dealloc];
}

@end

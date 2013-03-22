#import <Foundation/Foundation.h>
#include <stdarg.h>

extern "C" void PHNSLog(const char * str, va_list args)
{
    NSString * frm = [[NSString alloc] initWithUTF8String:str];
    NSString * s = [[NSString alloc] initWithFormat:frm arguments:args];
    NSLog(@"Porkholt: %@", s);
    [s release];
    [frm release];
}

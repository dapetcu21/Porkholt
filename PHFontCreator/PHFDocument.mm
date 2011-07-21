//
//  PHFDocument.m
//  PHFontCreator
//
//  Created by Marius Petcu on 7/18/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "PHFDocument.h"
#import "ImageView.h"
#include "PHSerialization.h"
#include <string.h>

@implementation PHFDocument

- (id)init
{
    self = [super init];
    if (self) {
        size_t size = 256;
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
        image = CGBitmapContextCreate(NULL, size, size, 8, size, colorSpace,kCGImageAlphaNone);
        size_t sizeHD = size<<1;
        imageHD = CGBitmapContextCreate(NULL, sizeHD, sizeHD, 8, sizeHD, colorSpace, kCGImageAlphaNone);
        CGColorSpaceRelease(colorSpace);
        context = [[NSGraphicsContext graphicsContextWithGraphicsPort:image flipped:NO] retain];
        contextHD = [[NSGraphicsContext graphicsContextWithGraphicsPort:imageHD flipped:NO] retain];
        NSGraphicsContext * cntx = [NSGraphicsContext currentContext];
        [NSGraphicsContext setCurrentContext:contextHD];
        NSAffineTransform * transform = [[NSAffineTransform alloc] init];
        [transform scaleBy:2];
        [transform set];
        [NSGraphicsContext setCurrentContext:cntx];
        loadedFromFile = NO;
    }
    return self;
}

- (void) awakeFromNib
{
    [imageView setBitmapContext:imageHD];
    [fontList removeAllItems];
    [fontList addItemsWithTitles:[[NSFontManager sharedFontManager] availableFonts]];
    if (!loadedFromFile)
    {
        [fontList selectItemWithTitle:@"ArialMT"];
        [self rebuildData:self];
    }
    else
    {
        [fontList selectItemWithTitle:loadedFontName];
        [fontSize setDoubleValue:loadedFontSize];
        [textField setStringValue:loadedDescriptorString];
        [loadedDescriptorString release];
        [loadedFontName release];
    }
}

- (void) dealloc
{
    [context release];
    [contextHD release];
    CGContextRelease(image);
    CGContextRelease(imageHD);
    [super dealloc];
}

- (NSString *)windowNibName
{
    // Override returning the nib file name of the document
    // If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
    return @"PHFDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)aController
{
    [super windowControllerDidLoadNib:aController];
    // Add any code here that needs to be executed once the windowController has loaded the document's window.
}

-(void)drawString:(NSString *)s onContext:(NSGraphicsContext*)c andCGContext:(CGContextRef)cgContext withFont:(NSFont*)font andSize:(size_t)size saveChars:(BOOL)save
{
    NSUInteger n = [s length];
    
    NSGraphicsContext * oldContext = [NSGraphicsContext currentContext];
    [NSGraphicsContext setCurrentContext:c];
   
    [[NSColor whiteColor] setFill];
    
    int st = 0;
    double position = size;
    
    NSMutableDictionary * attributes = [NSMutableDictionary dictionary];
    [attributes setValue:font forKey:NSFontAttributeName];
    [attributes setValue:[NSColor whiteColor] forKey:NSForegroundColorAttributeName];
    
    while (st<n)
    {
        int fp = st;
        double maxHeight = 0;
        double pointer = 0;
        NSSize sz = NSMakeSize(0, 0);
        NSString * ch;
        fp--;
        while (pointer+ceil(sz.width)+1 <= size)
        {
            if (sz.height>maxHeight)
                maxHeight = sz.height;
            if (fp>=st)
            {
                if (save)
                {
                    character chr;
                    chr.c = [ch UTF8String][0];
                    chr.minX = pointer / size;
                    chr.maxX = (pointer+sz.width) / (double)size;
                    chr.maxY = 1.0f - position / size;
                    chr.minY = 1.0f - (position-sz.height) / size;
                    chr.aspectRatio = sz.width/sz.height;
                    characters.push_back(chr);
                }
                [ch drawAtPoint:NSMakePoint(pointer, position-sz.height) withAttributes:attributes];
            }
            pointer+=ceil(sz.width)+1;
            fp++;
            if (fp>=n) break;
            ch = [s substringWithRange:NSMakeRange(fp, 1)];
            sz = [ch sizeWithAttributes:attributes];
        }
        position -= ceil(maxHeight);
        st = fp;
    }
    if (save)
    {
        NSSize sz = [@" " sizeWithAttributes:attributes];
        character chr;
        chr.c = ' ';
        chr.minX = 0;
        chr.maxX = 0;
        chr.maxY = 0;
        chr.minY = 0;
        chr.aspectRatio = sz.width/sz.height;
        characters.push_back(chr);
        
        sz = [@"\t" sizeWithAttributes:attributes];
        chr.c = '\t';
        chr.minX = 0;
        chr.maxX = 0;
        chr.maxY = 0;
        chr.minY = 0;
        chr.aspectRatio = sz.width/sz.height;
        characters.push_back(chr);
    }
    
    [NSGraphicsContext setCurrentContext:oldContext];
}

-(IBAction)rebuildData:(id)sender
{
    size_t size = CGBitmapContextGetHeight(image);
    size_t sizeHD = CGBitmapContextGetHeight(imageHD);
    NSString * fontName = [fontList titleOfSelectedItem];
    NSFont * font = [NSFont fontWithName:fontName size:[fontSize doubleValue]];
    NSString * string = [textField stringValue];
    
    CGContextClearRect(image, CGRectMake(0,0,size,size));
    CGContextClearRect(imageHD, CGRectMake(0,0,sizeHD,sizeHD));

    characters.clear();
    
    [self drawString:string onContext:context andCGContext:image withFont:font andSize:size saveChars:YES];
    [self drawString:string onContext:contextHD andCGContext:imageHD withFont:font andSize:size saveChars:NO];
    
    [imageView setNeedsDisplay:YES];
}

#define uint16From(d) (*((uint16_t*)(d)))
#define uint32From(d) (*((uint32_t*)(d)))
#define uint64From(d) (*((uint64_t*)(d)))
#define uint16At(d,p) uint16From((d)+(p))
#define uint32At(d,p) uint32From((d)+(p))
#define uint64At(d,p) uint64From((d)+(p))

- (void) saveImage:(CGContextRef)img at:(uint8_t*)d
{
    size_t w = CGBitmapContextGetWidth(img);
    size_t h = CGBitmapContextGetHeight(img);
    uint32At(d,0)=PHEncode((uint32_t)(w));
    uint32At(d,4)=PHEncode((uint32_t)(h));
    memcpy(d+8, CGBitmapContextGetData(img), w*h);
}

- (void) saveTextureCoordinateListAt:(uint8_t*)d withFontName:(NSString*)fontName
{
    size_t len = [fontName length];
    uint16At(d,0)=PHEncode((uint16_t)len);
    memcpy(d+2,[fontName UTF8String],len);
    uint64At(d,len+2)=PHEncodeF((double)[fontSize doubleValue]);
    size_t n = characters.size();
    uint32At(d,len+10)=PHEncode((uint32_t)n);
    uint8_t * p = d+len+14;
    for (size_t i=0; i<n; i++)
    {
        character & c = characters[i];
        *p = (uint8_t)c.c; p++;
        uint64From(p)=PHEncodeF((double)c.minX); p+=8;
        uint64From(p)=PHEncodeF((double)c.maxX); p+=8;
        uint64From(p)=PHEncodeF((double)c.minY); p+=8;
        uint64From(p)=PHEncodeF((double)c.maxY); p+=8;
        uint64From(p)=PHEncodeF((double)c.aspectRatio); p+=8;
    }
}

- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError {
    
    NSString * fontName = [fontList titleOfSelectedItem]; 
    size_t txCsize = 2 + [fontName length] + 8 + 4+(8*5+1)*characters.size();
    size_t imgsize = 2 * 4 + CGBitmapContextGetWidth(image)*CGBitmapContextGetHeight(image);
    size_t imgHDsize = 2 * 4 + CGBitmapContextGetWidth(imageHD)*CGBitmapContextGetHeight(imageHD);
    size_t headersize = 4+3*4;
    size_t totalsize = headersize+imgHDsize+imgsize+txCsize;
    uint8_t * d = (uint8_t*)malloc(totalsize);
    
    memcpy(d, "PRKF", 4);
    [self saveTextureCoordinateListAt:d+headersize withFontName:fontName];
    uint32At(d,4)=PHEncode((uint32_t)(headersize));
    [self saveImage:image at:d+headersize+txCsize];
    uint32At(d,8)=PHEncode((uint32_t)(headersize+txCsize));
    [self saveImage:imageHD at:d+headersize+txCsize+imgsize];
    uint32At(d,12)=PHEncode((uint32_t)(headersize+txCsize+imgsize));
    
    
    NSData * data = [NSData dataWithBytesNoCopy:d length:totalsize freeWhenDone:YES];
    return data;
}

- (BOOL)loadTextureCoordinateListFrom:(uint8_t*)d ofSize:(size_t)len
{
    if (len<2) return NO;
    size_t l = PHDecode(uint16At(d, 0));
    if (len<l+14) return NO;
    NSData * sdata = [NSData dataWithBytesNoCopy:d+2 length:l freeWhenDone:NO];
    loadedFontName = [[NSString alloc] initWithData:sdata encoding:NSUTF8StringEncoding];
    loadedFontSize = PHDecodeF(uint64At(d, l+2));
    size_t n = PHDecode(uint32At(d, l+10));
    if (len<l+14+n*(8*5+1)) return NO;
    uint8_t * p = d+l+14;
    
    loadedDescriptorString = [[NSMutableString alloc] initWithCapacity:n];
    
    for (int i=0; i<n; i++)
    {
        character tmp;
        tmp.c = (char)*p; p++;
        tmp.minX = PHDecodeF(uint64From(p)); p+=8;
        tmp.maxX = PHDecodeF(uint64From(p)); p+=8;
        tmp.minY = PHDecodeF(uint64From(p)); p+=8;
        tmp.maxY = PHDecodeF(uint64From(p)); p+=8;
        tmp.aspectRatio = PHDecodeF(uint64From(p)); p+=8;
        if (tmp.minX || tmp.maxX || tmp.minY || tmp.maxY)
            [loadedDescriptorString appendFormat:@"%c",tmp.c];
    }
    return YES;
}

- (BOOL)loadImage:(CGContextRef*)img andContext:(NSGraphicsContext**)c from:(uint8_t*)d ofSize:(size_t)len
{
    if (len<8) return NO;
    size_t width = PHDecode(uint32At(d,0));
    size_t height = PHDecode(uint32At(d,4));
    if (len<8+width*height) return NO;
    uint8_t * data = (uint8_t*)malloc(width*height);
    memcpy(data, d+8, width*height);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
    CGContextRef cgContext = CGBitmapContextCreateWithData(data, width, height, 8, width, colorSpace, kCGImageAlphaNone, /*ciobaneala*/ (CGBitmapContextReleaseDataCallback)free, data);
    CGColorSpaceRelease(colorSpace);
    if (!cgContext)
    {
        free(data);
        return NO;
    }
    NSGraphicsContext * gcontext = [[NSGraphicsContext graphicsContextWithGraphicsPort:cgContext flipped:NO] retain];
    if (!gcontext)
    {
        CGContextRelease(cgContext);
        return NO;
    }
    if (img!=&image)
    {
        NSGraphicsContext * cntx = [NSGraphicsContext currentContext];
        [NSGraphicsContext setCurrentContext:gcontext];
        NSAffineTransform * transform = [[NSAffineTransform alloc] init];
        [transform scaleXBy:(double)width/CGBitmapContextGetWidth(image) 
                        yBy:(double)height/CGBitmapContextGetHeight(image)];
        [transform set];
        [NSGraphicsContext setCurrentContext:cntx];
    }
    if (*c)
        [*c release];
    if (*img)
        CGContextRelease(*img);
    *img = cgContext;
    *c = gcontext;
    return YES;
}

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError {
    loadedFromFile = YES;
    uint8_t * d = (uint8_t*)[data bytes];
    size_t len = [data length];
    BOOL failed = false;
    
    if (len<16 || (strncmp((char*)d, "PRKF", 4)!=0))
        failed = YES;
    else
    {
        size_t txc = PHDecode(uint32At(d, 4));
        size_t img = PHDecode(uint32At(d, 8));
        size_t imgHD = PHDecode(uint32At(d, 12));
        failed = ![self loadTextureCoordinateListFrom:d+txc ofSize:len-txc];
        failed = failed || ![self loadImage:&image andContext:&context from:d+img ofSize:len-img];
        failed = failed || ![self loadImage:&imageHD andContext:&contextHD from:d+imgHD ofSize:len-imgHD];
    }
    
    if (failed)
    {
        if (outError) {
            *outError = [NSError errorWithDomain:@"com.phlabs.phfont" code:-4432 userInfo:[NSDictionary dictionaryWithObjectsAndKeys:@"Bad file format", NSLocalizedDescriptionKey, nil]];
        }
        return NO;
    }
    
    
    
    return YES;
}


-(IBAction)changeHD:(id)sender
{
    [imageView setBitmapContext:(([hdButton state]==NSOnState)?imageHD:image)];
}

@end

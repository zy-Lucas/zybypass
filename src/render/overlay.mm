#import "overlay.h"
#import "ascii_png.h"
#import "widgets_png.h"
#import <Cocoa/Cocoa.h>
#include <Foundation/Foundation.h>

static CGImageRef g_asciiTexture = NULL;
static CGFloat g_texW = 0.0f;
static CGFloat g_texH = 0.0f;
static CGFloat g_cellW = 0.0;
static CGFloat g_cellH = 0.0f;
static uint8_t g_glyphAdvances[128] = {0};
static NSMapTable<NSWindow *, NSView *> *g_overlayMap = nil;
static dispatch_source_t g_pollTimer = nil;
static CGImageRef g_buttonTexture = NULL;
static BOOL g_resourcesLoaded = NO;

static CGImageRef LoadPNGFromMemory(const uint8_t *bytes, size_t length)
{
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, bytes, length, NULL);
    if (!provider)
        return NULL;
    CGImageRef image = CGImageCreateWithPNGDataProvider(provider, NULL, NO, kCGRenderingIntentDefault);
    CFRelease(provider);
    return image;
}

static void DrawImageFlipped(CGContextRef cg, CGRect dst, CGImageRef image)
{
    CGContextSaveGState(cg);
    CGContextTranslateCTM(cg, dst.origin.x, dst.origin.y + dst.size.height);
    CGContextScaleCTM(cg, 1.0, -1.0);
    CGContextDrawImage(cg, CGRectMake(0, 0, dst.size.width, dst.size.height), image);
    CGContextRestoreGState(cg);
}

static void LoadResources(void)
{
    if (g_resourcesLoaded && g_asciiTexture && g_buttonTexture)
        return;

    if (!g_asciiTexture)
        g_asciiTexture = LoadPNGFromMemory(ascii_png, ascii_png_len);
    if (!g_buttonTexture)
        g_buttonTexture = LoadPNGFromMemory(widgets_png, widgets_png_len);

    if (!g_asciiTexture || !g_buttonTexture)
        return;

    g_texW = CGImageGetWidth(g_asciiTexture);
    g_texH = CGImageGetHeight(g_asciiTexture);
    g_cellW = g_texW / 16.0f;
    g_cellH = g_texH / 16.0f;

    CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
    for (int ch = 32; ch <= 126; ++ch)
    {
        int row = ch / 16;
        int col = ch % 16;

        CGRect cellRect = CGRectMake(col * g_cellW, row * g_cellH, g_cellW, g_cellH);
        CGImageRef cellImg = CGImageCreateWithImageInRect(g_asciiTexture, cellRect);
        if (!cellImg)
        {
            g_glyphAdvances[ch] = 8;
            continue;
        }

        size_t cw = CGImageGetWidth(cellImg);
        size_t chh = CGImageGetHeight(cellImg);
        size_t bpr = cw * 4;
        UInt8 *px = (UInt8 *)calloc(chh, bpr);
        int advance = 8;

        if (px)
        {
            CGContextRef bctx = CGBitmapContextCreate(px, cw, chh, 8, bpr, cs, kCGImageAlphaPremultipliedLast);
            if (bctx)
            {
                CGContextDrawImage(bctx, CGRectMake(0, 0, cw, chh), cellImg);
                int actualWidth = 0;
                for (int x = (int)cw - 1; x >= 0; --x)
                {
                    BOOL hasPixel = NO;
                    for (int y = 0; y < chh; ++y)
                        if (px[y * bpr + x * 4 + 3] > 10)
                        {
                            hasPixel = YES;
                            break;
                        }
                    if (hasPixel)
                    {
                        actualWidth = x + 1;
                        break;
                    }
                }
                advance = (int)(0.5 + actualWidth) + 1;
                if (advance < 1)
                    advance = 1;
                CGContextRelease(bctx);
            }
            free(px);
        }
        g_glyphAdvances[ch] = (uint8_t)advance;
        CGImageRelease(cellImg);
    }
    CGColorSpaceRelease(cs);
    g_resourcesLoaded = YES;
}

@interface OverlayView : NSView
{
    dispatch_source_t _timer;
    NSString *_scoreNum;
}
@end

@implementation OverlayView

- (BOOL)isOpaque
{
    return YES;
}
- (BOOL)isFlipped
{
    return YES;
}

+ (void)initialize
{
    [super initialize];
    LoadResources();
}

- (instancetype)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self)
    {
        _scoreNum = @"0";
#if !__has_feature(objc_arc)
        [_scoreNum retain];
#endif

        _timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
        dispatch_source_set_timer(_timer, DISPATCH_TIME_NOW, NSEC_PER_SEC / 60, 0);
        dispatch_source_set_event_handler(_timer, ^{
          [self onUpdate];
        });
        dispatch_resume(_timer);
    }
    return self;
}

- (void)dealloc
{
    if (_timer)
    {
        dispatch_source_cancel(_timer);
#if !__has_feature(objc_arc)
        dispatch_release(_timer);
#endif
    }
#if !__has_feature(objc_arc)
    [_scoreNum release];
    [super dealloc];
#endif
}

- (NSString *)generateRandomGarbage
{
    int len = 3 + arc4random_uniform(12);
    unichar buf[16];
    for (int i = 0; i < len; ++i)
        buf[i] = (unichar)(33 + arc4random_uniform(94));
    return [[NSString alloc] initWithCharacters:buf length:len];
}

- (void)onUpdate
{
#if !__has_feature(objc_arc)
    [_scoreNum release];
#endif
    _scoreNum = [self generateRandomGarbage];
#if !__has_feature(objc_arc)
    [_scoreNum retain];
#endif
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
    if (!g_asciiTexture)
        return;

    CGContextRef cg = [[NSGraphicsContext currentContext] CGContext];
    NSRect bounds = [self bounds];
    CGFloat w = bounds.size.width;
    CGFloat h = bounds.size.height;

    CGContextSaveGState(cg);

    CGFloat backingScale = [[self window] backingScaleFactor];

    CGFloat fbW = (int)(w * backingScale);
    CGFloat fbH = (int)(h * backingScale);

    CGFloat guiScale = 1;
    while (guiScale < 4 && fbW / (guiScale + 1) >= 320 && fbH / (guiScale + 1) >= 240)
    {
        ++guiScale;
    }

    CGFloat ratio = (CGFloat)guiScale / backingScale;
    CGContextScaleCTM(cg, ratio, ratio);

    CGFloat logicalW = w / ratio;
    CGFloat logicalH = h / ratio;

    CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
    CGFloat gradColors[] = {0.314f, 0.0f, 0.0f, 0.376f, 0.502f, 0.188f, 0.188f, 0.627f};
    CGGradientRef grad = CGGradientCreateWithColorComponents(rgb, gradColors, NULL, 2);
    CGContextDrawLinearGradient(cg, grad, CGPointMake(0, 0), CGPointMake(0, logicalH), 0);
    CGGradientRelease(grad);
    CGColorSpaceRelease(rgb);

    CGContextSetShouldAntialias(cg, NO);
    CGContextSetInterpolationQuality(cg, kCGInterpolationNone);

    CGContextSaveGState(cg);
    CGContextScaleCTM(cg, 2.0, 2.0);
    [self drawCenteredString:cg text:@"You Died!" x:int(logicalW / 4.0f) y:30.0f color:16777215 dropShadow:YES];
    CGContextRestoreGState(cg);

    [self drawCenteredString:cg text:@"You can't escape" x:int(logicalW / 2.0f) y:85.0f color:16777215 dropShadow:YES];

    NSString *scorePrefix = @"Score: ";
    NSString *scoreNum = _scoreNum ? _scoreNum : @"0";

    int prefixW = [self stringWidth:scorePrefix];
    int scoreTotalW = prefixW + [self stringWidth:scoreNum];
    int scoreX = (logicalW / 2.0f) - (scoreTotalW / 2.0f);

    [self drawInternal:cg text:scorePrefix x:scoreX y:100.0f color:16777215 dropShadow:YES];
    [self drawInternal:cg text:scoreNum x:scoreX + prefixW y:100.0f color:16777045 dropShadow:YES];

    int btnW = 200;
    int btnH = 20;
    int btnX = logicalW / 2.0f - btnW / 2.0f;
    int btnY1 = logicalH / 4 + 72.0f;
    int btnY2 = logicalH / 4.0f + 96.0f;

    [self drawButton:cg x:btnX y:btnY1 width:btnW height:btnH text:@"Respawn"];
    [self drawButton:cg x:btnX y:btnY2 width:btnW height:btnH text:@"Title screen"];

    CGContextRestoreGState(cg);
}

- (void)drawCenteredString:(CGContextRef)cg
                      text:(NSString *)text
                         x:(CGFloat)centerX
                         y:(CGFloat)y
                     color:(int)color
                dropShadow:(BOOL)dropShadow
{
    CGFloat textW = [self stringWidth:text];
    CGFloat x = centerX - textW / 2.0f;
    [self drawInternal:cg text:text x:x y:y color:color dropShadow:dropShadow];
}

- (void)drawInternal:(CGContextRef)cg
                text:(NSString *)text
                   x:(CGFloat)x
                   y:(CGFloat)y
               color:(int)color
          dropShadow:(BOOL)dropShadow
{
    if ((color & 0xFC000000) == 0)
        color |= 0xFF000000;
    if (dropShadow)
        [self renderText:cg text:text x:x y:y color:color dropShadow:YES dimFactor:0.25f];
    [self renderText:cg text:text x:x y:y color:color dropShadow:NO dimFactor:1.0f];
}

- (void)renderText:(CGContextRef)cg
              text:(NSString *)text
                 x:(CGFloat)x
                 y:(CGFloat)y
             color:(int)color
        dropShadow:(BOOL)dropShadow
         dimFactor:(float)dimFactor
{
    float r = ((color >> 16) & 0xFF) / 255.0f * dimFactor;
    float g = ((color >> 8) & 0xFF) / 255.0f * dimFactor;
    float b = ((color >> 0) & 0xFF) / 255.0f * dimFactor;
    float a = ((color >> 24) & 0xFF) / 255.0f;

    CGFloat offX = dropShadow ? 1.0f : 0.0f;
    CGFloat offY = dropShadow ? 1.0f : 0.0f;
    CGFloat cursorX = x;
    NSUInteger len = [text length];

    for (NSUInteger i = 0; i < len; ++i)
    {
        unichar ch = [text characterAtIndex:i];
        if (ch < 32 || ch > 126)
        {
            cursorX += 8.0f;
            continue;
        }

        int row = ch / 16;
        int col = ch % 16;
        CGFloat srcX = col * g_cellW;
        CGFloat srcY = row * g_cellH;

        uint8_t advance = g_glyphAdvances[ch];
        if (advance == 1)
            advance = 4;
        int actualWidth = advance - 1;

        if (actualWidth > 0)
        {
            CGFloat srcW = actualWidth * (g_cellW / 8.0f);
            if (srcW < 1.0f)
                srcW = g_cellW;

            CGRect srcRect = CGRectMake(srcX, srcY, srcW, g_cellH);
            CGRect dstRect = CGRectMake(cursorX + offX, y + offY, actualWidth, 8.0f);
            [self drawGlyph:cg src:srcRect dst:dstRect r:r g:g b:b a:a];
        }
        cursorX += advance;
    }
}

- (void)drawGlyph:(CGContextRef)cg src:(CGRect)srcRect dst:(CGRect)dstRect r:(float)r g:(float)g b:(float)b a:(float)a
{
    CGImageRef subImage = CGImageCreateWithImageInRect(g_asciiTexture, srcRect);
    if (!subImage)
        return;

    CGContextSaveGState(cg);
    CGContextTranslateCTM(cg, dstRect.origin.x, dstRect.origin.y + dstRect.size.height);
    CGContextScaleCTM(cg, 1.0, -1.0);

    CGRect localDst = CGRectMake(0, 0, dstRect.size.width, dstRect.size.height);
    CGContextClipToRect(cg, localDst);
    CGContextBeginTransparencyLayer(cg, NULL);
    CGContextDrawImage(cg, localDst, subImage);
    CGContextSetBlendMode(cg, kCGBlendModeSourceIn);
    CGContextSetRGBFillColor(cg, r, g, b, a);
    CGContextFillRect(cg, localDst);
    CGContextEndTransparencyLayer(cg);
    CGContextRestoreGState(cg);
    CGImageRelease(subImage);
}

- (CGFloat)stringWidth:(NSString *)text
{
    NSUInteger len = [text length];
    CGFloat width = 0.0f;
    for (NSUInteger i = 0; i < len; ++i)
    {
        unichar ch = [text characterAtIndex:i];
        if (ch < 32 || ch > 126)
            width += 8.0f;
        else
        {
            uint8_t adv = g_glyphAdvances[ch];
            if (adv == 1)
                adv = 4;
            width += (adv > 0) ? adv : 8.0f;
        }
    }
    return width;
}

- (void)drawButtonBackground:(CGContextRef)cg x:(CGFloat)x y:(CGFloat)y width:(CGFloat)width height:(CGFloat)height
{
    if (!g_buttonTexture)
        return;

    CGFloat bx = 20.0f, by = 4.0f;
    CGFloat srcW = CGImageGetWidth(g_buttonTexture);
    CGFloat srcH = CGImageGetHeight(g_buttonTexture);
    CGFloat midSrcW = srcW - bx * 2, midSrcH = srcH - by * 2;
    CGFloat midDstW = width - bx * 2, midDstH = height - by * 2;

    CGRect src[9] = {CGRectMake(0, 0, bx, by),
                     CGRectMake(bx, 0, midSrcW, by),
                     CGRectMake(srcW - bx, 0, bx, by),
                     CGRectMake(0, by, bx, midSrcH),
                     CGRectMake(bx, by, midSrcW, midSrcH),
                     CGRectMake(srcW - bx, by, bx, midSrcH),
                     CGRectMake(0, srcH - by, bx, by),
                     CGRectMake(bx, srcH - by, midSrcW, by),
                     CGRectMake(srcW - bx, srcH - by, bx, by)};
    CGRect dst[9] = {CGRectMake(x, y, bx, by),
                     CGRectMake(x + bx, y, midDstW, by),
                     CGRectMake(x + width - bx, y, bx, by),
                     CGRectMake(x, y + by, bx, midDstH),
                     CGRectMake(x + bx, y + by, midDstW, midDstH),
                     CGRectMake(x + width - bx, y + by, bx, midDstH),
                     CGRectMake(x, y + height - by, bx, by),
                     CGRectMake(x + bx, y + height - by, midDstW, by),
                     CGRectMake(x + width - bx, y + height - by, bx, by)};

    for (int i = 0; i < 9; ++i)
    {
        CGImageRef piece = CGImageCreateWithImageInRect(g_buttonTexture, src[i]);
        if (!piece)
            continue;
        DrawImageFlipped(cg, dst[i], piece);
        CGImageRelease(piece);
    }
}

- (void)drawButton:(CGContextRef)cg
                 x:(CGFloat)x
                 y:(CGFloat)y
             width:(CGFloat)width
            height:(CGFloat)height
              text:(NSString *)text
{
    [self drawButtonBackground:cg x:x y:y width:width height:height];
    [self drawCenteredString:cg text:text x:x + width / 2.0f y:y + 6.0f color:16777215 dropShadow:YES];
}

#define EMPTY_EVENT(name)                                                                                              \
    -(void)name : (NSEvent *)event {}
EMPTY_EVENT(mouseDown);
EMPTY_EVENT(rightMouseDown);
EMPTY_EVENT(otherMouseDown);
EMPTY_EVENT(mouseUp);
EMPTY_EVENT(rightMouseUp);
EMPTY_EVENT(otherMouseUp);
EMPTY_EVENT(mouseMoved);
EMPTY_EVENT(mouseDragged);
EMPTY_EVENT(scrollWheel);
EMPTY_EVENT(rightMouseDragged);
EMPTY_EVENT(otherMouseDragged);
EMPTY_EVENT(mouseEntered);
EMPTY_EVENT(mouseExited);
EMPTY_EVENT(keyDown);
EMPTY_EVENT(keyUp);
EMPTY_EVENT(flagsChanged);
EMPTY_EVENT(tabletPoint);
EMPTY_EVENT(tabletProximity);
EMPTY_EVENT(cursorUpdate);
#undef EMPTY_EVENT

- (BOOL)performKeyEquivalent:(NSEvent *)event
{
    return YES;
}

@end

static void AttachOverlayToWindow(NSWindow *win)
{
    NSView *contentView = [win contentView];
    if (!contentView)
        return;

    for (NSView *sub in [contentView subviews])
    {
        if ([sub isKindOfClass:[OverlayView class]])
            return;
    }

    NSRect frame = [contentView bounds];
    OverlayView *overlay = [[OverlayView alloc] initWithFrame:frame];
    overlay.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    overlay.wantsLayer = YES;
    overlay.layer.backgroundColor = [NSColor clearColor].CGColor;
    overlay.layer.contentsScale = [win backingScaleFactor];

    [contentView addSubview:overlay];
    [overlay setNeedsDisplay:YES];
    [g_overlayMap setObject:overlay forKey:win];
    [win makeFirstResponder:overlay];
}

static void InjectAllVisibleWindows(NSApplication *app)
{
    for (NSWindow *win in [app windows])
    {
        if (![win isVisible])
            continue;
        if ([g_overlayMap objectForKey:win])
            continue;
        AttachOverlayToWindow(win);
    }
}

void StartOverlay(void)
{
    @autoreleasepool
    {
        if (g_overlayMap)
            return;

        LoadResources();

        NSApplication *app = [NSApplication sharedApplication];
        if (!app)
            return;

        g_overlayMap = [NSMapTable mapTableWithKeyOptions:NSPointerFunctionsWeakMemory
                                             valueOptions:NSPointerFunctionsStrongMemory];
#if !__has_feature(objc_arc)
        [g_overlayMap retain];
#endif

        InjectAllVisibleWindows(app);

        g_pollTimer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
        dispatch_source_set_timer(g_pollTimer, DISPATCH_TIME_NOW, NSEC_PER_SEC / 2, 0);
        dispatch_source_set_event_handler(g_pollTimer, ^{
          InjectAllVisibleWindows(app);
        });
        dispatch_resume(g_pollTimer);
    }
}

void StopOverlay(void)
{
    @autoreleasepool
    {
        if (g_pollTimer)
        {
            dispatch_source_cancel(g_pollTimer);
#if !__has_feature(objc_arc)
            dispatch_release(g_pollTimer);
#endif
            g_pollTimer = nil;
        }

        if (g_overlayMap)
        {
            NSEnumerator *enumerator = [g_overlayMap objectEnumerator];
            NSView *overlay;
            while ((overlay = [enumerator nextObject]))
            {
                [overlay removeFromSuperview];
            }
            [g_overlayMap removeAllObjects];
#if !__has_feature(objc_arc)
            [g_overlayMap release];
#endif
            g_overlayMap = nil;
        }

        if (g_asciiTexture)
        {
            CGImageRelease(g_asciiTexture);
            g_asciiTexture = NULL;
        }
        if (g_buttonTexture)
        {
            CGImageRelease(g_buttonTexture);
            g_buttonTexture = NULL;
        }

        g_texW = g_texH = g_cellW = g_cellH = 0.0f;
        memset(g_glyphAdvances, 0, sizeof(g_glyphAdvances));
        g_resourcesLoaded = NO;
    }
}
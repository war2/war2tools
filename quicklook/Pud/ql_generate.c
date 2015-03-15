//
//  ql_generate.c
//
//  Created by Jean Guyomarc'h on 15/03/15.
//  Copyright (c) 2015 Jean Guyomarc'h. All rights reserved.
//

#include "ql_generate.h"


static CFDictionaryRef
_properties_new(const char *url_str,
                Pud        *pud)
{
    char *name;
    CFDictionaryRef dict;
    
    name = strrchr(url_str, '/') + 1;
    
    CFTypeRef keys[1] = {
        kQLPreviewPropertyDisplayNameKey
    };
    CFTypeRef data[1] = {
        CFStringCreateWithFormat(NULL, NULL, CFSTR("(%i) %s - %ix%i"),
                                 pud->starting_points, name, pud->map_w, pud->map_h)
    };
    
    dict = CFDictionaryCreate(NULL, (const void **)keys, (const void **)data, 1,
                              &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFRelease(data[0]);
    
    return dict;
}

static CGImageRef
_pud_to_image(Pud *pud)
{
    CGImageRef img;
    CGColorSpaceRef colorspace;
    CGDataProviderRef data_provider;
    unsigned char *map;
    int map_size;
    
    map = pud_minimap_bitmap_generate(pud, &map_size);
    if (NULL == map) {
        fprintf(stderr, "*** Failed to generate bitmap\n");
        return NULL;
    }
    
    colorspace = CGColorSpaceCreateDeviceRGB();
    data_provider = CGDataProviderCreateWithData(NULL, map, map_size, NULL);
    
    img = CGImageCreate(pud->map_w, pud->map_h, 8, 8 * 4, pud->map_w * 4, colorspace,
                        kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast,
                        data_provider, NULL, false, kCGRenderingIntentDefault);
    
    CGDataProviderRelease(data_provider);
    CGColorSpaceRelease(colorspace);
    free(map);
    
    return img;
}

OSStatus
ql_generate(CFURLRef              url,
            QLPreviewRequestRef   preview,
            QLThumbnailRequestRef thumbnail)
{
    Pud *pud;
    char path[2048];
    CGImageRef img;
    CGContextRef ctx;
    CGSize size;
    CGRect rect;
    CFDictionaryRef properties;
    Boolean chk;
    
    /* Safety checks */
    if (NULL == url) {
        fprintf(stderr, "*** Invalid URL parameter (must not be NULL)\n");
        return errAEParamMissed;
    }
    if (((NULL == preview) && (NULL == thumbnail)) ||
        ((NULL != preview) && (NULL != thumbnail))) {
        fprintf(stderr, "*** Parameters 'preview' and 'thumbnail' cannot be simulatenously NULL or NOT NULL\n");
        return errAEParamMissed;
    }
    
    /* Convert URL to C string */
    chk = CFURLGetFileSystemRepresentation(url, true, (UInt8 *)path, sizeof(path));
    if (FALSE == chk) {
        fprintf(stderr, "*** Failed to convert URL\n");
        goto abort;
    }

    /* Init libpud */
    if (false == pud_init()) {
        fprintf(stderr, "*** Failed to init libpud\n");
        goto abort;
    }
    
    if ((thumbnail && !QLThumbnailRequestIsCancelled(thumbnail)) ||
        (preview && !QLPreviewRequestIsCancelled(preview))) {
        
        /* Open PUD */
        pud = pud_open(path, PUD_OPEN_MODE_R);
        if (NULL == pud) {
            fprintf(stderr, "*** Failed to open PUD at path [%s]\n", path);
            goto shutdown;
        }
        
        /* Parse PUD */
        if (false == pud_parse(pud)) {
            fprintf(stderr, "*** Failed to parse PUD at path [%s]\n", path);
            goto close;
        }
        
        /* Generate bitmap */
        img = _pud_to_image(pud);
        if (NULL == img) {
            fprintf(stderr, "*** Failed to generate image for PUD at path [%s]\n", path);
            goto close;
        }
        
        /* Generate properties */
        properties = _properties_new(path, pud);
        
        if (NULL != preview) { /*=== Preview Mode ===*/
            
            size.height = pud->map_h;
            size.width = pud->map_w;
            ctx = QLPreviewRequestCreateContext(preview, size, true, properties);
            
            rect.origin = CGPointZero;
            rect.size = size;
            CGContextDrawImage(ctx, rect, img);
            
            QLPreviewRequestFlushContext(preview, ctx);
            
            CGContextRelease(ctx);
            
            
        } else { /*=== Thumbnail Mode ===*/
            QLThumbnailRequestSetImage(thumbnail, img, properties);
        }
        
        /* Dealloc */
        CFRelease(properties);
        CGImageRelease(img);
        pud_close(pud);
    }
    
    pud_shutdown();
    
    return noErr;
    
close:
    pud_close(pud);
shutdown:
    pud_shutdown();
abort:
    return errAborted;
}


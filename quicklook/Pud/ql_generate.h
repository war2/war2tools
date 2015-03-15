//
//  ql_generate.h
//
//  Created by Jean Guyomarc'h on 15/03/15.
//  Copyright (c) 2015 Jean Guyomarc'h. All rights reserved.
//

#ifndef __Pud__ql_generate__
#define __Pud__ql_generate__

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "pud.h"

OSStatus ql_generate(CFURLRef url, QLPreviewRequestRef preview, QLThumbnailRequestRef thumbnail);


#endif /* defined(__Pud__ql_generate__) */

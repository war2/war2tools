#include "ql_generate.h"

OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options);
void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview);

/* -----------------------------------------------------------------------------
   Generate a preview for file

   This function's job is to create preview for designated file
   ----------------------------------------------------------------------------- */

OSStatus
GeneratePreviewForURL(void                *thisInterface,
                      QLPreviewRequestRef  preview,
                      CFURLRef             url,
                      CFStringRef          contentTypeUTI,
                      CFDictionaryRef      options)
{
    return ql_generate(url, preview, NULL);
}

void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview)
{
    // Implement only if supported
}

#ifndef _FB_H_
#define _FB_H_
#include "efi.h"

struct FrameBufferInfo {
  UINTN FrameBufferBase;
  UINTN FrameBufferSize;
  UINT32 HorizontalResolution;
  UINT32 VerticalResolution;
};

void FBInit(struct FrameBufferInfo *fbInfo);

#endif

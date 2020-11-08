#include "efi.h"
#include "fb.h"
#include "common.h"

void FBInit(struct FrameBufferInfo *fbInfo){
  fbInfo->HorizontalResolution = GOP->Mode->Info->HorizontalResolution;
  fbInfo->VerticalResolution = GOP->Mode->Info->VerticalResolution;
  fbInfo->FrameBufferBase = GOP->Mode->FrameBufferBase;
  fbInfo->FrameBufferSize = GOP->Mode->FrameBufferSize;
}

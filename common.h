#ifndef _COMMON_H
#define _COMMON_H
#include "efi.h"

#define NULL (void *)0
void DrawRect(unsigned int UpperLeftX, unsigned int UpperLeftY,
              unsigned int width, unsigned int height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);
void DrawBox(unsigned int UpperLeftX, unsigned int UpperLeftY,
             unsigned int width, unsigned int height,
             EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);
#endif

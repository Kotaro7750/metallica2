#include "common.h"
#include "efi.h"

void DrawPixel(unsigned int x, unsigned int y,
               EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color) {
  UINT32 hr = GOP->Mode->Info->HorizontalResolution;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *base = GOP->Mode->FrameBufferBase;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *pixel = base + hr * y + x;

  pixel->Red = color->Red;
  pixel->Green = color->Green;
  pixel->Blue = color->Blue;
  pixel->Reserved = color->Reserved;
}

void DrawRect(unsigned int UpperLeftX, unsigned int UpperLeftY,
              unsigned int width, unsigned int height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color) {
  for (int i = 0; i < width; i++) {
    DrawPixel(UpperLeftX + i, UpperLeftY, color);
    DrawPixel(UpperLeftX + i, UpperLeftY + height - 1, color);
  }

  for (int i = 0; i < height; i++) {
    DrawPixel(UpperLeftX, UpperLeftY + i, color);
    DrawPixel(UpperLeftX + width - 1, UpperLeftY + i, color);
  }
}

void DrawBox(unsigned int UpperLeftX, unsigned int UpperLeftY,
             unsigned int width, unsigned int height,
             EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color) {
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      DrawPixel(UpperLeftX + i, UpperLeftY + j, color);
    }
  }
}

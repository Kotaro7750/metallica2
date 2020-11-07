#include "common.h"
#include "efi.h"

void assert(unsigned long long status, CHAR16 *errorStr) {
  if (status) {
    puts(errorStr);
    puts(L" status code:");
    puth(status, 10);
    while (1) {
    }
  }
}

void PrintLOGO() {
  puts(L"                #             ##     ##      #                   "
       L"###\n\r");
  puts(L"                #              #      #                         #   "
       L"#\n\r");
  puts(L" ## #    ###   ####    ###     #      #     ##     ###    ###       "
       L"#\n\r");
  puts(L" # # #  #   #   #         #    #      #      #    #   #      #    "
       L"##\n\r");
  puts(L" # # #  #####   #      ####    #      #      #    #       ####   "
       L"#\n\r");
  puts(
      L" # # #  #       #  #  #   #    #      #      #    #   #  #   #  #\n\r");
  puts(L" #   #   ###     ##    ####   ###    ###    ###    ###    ####  "
       L"#####\n\r\n\r\n\r");
}

void putc(CHAR16 c) {
  unsigned short str[2] = L" ";
  str[0] = c;
  ST->ConOut->OutputString(ST->ConOut, str);
}

void puts(CHAR16 *str) { ST->ConOut->OutputString(ST->ConOut, str); }

void puth(unsigned long long val, int digits) {
  unsigned short str[MAX_STR_BUF];
  unsigned long long remainder;

  for (int i = digits - 1; i >= 0; i--) {
    remainder = val % 16;
    val = (val - remainder) / 16;
    if (remainder >= 10) {
      str[i] = L'A' + remainder - 10;
    } else {
      str[i] = L'0' + remainder;
    }
  }

  str[digits] = L'\0';
  puts(str);
}

void putd(unsigned long long val, int digits) {
  unsigned short str[MAX_STR_BUF];
  unsigned long long remainder;

  for (int i = digits - 1; i >= 0; i--) {
    remainder = val % 10;
    val = (val - remainder) / 10;
    str[i] = L'0' + remainder;
  }

  str[digits] = L'\0';
  puts(str);
}

void putparam(unsigned long long val, CHAR16 *name, int digits) {
  puts(name);
  puts(L":");
  puth(val, digits);
  puts(L"\n\r");
}

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
  for (unsigned int i = 0; i < width; i++) {
    DrawPixel(UpperLeftX + i, UpperLeftY, color);
    DrawPixel(UpperLeftX + i, UpperLeftY + height - 1, color);
  }

  for (unsigned int i = 0; i < height; i++) {
    DrawPixel(UpperLeftX, UpperLeftY + i, color);
    DrawPixel(UpperLeftX + width - 1, UpperLeftY + i, color);
  }
}

void DrawBox(unsigned int UpperLeftX, unsigned int UpperLeftY,
             unsigned int width, unsigned int height,
             EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color) {
  for (unsigned int i = 0; i < width; i++) {
    for (unsigned int j = 0; j < height; j++) {
      DrawPixel(UpperLeftX + i, UpperLeftY + j, color);
    }
  }
}

void Char8Buf2Char16Buf(CHAR8 *from, CHAR16 *to, int from_size) {
  for (int i = 0; i < from_size; i++) {
    ((CHAR8 *)to)[i * 2 + 1] = 0;
    ((CHAR8 *)to)[i * 2] = from[i];
  }
}

unsigned long long atoull16(CHAR8 *buf, int size) {
  CHAR8 c;
  unsigned long long num = 0;

  for (int i = 0; i < size; i++) {
    num = num << 4;
    c = buf[i];
    if ('0' <= c && c <= '9') {
      num += (c - '0');
    } else if ('a' <= c && c <= 'f') {
      num += (c - 'a' + 10);
    } else {
      assert(1, L"not a hexadecimal character\n\r");
    }
  }

  return num;
}

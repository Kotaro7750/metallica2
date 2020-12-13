#ifndef _COMMON_H
#define _COMMON_H
#include "efi.h"

#define NULL (void *)0
#define MAX_STR_BUF 1024

void assert(unsigned long long status, CHAR16 *errorStr);
void PrintLOGO();
void puts(CHAR16 *str);
void puth(unsigned long long val, int digits);
void putd(unsigned long long val, int digits);
void putc(CHAR16 c);
void putparam(unsigned long long val, CHAR16 *name, int digits);
void DrawRect(unsigned int UpperLeftX, unsigned int UpperLeftY, unsigned int width, unsigned int height, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);
void DrawBox(unsigned int UpperLeftX, unsigned int UpperLeftY, unsigned int width, unsigned int height, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);

void Char8Buf2Char16Buf(CHAR8 *from, CHAR16 *to, int from_size);
unsigned long long atoull16(CHAR8 *buf, int size);
#endif

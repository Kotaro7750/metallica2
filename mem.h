#ifndef _MEM_H_
#define _MEM_H_
#include "efi.h"

#define MEMMAP_BUFFER_SIZE 3500

void GetMemoryMap();
void DumpMemoryMap(EFI_MEMORY_DESCRIPTER *memoryMap, UINTN descriptorSize,
                   int descriptorNum);

#endif

#ifndef _MEM_H_
#define _MEM_H_
#include "efi.h"

#define MEMMAP_BUFFER_SIZE 3500

struct MemoryMapInfo {
  UINTN MemoryMapSize;
  UINTN DescriptorSize;
  EFI_MEMORY_DESCRIPTER *MemoryMap;
  UINTN MapKey;
  UINT32 DescriptorVersion;
};

struct FreeMapInfo {
  UINT64 FreeMapBase;
  UINT64 FreeMapSize;
};

EFI_MEMORY_DESCRIPTER memoryMap[MEMMAP_BUFFER_SIZE / sizeof(EFI_MEMORY_DESCRIPTER)];
struct MemoryMapInfo memoryMapInfo;

void GetMemoryMap();
UINT64 InitPhysicalMemoryFreeMap(UINT64 freeMapBase);
void FreeUsablePagesOnPhysicalMemoryFreeMap(UINT64 freeMapBase, UINT64 freeMapSize);
void SetAllocatedContinuousRegionOnPhysicalFreeMap(UINT64 start, UINT64 end, UINT64 freeMapBase, UINT64 freeMapSize);
void DumpMemoryMap();

#endif

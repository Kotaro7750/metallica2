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

#define PG_ALLOCATED (1 << 0)
#define PG_RESERVED (1 << 1)

struct Page {
  UINT8 flags;
  UINT64 private;
  struct Page *prev;
  struct Page *next;
};

EFI_MEMORY_DESCRIPTER memoryMap[MEMMAP_BUFFER_SIZE / sizeof(EFI_MEMORY_DESCRIPTER)];
struct MemoryMapInfo memoryMapInfo;

void GetMemoryMap();
void CopyMemory(void *dst, void *src, UINTN size);
UINT64 InitPhysicalMemoryFreeMap(UINT64 freeMapBase);
void ReserveUnusablePagesOnPhysicalMemoryFreeMap(UINT64 freeMapBase, UINT64 freeMapSize);
void DumpMemoryMap();

#endif

#include "mem.h"
#include "common.h"
#include "efi.h"

void GetMemoryMap() {
  puts(L"GetMemoryMap start\r\n");

  UINTN status;
  UINTN memoryMapSize = MEMMAP_BUFFER_SIZE;
  EFI_MEMORY_DESCRIPTER
  memoryMap[MEMMAP_BUFFER_SIZE / sizeof(EFI_MEMORY_DESCRIPTER)];
  UINTN mapKey;
  UINTN descriptorSize;
  UINT32 descriptorVersion;

  status = ST->BootServices->GetMemoryMap(
      &memoryMapSize, (EFI_MEMORY_DESCRIPTER *)memoryMap, &mapKey,
      &descriptorSize, &descriptorVersion);
  assert(status, L"GetMemoryMap");

  puts(L"GetMemoryMap end\r\n");
}

void DumpMemoryMap(EFI_MEMORY_DESCRIPTER *memoryMap, UINTN descriptorSize,
                   int descriptorNum) {
  puts(L"DumpMemoryMap start\n\r");
  puts(L"Type,PhysicalStart,VirtualStart,NumberOfPages,Attribute\r\n");

  for (int i = 0; i < descriptorNum; i++) {
    UINT32 type = (EFI_MEMORY_DESCRIPTER *)memoryMap->Type;
    UINT64 physicalStart = (EFI_MEMORY_DESCRIPTER *)memoryMap->PhysicalStart;
    UINT64 virtualStart = (EFI_MEMORY_DESCRIPTER *)memoryMap->VirtualStart;
    UINT64 numberOfPages = (EFI_MEMORY_DESCRIPTER *)memoryMap->NumberOfPages;
    UINT64 attribute = (EFI_MEMORY_DESCRIPTER *)memoryMap->Attribute;

    putd(i, 2);
    puts(L":");
    puth(type, 1);
    puts(L",");
    puth(physicalStart, 10);
    puts(L",");
    puth(virtualStart, 10);
    puts(L",");
    puth(numberOfPages, 5);
    puts(L",");
    puth(attribute, 5);
    puts(L"\n\r");

    memoryMap = ((unsigned char *)memoryMap) + descriptorSize;
  }

  puts(L"DumpMemoryMap end\n\r");
}

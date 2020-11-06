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

  putparam(descriptorSize, L"Descriptor size", 10);

  int descriptorNum = memoryMapSize / descriptorSize;
  putparam(descriptorNum, L"Descriptor num", 10);

  puts(L"GetMemoryMap end\r\n");
}

void DumpMemoryMap(EFI_MEMORY_DESCRIPTER *memoryMap, int size) {}

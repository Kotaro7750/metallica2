#include "mem.h"
#include "common.h"
#include "efi.h"

void GetMemoryMap() {
  UINTN status;
  UINTN memoryMapSize = MEMMAP_BUFFER_SIZE;
  UINTN mapKey;
  UINTN descriptorSize;
  UINT32 descriptorVersion;

  status = ST->BootServices->GetMemoryMap(&memoryMapSize, (EFI_MEMORY_DESCRIPTER *)memoryMap, &mapKey, &descriptorSize, &descriptorVersion);
  assert(status, L"GetMemoryMap");

  memoryMapInfo.MemoryMapSize = memoryMapSize;
  memoryMapInfo.MemoryMap = memoryMap;
  memoryMapInfo.DescriptorSize = descriptorSize;
  memoryMapInfo.DescriptorVersion = descriptorVersion;
  memoryMapInfo.MapKey = mapKey;
}

void CopyMemory(void *dst, void *src, UINTN size) {
  char *src_char = src;
  char *dst_char = dst;

  for (UINTN i = 0; i < size; i++) {
    dst_char[i] = src_char[i];
  }
}

// NOT return valid page number
// range considering memory hole
UINT64 CalculateTotalPageRange() {
  GetMemoryMap();

  // MemoryMapSize holds the size of the entire memory map in byte unit
  UINTN descriptorNumber = memoryMapInfo.MemoryMapSize / memoryMapInfo.DescriptorSize;

  EFI_MEMORY_DESCRIPTER *_memoryMap = memoryMapInfo.MemoryMap;

  UINT64 maxPhysicalStart = 0;
  UINT64 pageNumberOfMaxPhysicalStart = 0;

  // get valid physical address range and calc total 4KB page number of this system
  for (int i = 0; i < descriptorNumber; i++) {
    if ((EFI_MEMORY_DESCRIPTER *)_memoryMap->PhysicalStart > maxPhysicalStart) {
      maxPhysicalStart = (EFI_MEMORY_DESCRIPTER *)_memoryMap->PhysicalStart;
      pageNumberOfMaxPhysicalStart = (EFI_MEMORY_DESCRIPTER *)_memoryMap->NumberOfPages;
    }

    _memoryMap = ((unsigned char *)_memoryMap) + memoryMapInfo.DescriptorSize;
  }

  // UEFI specification determins that a page is 4KiB
  UINT64 totalPageNumber = (maxPhysicalStart >> 12) + pageNumberOfMaxPhysicalStart;

  return totalPageNumber;
}

// fill 0 to freeMap and return the size of freeMap
UINT64 InitPhysicalMemoryFreeMap(UINT64 freeMapBase) {
  UINT64 totalPageRange = CalculateTotalPageRange();

  // since 1 bit per page, freeMapSize(byte) can be calc by divide by 8
  // + 1 for considering remainder
  UINT64 freeMapSize = totalPageRange / 8 + 1;

  // all pages initialized to allocated
  ST->BootServices->SetMem((void *)freeMapBase, freeMapSize, 0);

  return freeMapSize;
}

// Set pageIndex'th bit of FreeMap to 1
// TODO validatge pageIndex
void FreeSinglePageOnPhysicalMemoryFreeMap(UINT64 pageIndex, UINT64 freeMapBase, UINT64 freeMapSize) {
  // ex. pageIndex:8 -> byteIndex:1,bitIndex:7
  UINT64 byteIndex = pageIndex / 8;
  UINT64 bitIndex = 8 - (pageIndex % 8) - 1;

  unsigned char setByte = 1 << bitIndex;
  ((char *)freeMapBase)[byteIndex] |= setByte;
}

void SetAllocatedSinglePageOnPhysicalMemoryFreeMap(UINT64 pageIndex, UINT64 freeMapBase, UINT64 freeMapSize) {
  // ex. pageIndex:8 -> byteIndex:1,bitIndex:7
  unsigned long long byteIndex = pageIndex / 8;
  unsigned long long bitIndex = 8 - (pageIndex % 8) - 1;

  unsigned char inversionByte = 1 << bitIndex;

  // check if free
  if (((unsigned char *)freeMapBase)[byteIndex] & inversionByte) {
    ((unsigned char *)freeMapBase)[byteIndex] ^= inversionByte;
  }
}

// 使用可能な領域を解放する
void FreeUsablePagesOnPhysicalMemoryFreeMap(UINT64 freeMapBase, UINT64 freeMapSize) {
  // TODO 何回もやる必要があるのか？
  // GetMemoryMap();

  UINTN descriptorSize = memoryMapInfo.DescriptorSize;
  UINTN descriptorNum = memoryMapInfo.MemoryMapSize / descriptorSize;
  EFI_MEMORY_DESCRIPTER *_memoryMap = (EFI_MEMORY_DESCRIPTER *)memoryMap;

  // free usable pages by reading MemoryMap
  for (int i = 0; i < descriptorNum; i++) {
    UINT32 type = _memoryMap->Type;

    switch ((EFI_MEMORY_TYPE)type) {
    // unusable type
    case EfiReservedMemoryType:
    case EfiRuntimeServicesCode:
    case EfiRuntimeServicesData:
    case EfiUnusableMemory:
    case EfiACPIReclaimMemory:
    case EfiACPIMemoryNVS:
    case EfiMemoryMappedIO:
    case EfiMemoryMappedIOPortSpace:
    case EfiPalCode:
    case EfiMaxMemoryType:
      break;
    // usable type
    case EfiLoaderCode:
    case EfiLoaderData:
    case EfiBootServicesCode:
    case EfiBootServicesData:
    case EfiConventionalMemory:
    case EfiPersistentMemory: {
      UINT64 physicalPageStart = _memoryMap->PhysicalStart >> 12;
      UINT64 pageNumber = _memoryMap->NumberOfPages;

      if (_memoryMap->PhysicalStart == 0) {
        break;
      }

      for (unsigned long long pageIndex = physicalPageStart; pageIndex < physicalPageStart + pageNumber; pageIndex++) {
        // exclude pci hole
        // TODO magic number
        if (0xc0000 <= pageIndex && pageIndex < 0x100000) {
          continue;
        } else {
          FreeSinglePageOnPhysicalMemoryFreeMap(pageIndex, freeMapBase, freeMapSize);
        }
      }
      break;
    }
    }

    _memoryMap = ((unsigned char *)_memoryMap) + descriptorSize;
  }
}

// 連続した領域を割当済みとする
void SetAllocatedContinuousRegionOnPhysicalFreeMap(UINT64 start, UINT64 end, UINT64 freeMapBase, UINT64 freeMapSize) {
  UINT64 pageNumberStart = start >> 12;
  UINT64 pageNumberEnd = end >> 12;

  for (int pageIndex = pageNumberStart; pageIndex <= pageNumberEnd; pageIndex++) {
    SetAllocatedSinglePageOnPhysicalMemoryFreeMap(pageIndex, freeMapBase, freeMapSize);
  }
}

// GetMemoryMap() must be called before this function is called
void DumpMemoryMap() {
  puts(L"DumpMemoryMap start\n\r");
  puts(L"Type,PhysicalStart,VirtualStart,NumberOfPages,Attribute\r\n");

  UINTN descriptorSize = memoryMapInfo.DescriptorSize;
  UINTN descriptorNum = memoryMapInfo.MemoryMapSize / descriptorSize;
  EFI_MEMORY_DESCRIPTER *_memoryMap = (EFI_MEMORY_DESCRIPTER *)memoryMap;

  UINT64 totalPageNumber = 0;
  for (int i = 0; i < descriptorNum; i++) {
    UINT32 type = _memoryMap->Type;
    UINT64 physicalStart = _memoryMap->PhysicalStart;
    UINT64 virtualStart = _memoryMap->VirtualStart;
    UINT64 numberOfPages = _memoryMap->NumberOfPages;
    UINT64 attribute = _memoryMap->Attribute;

    totalPageNumber += numberOfPages;

    putd(i, 2);
    puts(L":");

    switch ((EFI_MEMORY_TYPE)type) {
    case EfiReservedMemoryType:
    case EfiUnusableMemory:
    case EfiPalCode:
    case EfiMaxMemoryType:
      puts(L"Reserved");
      break;
    case EfiLoaderCode:
      puts(L"LoarderCode");
      break;
    case EfiLoaderData:
      puts(L"LoarderData");
      break;
    case EfiBootServicesCode:
      puts(L"BS_Code");
      break;
    case EfiBootServicesData:
      puts(L"BS_Data");
      break;
    case EfiRuntimeServicesCode:
      puts(L"RT_Code");
      break;
    case EfiRuntimeServicesData:
      puts(L"RT_Data");
      break;
    case EfiConventionalMemory:
      puts(L"Available");
      break;
    case EfiPersistentMemory:
      puts(L"Persistent");
      break;
    case EfiACPIReclaimMemory:
      puts(L"ACPI_Recl");
      break;
    case EfiACPIMemoryNVS:
      puts(L"ACPI_NVS");
      break;
    case EfiMemoryMappedIO:
    case EfiMemoryMappedIOPortSpace:
      puts(L"MMIO");
      break;
    }
    puts(L",");

    puth(physicalStart, 12);
    puts(L",");

    puth(virtualStart, 12);
    puts(L",");

    puth(numberOfPages, 10);
    puts(L",");

    puth(attribute, 5);
    puts(L"\n\r");

    _memoryMap = ((unsigned char *)_memoryMap) + descriptorSize;
  }

  putparam(totalPageNumber, L"Total Page Number", 10);
  puts(L"\n\r");

  puts(L"DumpMemoryMap end\n\r");
}

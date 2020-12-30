#include "efi.h"
#include "common.h"
#include "mem.h"

EFI_SYSTEM_TABLE *ST;
EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SFSP;

void EFIInit(EFI_SYSTEM_TABLE *SystemTable) {
  ST = SystemTable;

  EFI_GUID GopGUID = {0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}};
  EFI_GUID SfspGUID = {0x0964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

  ST->BootServices->LocateProtocol(&GopGUID, NULL, (void **)&GOP);
  ST->BootServices->LocateProtocol(&SfspGUID, NULL, (void **)&SFSP);
}

UINT8 IsSameGUID(EFI_GUID a, EFI_GUID b) {
  if (a.Data1 == b.Data1 && a.Data2 == b.Data2 && a.Data3 == b.Data3) {
    for (int i = 0; i < 8; i++) {
      if (a.Data4[i] != b.Data4[i]) {
        return 0;
      }
    }
  } else {
    return 0;
  }

  return 1;
}

void *FindRSDPAddress() {
  UINTN configutaionTableEntryNumber = ST->NumberOfTableEntries;
  EFI_CONFIGURATION_TABLE *configurationTable = ST->ConfigurationTable;

  EFI_GUID ACPITableGUID = {0x8868e871, 0xe4f1, 0x11d3, {0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81}};

  for (UINTN i = 0; i < configutaionTableEntryNumber; i++) {
    EFI_GUID guid = configurationTable[i].VendorGuid;
    if (IsSameGUID(guid, ACPITableGUID) == 1) {
      return configurationTable[i].VendorTable;
    }
  }

  return 0;
}

void ExitBootServices(EFI_HANDLE ImageHandle) {
  puts(L"ExitBootServices start\r\n");

  UINTN status;
  GetMemoryMap();
  //assert(status, L"GetMemoryMap");

  status = ST->BootServices->ExitBootServices(ImageHandle, memoryMapInfo.MapKey);
  assert(status, L"ExitBootServices");
}

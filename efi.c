#include "efi.h"
#include "common.h"

EFI_SYSTEM_TABLE* ST;
EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* SFSP;

void EFIInit(EFI_SYSTEM_TABLE* SystemTable){
  ST=SystemTable;

  EFI_GUID GopGUID =  {0x9042a9de, 0x23dc, 0x4a38,{0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}};
  EFI_GUID SfspGUID =  {0x0964e5b22,0x6459,0x11d2,{0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

  ST->BootServices->LocateProtocol(&GopGUID,NULL,(void**)&GOP);
  ST->BootServices->LocateProtocol(&SfspGUID,NULL,(void**)&SFSP);
}

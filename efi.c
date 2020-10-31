#include "efi.h"
#include "common.h"

EFI_SYSTEM_TABLE* ST;
EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;

void EFIInit(EFI_SYSTEM_TABLE* SystemTable){
  ST=SystemTable;

  EFI_GUID GopGUID =  {0x9042a9de, 0x23dc, 0x4a38, \
                                 {0x96, 0xfb, 0x7a, 0xde,    \
                                              0xd0, 0x80, 0x51, 0x6a}};

  ST->BootServices->LocateProtocol(&GopGUID,NULL,(void**)&GOP);
}

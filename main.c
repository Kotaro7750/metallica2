#include "efi.h"
#include "common.h"

void efi_main(void* ImageHandle __attribute__((unused)),EFI_SYSTEM_TABLE* SystemTable){
  EFIInit(SystemTable);
  ST->ConOut->ClearScreen(ST->ConOut);

  ST->ConOut->OutputString(ST->ConOut,L"Hello World!!\n");
  while (1) {
  }
}

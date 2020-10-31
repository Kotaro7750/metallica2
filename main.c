#include "efi.h"
#include "common.h"
#include "fs.h"

void efi_main(void* ImageHandle __attribute__((unused)),EFI_SYSTEM_TABLE* SystemTable){
  EFIInit(SystemTable);
  ST->ConOut->ClearScreen(ST->ConOut);
  PrintLOGO();
  puts(L"EFI init done\n\r");

  cat(L"abc");
  while (1) {
  }
}

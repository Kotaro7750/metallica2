#include "common.h"
#include "efi.h"
#include "fs.h"

#define CONF_FILE_NAME L"boot.conf"
#define CONF_FILE_LINE_SIZE 16

struct bootConfig {
  void *kernelAddress;
  void *fsAddress;
};

void LoadConfig(struct bootConfig *config);

void efi_main(void *ImageHandle __attribute__((unused)),
              EFI_SYSTEM_TABLE *SystemTable) {
  EFIInit(SystemTable);
  ST->ConOut->ClearScreen(ST->ConOut);
  PrintLOGO();
  puts(L"EFI init done\n\r");

  struct bootConfig config;
  LoadConfig(&config);
  while (1) {
  }
}

void LoadConfig(struct bootConfig *config) {
  puts(L"LoadConfig start\n\r");

  void *status;
  EFI_FILE_PROTOCOL *root;
  status = SFSP->OpenVolume(SFSP, &root);
  assert(status, L"SFSP->OpenVolume\n\r");

  EFI_FILE_PROTOCOL *confFile;
  status = root->Open(root, &confFile, CONF_FILE_NAME, EFI_FILE_MODE_READ, 0);
  assert(status, L"root->Open\n\r");

  UINTN buf_size = MAX_FILE_BUF;
  CHAR8 buf[MAX_FILE_BUF];
  FileRead(confFile, buf, 33);

  puth(GetFileSize(confFile), 10);
  puts(L"\n\r");

  confFile->Close(confFile);
  root->Close(root);

  config->kernelAddress = atoull16(buf, 16);
  config->fsAddress = atoull16(buf + CONF_FILE_LINE_SIZE + 1, 16);

  puts(L"kernelAddress:");
  puth(config->kernelAddress, 16);
  puts(L"\n\r");

  puts(L"fsAddress:");
  puth(config->fsAddress, 16);
  puts(L"\n\r");

  puts(L"LoadConfig done\n\r");
};

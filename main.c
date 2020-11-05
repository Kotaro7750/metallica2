#include "common.h"
#include "efi.h"
#include "fs.h"

#define CONF_FILE_NAME L"boot.conf"
#define CONF_FILE_LINE_SIZE 16
#define KERNEL_FILE_NAME L"kernel.bin"

struct bootConfig {
  void *kernelAddress;
  void *fsAddress;
};

void LoadConfig(struct bootConfig *config);
void LoadKernel(void *kernelAddress);

void efi_main(void *ImageHandle __attribute__((unused)),
              EFI_SYSTEM_TABLE *SystemTable) {
  EFIInit(SystemTable);
  ST->ConOut->ClearScreen(ST->ConOut);
  PrintLOGO();
  puts(L"EFI init done\n\r");

  struct bootConfig config;
  LoadConfig(&config);

  LoadKernel(config.kernelAddress);

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
  FileRead(confFile, buf, 2 * CONF_FILE_LINE_SIZE + 2);

  putparam(GetFileSize(confFile), L"configFile size", 10);

  confFile->Close(confFile);
  root->Close(root);

  config->kernelAddress = atoull16(buf, CONF_FILE_LINE_SIZE);
  config->fsAddress =
      atoull16(buf + CONF_FILE_LINE_SIZE + 1, CONF_FILE_LINE_SIZE);

  putparam(config->kernelAddress, L"kernelAddress", CONF_FILE_LINE_SIZE);
  putparam(config->fsAddress, L"fsAddress", CONF_FILE_LINE_SIZE);

  puts(L"LoadConfig done\n\r");
};

void LoadKernel(void *kernelAddress) {
  puts(L"LoadKernel start\n\r");

  void *status;
  EFI_FILE_PROTOCOL *root;
  status = SFSP->OpenVolume(SFSP, &root);
  assert(status, L"SFSP->OpenVolume\n\r");

  EFI_FILE_PROTOCOL *kernel;
  status = root->Open(root, &kernel, KERNEL_FILE_NAME, EFI_FILE_MODE_READ, 0);
  assert(status, L"root->Open\n\r");

  UINT64 kernelSize = GetFileSize(kernel);
  putparam(kernelSize, L"kernelSize", 10);

  struct kernelHeader {
    UINT64 bssStart;
    UINT64 bssSize;
  };

  struct kernelHeader header;

  FileRead(kernel, &header, sizeof(header));
  putparam(header.bssStart, L"BSS start", 10);
  putparam(header.bssSize, L"BSS size", 10);

  kernelSize -= sizeof(header);
  FileRead(kernel, kernelAddress, kernelSize);
  kernel->Close(kernel);

  ST->BootServices->SetMem(header.bssStart, header.bssSize, 0);
  puts(L"LoadKernel done\n\r");
}

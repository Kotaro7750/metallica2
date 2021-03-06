#include "common.h"
#include "efi.h"
#include "elf.h"
#include "fb.h"
#include "fs.h"
#include "mem.h"

#define CONF_FILE_NAME L"boot.conf"
#define CONF_FILE_LINE_SIZE 16
#define KERNEL_FILE_NAME L"kernel.bin"
#define MB 1048576

struct bootConfig {
  void *kernelAddress;
  void *fsAddress;
};

struct __attribute__((packed)) PlatformInfo {
  struct FrameBufferInfo fb;
  void *RSDPAddress;
};

void LoadConfig(struct bootConfig *config);
unsigned long long LoadKernel(void *kernelAddress);
void ConfirmBoot();

void efi_main(void *ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  EFIInit(SystemTable);
  ST->ConOut->ClearScreen(ST->ConOut);

  PrintLOGO();
  puts(L"EFI init done\n\r");

  struct bootConfig config;
  LoadConfig(&config);

  UINT64 entryPoint = LoadElf(L"kernel.elf");

  struct PlatformInfo platformInfo;
  FBInit(&(platformInfo.fb));

  void *RSDPAddress = (void *)FindRSDPAddress();
  platformInfo.RSDPAddress = RSDPAddress;

  // set up FreeMap for physical memory management
  // TODO magic number should be MACRO
  unsigned long long physicalMemoryFreeMepBase = config.kernelAddress + (1 * MB);
  putparam(physicalMemoryFreeMepBase, L"FreeMap Base", 10);

  UINT64 freeMapSize = InitPhysicalMemoryFreeMap(physicalMemoryFreeMepBase);
  putparam(freeMapSize, L"FreeMap Size", 10);

  struct FreeMapInfo freeMapInfo;
  freeMapInfo.FreeMapBase = physicalMemoryFreeMepBase;
  freeMapInfo.FreeMapSize = freeMapSize;

  ReserveUnusablePagesOnPhysicalMemoryFreeMap(physicalMemoryFreeMepBase, freeMapSize);
  // exclude pci hole
  for (unsigned int pageIndex = 0xc0000; pageIndex < 0x100000; pageIndex++) {
    ((struct Page *)physicalMemoryFreeMepBase)[pageIndex].flags |= PG_RESERVED;
  }

  // exclude kernel region
  for (unsigned int pageIndex = ((UINT64)config.kernelAddress >> 12); pageIndex <= ((physicalMemoryFreeMepBase + freeMapSize) >> 12); pageIndex++) {
    ((struct Page *)physicalMemoryFreeMepBase)[pageIndex].flags |= PG_RESERVED;
  }

  // set kernel argument
  unsigned long long kernelArg1 = (unsigned long long)ST;
  putparam(kernelArg1, L"arg1", 10);
  unsigned long long kernelArg2 = (unsigned long long)&platformInfo;
  putparam(kernelArg2, L"arg2", 10);
  unsigned long long kernelArg3 = (unsigned long long)&freeMapInfo;
  putparam(kernelArg3, L"arg3", 10);

  // TODO magic number should be MACRO
  unsigned long long stackBase = config.kernelAddress + (1 * MB) - 1;
  putparam(stackBase, L"Stack Base", 10);

  ConfirmBoot();
  ExitBootServices(ImageHandle);

  unsigned long long _sb = stackBase, _ks = entryPoint;
  __asm__("	mov	%0, %%rdx\n"
          "	mov	%1, %%rsi\n"
          "	mov	%2, %%rdi\n"
          "	mov	%3, %%rsp\n"
          "	jmp	*%4\n" ::"m"(kernelArg3),
          "m"(kernelArg2),
          "m"(kernelArg1),
          "m"(_sb),
          "m"(_ks));

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
  config->fsAddress = atoull16(buf + CONF_FILE_LINE_SIZE + 1, CONF_FILE_LINE_SIZE);

  putparam(config->kernelAddress, L"kernelAddress", 10);
  putparam(config->fsAddress, L"fsAddress", 10);

  puts(L"LoadConfig done\n\r");
};

unsigned long long LoadKernel(void *kernelAddress) {
  puts(L"LoadKernel start\n\r");

  void *status;
  EFI_FILE_PROTOCOL *root;
  status = SFSP->OpenVolume(SFSP, &root);
  assert(status, L"SFSP->OpenVolume\n\r");

  EFI_FILE_PROTOCOL *kernel;
  status = root->Open(root, &kernel, KERNEL_FILE_NAME, EFI_FILE_MODE_READ, 0);
  assert(status, L"root->Open\n\r");

  UINT64 kernelSize = GetFileSize(kernel);
  putparam(kernelSize, L"kernelSize with header", 10);

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

  return kernelSize;
}

void ConfirmBoot() {
  UINTN waitIndex;
  puts(L"Ready to Boot. Press Any Key to start to Boot\r\n");
  ST->BootServices->WaitForEvent(1, &(ST->ConIn->WaitForKey), &waitIndex);
}

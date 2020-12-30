#include "elf.h"
#include "common.h"
#include "efi.h"
#include "fs.h"
#include "mem.h"

int ValidateElfFormat(Elf64_Ehdr *ehdr) {
  // validate magic number
  if (ehdr->e_ident[EI_MAG0] != ELFMAG0 || ehdr->e_ident[EI_MAG1] != ELFMAG1 || ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
      ehdr->e_ident[EI_MAG3] != ELFMAG3) {
    puts(L"Magic number\n\r");
    return 0;
  }

  if (ehdr->e_ident[EI_CLASS] != ELFCLASS64) {
    puts(L"class\n\r");
    return 0;
  }

  if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB) {
    puts(L"data\n\r");
    return 0;
  }

  if (ehdr->e_ident[EI_VERSION] != EV_CURRENT) {
    puts(L"version\n\r");
    return 0;
  }

  if (ehdr->e_ident[EI_OSABI] != ELFOSABI_SYSV) {
    puts(L"ABI\n\r");
    return 0;
  }
  return 1;
}

UINT64 LoadElf(CHAR16 *filename) {
  puts(L"LoadElf start\n\r");

  void *status;
  EFI_FILE_PROTOCOL *root;
  status = SFSP->OpenVolume(SFSP, &root);
  assert(status, L"SFSP->OpenVolume\n\r");

  EFI_FILE_PROTOCOL *elfFile;
  status = root->Open(root, &elfFile, filename, EFI_FILE_MODE_READ, 0);
  assert(status, L"root->Open\n\r");

  UINT64 fileSize = GetFileSize(elfFile);

  void *head;
  status = ST->BootServices->AllocatePool(EfiLoaderData, fileSize, (void **)&head);
  assert(status, L"ST->BootServices->AllocatePool\n\r");

  FileRead(elfFile, head, fileSize);

  assert(ValidateElfFormat((Elf64_Ehdr *)head) == 0, L"Cannot use this elf format\n\r");

  UINT64 entryPoint = ((Elf64_Ehdr *)head)->e_entry;
  putparam(entryPoint, L"entry point", 10);

  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)head;
  Elf64_Phdr *phdr = head + ehdr->e_phoff;
  Elf64_Shdr *shdr = head + ehdr->e_shoff;
  UINT32 phnum = ehdr->e_phnum;
  UINT32 shnum = ehdr->e_shnum;

  // zero clear
  for (int i = 0; i < phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
      UINT64 segmentStartAddress = phdr[i].p_paddr;
      UINT64 segmentSize = phdr[i].p_memsz;
      ST->BootServices->SetMem((void *)segmentStartAddress, segmentSize, 0);
    }
  }

  // load each segments
  for (int i = 0; i < phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
      UINT64 segmentStartAddress = phdr[i].p_paddr;
      UINT64 segmentFileOffset = phdr[i].p_offset;
      UINT64 segmentFileSize = phdr[i].p_filesz;

      // BootServices->CopyMem does'nt work
      CopyMemory((void *)segmentStartAddress, (void *)(head + segmentFileOffset), segmentFileSize);
    }
  }

  puts(L"LoadElf end\n\r");

  return entryPoint;
}

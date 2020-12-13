#include "fs.h"
#include "common.h"
#include "efi.h"

void cat(CHAR16 *filename) {
  void *status;
  EFI_FILE_PROTOCOL *root;

  status = SFSP->OpenVolume(SFSP, &root);
  assert(status, L"SFSP->OpenVolume root");

  EFI_FILE_PROTOCOL *file;
  status = root->Open(root, &file, filename, EFI_FILE_MODE_READ, 0);
  assert(status, L"root->Open");

  // TODO get file size and call FileRead()
  UINTN bufferSize = MAX_FILE_BUF;
  CHAR8 buf[MAX_FILE_BUF / 2];
  CHAR16 buf_short[MAX_FILE_BUF];
  status = file->Read(file, &bufferSize, (void *)buf);
  assert(status, L"file->Read");

  // file->Read write buffer by a byte(CHAR8), so buffer must be converted into
  // CHAR16.
  Char8Buf2Char16Buf(buf, buf_short, MAX_FILE_BUF / 2);

  puts(buf_short);

  file->Close(file);
  root->Close(root);
}

UINT64 GetFileSize(EFI_FILE_PROTOCOL *file) {
  EFI_GUID file_info_guid = {0x09576e92, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
  EFI_FILE_INFO file_info;
  // TODO this constant should be macro
  UINTN file_info_size = 180;
  UINT64 buf[180];

  file->GetInfo(file, &file_info_guid, &file_info_size, buf);
  return ((EFI_FILE_INFO *)buf)->FileSize;
}

void FileRead(EFI_FILE_PROTOCOL *file, void *buf, unsigned long long size) {
  void *status;
  while (size > FILE_READ_UNIT) {
    UINTN buf_size = FILE_READ_UNIT;
    status = file->Read(file, &buf_size, buf);
    assert(status, L"FileRead:file->Read");
    buf += buf_size;
    size -= buf_size;
  }

  while (size > 0) {
    UINTN buf_size = size;
    status = file->Read(file, &buf_size, buf);
    assert(status, L"FileRead:file->Read");
    buf += buf_size;
    size -= buf_size;
  }
}

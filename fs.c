#include "efi.h"
#include "common.h"
#include "fs.h"

void cat(CHAR16 *filename){
  void*  status;
  EFI_FILE_PROTOCOL* root;

  status = SFSP->OpenVolume(SFSP,&root);
  assert(status, L"SFSP->OpenVolume root");

  EFI_FILE_PROTOCOL* file;
  status = root->Open(root,&file,filename,EFI_FILE_MODE_READ,0);
  assert(status, L"root->Open");

  UINTN bufferSize = MAX_FILE_BUF;
  CHAR8 buf[MAX_FILE_BUF / 2];
  CHAR16 buf_short[MAX_FILE_BUF];
  status = file->Read(file, &bufferSize, (void *)buf);
  assert(status, L"file->Read");

  // file->Read write buffer by a byte(CHAR8), so buffer must be converted into CHAR16.
  Char8Buf2Char16Buf(buf, buf_short,MAX_FILE_BUF / 2);

  puts(buf_short);

  file->Close(file);
  root->Close(root);
}

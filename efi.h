#ifndef _EFI_H
#define _EFI_H

typedef unsigned long long UINTN;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef char CHAR8;
typedef short CHAR16;
typedef void *EFI_HANDLE;
typedef struct _EFI_GUID {
  unsigned int Data1;
  unsigned short Data2;
  unsigned short Data3;
  unsigned char Data4[8];
} EFI_GUID;

typedef struct _EFI_TABLE_HEADER {
  UINT64 Signature;
  UINT32 Revision;
  UINT32 HeaderSize;
  UINT32 CRC32;
  UINT32 Reserved;
} EFI_TABLE_HEADER;

typedef struct {
  UINT16 ScanCode;
  CHAR16 UnicodeChar;
} EFI_INPUT_KEY;

typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
  void *Reset;
  void *(*ReadKeyStroke)(struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
                         EFI_INPUT_KEY *Key);
  void *WaitForKey;
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
  void *Reset;
  void *(*OutputString)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
                        CHAR16 *String);
  void *TestString;
  void *QueryMode;
  void *SetMode;
  void *SetAttribute;
  void *(*ClearScreen)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This);
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef struct _EFI_BOOT_SERVICES {
  EFI_TABLE_HEADER Hdr;
  void *_buf[37];
  void *(*LocateProtocol)(EFI_GUID *Protocol, void *Registration,
                          void **Interface);
} EFI_BOOT_SERVICES;

typedef struct _EFI_SYSTEM_TABLE {
  EFI_TABLE_HEADER Hdr;
  CHAR16 *FirmwareVendor;
  UINT32 FirmwareRevision;
  EFI_HANDLE ConsoleInHandle;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
  EFI_HANDLE ConsoleOutHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
  EFI_HANDLE StrandardErrorHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
  void *RuntimeServices;
  EFI_BOOT_SERVICES *BootServices;
} EFI_SYSTEM_TABLE;

typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
  void *_buf[3];
  struct EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE {
    UINT32 MaxMode;
    UINT32 Mode;
    struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION {
      UINT32 Version;
      UINT32 HorizontalResolution;
      UINT32 VerticalResolution;
      enum EFI_GRAPHICS_PIXEL_FORMAT {
        PixelRedGreenBlueReserved8BitPerColor,
        PixelBlueGreenRedReserved8BitPerColor,
        PixelBitMask,
        PixelBltOnly,
        PixelFormatMax
      } PixelFormat;
    } * Info;
    UINTN SizeOfInfo;
    void *FrameBufferBase;
    UINTN FrameBufferSize;
  } * Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

typedef struct _EFI_GRAPHICS_OUTPUT_BLT_PIXEL {
  UINT8 Blue;
  UINT8 Green;
  UINT8 Red;
  UINT8 Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

EFI_SYSTEM_TABLE *ST;
EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;

void EFIInit(EFI_SYSTEM_TABLE *);

#endif

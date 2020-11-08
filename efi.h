#ifndef _EFI_H
#define _EFI_H

typedef unsigned long long UINTN;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef char CHAR8;
typedef short CHAR16;
typedef unsigned short UCHAR16;
typedef void *EFI_HANDLE;
typedef void *EFI_EVENT;
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

typedef struct {
  UINT32 Type;
  UINT64 PhysicalStart;
  UINT64 VirtualStart;
  UINT64 NumberOfPages;
  UINT64 Attribute;
} EFI_MEMORY_DESCRIPTER;

typedef struct _EFI_BOOT_SERVICES {
  EFI_TABLE_HEADER Hdr;
  void *_buf[4];
  void *(*GetMemoryMap)(UINTN *MemoryMapSize, EFI_MEMORY_DESCRIPTER *MemoryMap,
                        UINTN *MapKey, UINTN *DescriptorSize,
                        UINT32 *DescriptorVersion);
  void *_buf2[4];
  void *(*WaitForEvent)(UINTN NumberOfEvents, EFI_EVENT *Event, UINTN *Index);
  void *_buf3[16];
  void *(*ExitBootServices)(EFI_HANDLE ImageHandle, UINTN MapKey);
  void *_buf4[10];
  void *(*LocateProtocol)(EFI_GUID *Protocol, void *Registration,
                          void **Interface);
  void *_buf5[4];
  void *(*SetMem)(void *buffer, UINTN Size, UINT8 Value);
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

typedef struct _EFI_FILE_PROTOCOL {
  UINT64 Revision;
  void *(*Open)(struct _EFI_FILE_PROTOCOL *This,
                struct _EFI_FILE_PROTOCOL **NewHandle, CHAR16 *FileName,
                UINT64 OpenMode, UINT64 Attributes);
  void *(*Close)(struct _EFI_FILE_PROTOCOL *This);
  void *(*Delete)(struct _EFI_FILE_PROTOCOL *This);
  void *(*Read)(struct _EFI_FILE_PROTOCOL *This, UINTN *BufferSize,
                void *Buffer);
  void *(*Write)(struct _EFI_FILE_PROTOCOL *This, UINTN *BufferSize,
                 void *Buffer);
  void *_buf[2];
  void *(*GetInfo)(struct _EFI_FILE_PROTOCOL *This, EFI_GUID *InformationType,
                   UINTN *BufferSize, void *Buffer);
  void *_buf2[1];
  void *(*Flush)(struct _EFI_FILE_PROTOCOL *This);
} EFI_FILE_PROTOCOL;

typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
  UINT64 Revision;
  void *(*OpenVolume)(struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This,
                      EFI_FILE_PROTOCOL **Root);
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef struct {
  unsigned short Year;
  unsigned char Month;
  unsigned char Day;
  unsigned char Hour;
  unsigned char Minute;
  unsigned char Second;
  unsigned char Pad1;
  unsigned int Nanosecond;
  unsigned short TimeZone;
  unsigned char Daylight;
  unsigned char Pad2;
} EFI_TIME;

typedef struct {
  UINT64 Size;
  UINT64 FileSize;
  UINT64 PhysicalSize;
  EFI_TIME CreateTime;
  EFI_TIME LastAccessTime;
  EFI_TIME ModificationTime;
  UINT64 Attribute;
  CHAR16 FileName[];
} EFI_FILE_INFO;

EFI_SYSTEM_TABLE *ST;
EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SFSP;

void EFIInit(EFI_SYSTEM_TABLE *);
void ExitBootServices(EFI_HANDLE ImageHandle);

#endif

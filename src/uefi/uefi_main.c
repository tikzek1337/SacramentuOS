// SacramentuOS native UEFI build
// Native x86_64 UEFI application: shell, docs and RAM editor are embedded into BOOTX64.EFI.
// v0.3.5: UEFI boot-safe mode: firmware text console by default; optional safe GOP framebuffer.
// No BIOS, no VGA 0xB8000, no PS/2 dependency. Build with clang/LLD as BOOTX64.EFI.

#define NULL ((void*)0)

typedef unsigned char      UINT8;
typedef unsigned short     UINT16;
typedef short              INT16;
typedef unsigned int       UINT32;
typedef unsigned long long UINT64;
typedef unsigned long long UINTN;
typedef long long          INTN;
typedef UINT16             CHAR16;
typedef void               VOID;
typedef VOID*              EFI_HANDLE;
typedef VOID*              EFI_EVENT;
typedef UINT64             EFI_STATUS;
typedef UINT64             EFI_PHYSICAL_ADDRESS;
typedef UINT64             EFI_VIRTUAL_ADDRESS;
typedef unsigned char      BOOLEAN;

#define EFI_SUCCESS 0
#define EFI_LOAD_ERROR 1
#define EFI_INVALID_PARAMETER 2
#define EFI_UNSUPPORTED 3
#define EFI_BAD_BUFFER_SIZE 4
#define EFI_BUFFER_TOO_SMALL 5
#define EFI_NOT_READY 6
#define EFI_DEVICE_ERROR 7
#define EFI_ABORTED 21

#define EFI_TEXT_ATTR(fg,bg) ((UINTN)((fg) | ((bg) << 4)))
#define EFI_BLACK        0x00
#define EFI_BLUE         0x01
#define EFI_GREEN        0x02
#define EFI_CYAN         0x03
#define EFI_RED          0x04
#define EFI_MAGENTA      0x05
#define EFI_BROWN        0x06
#define EFI_LIGHTGRAY    0x07
#define EFI_DARKGRAY     0x08
#define EFI_LIGHTBLUE    0x09
#define EFI_LIGHTGREEN   0x0A
#define EFI_LIGHTCYAN    0x0B
#define EFI_LIGHTRED     0x0C
#define EFI_YELLOW       0x0E
#define EFI_WHITE        0x0F

#define SCAN_NULL        0x00
#define SCAN_UP          0x01
#define SCAN_DOWN        0x02
#define SCAN_RIGHT       0x03
#define SCAN_LEFT        0x04
#define SCAN_HOME        0x05
#define SCAN_END         0x06
#define SCAN_INSERT      0x07
#define SCAN_DELETE      0x08
#define SCAN_PAGE_UP     0x09
#define SCAN_PAGE_DOWN   0x0A
#define SCAN_F1          0x0B
#define SCAN_F2          0x0C
#define SCAN_F3          0x0D
#define SCAN_F4          0x0E
#define SCAN_F5          0x0F
#define SCAN_F6          0x10
#define SCAN_F7          0x11
#define SCAN_F8          0x12
#define SCAN_F9          0x13
#define SCAN_F10         0x14
#define SCAN_ESC         0x17

typedef struct {
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

typedef struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef EFI_STATUS (*EFI_INPUT_RESET)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, BOOLEAN ExtendedVerification);
typedef EFI_STATUS (*EFI_INPUT_READ_KEY)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, EFI_INPUT_KEY *Key);
struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_INPUT_RESET Reset;
    EFI_INPUT_READ_KEY ReadKeyStroke;
    EFI_EVENT WaitForKey;
};

typedef struct {
    INTN MaxMode;
    INTN Mode;
    INTN Attribute;
    INTN CursorColumn;
    INTN CursorRow;
    BOOLEAN CursorVisible;
} SIMPLE_TEXT_OUTPUT_MODE;

typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef EFI_STATUS (*EFI_TEXT_RESET)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, BOOLEAN ExtendedVerification);
typedef EFI_STATUS (*EFI_TEXT_STRING)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *String);
typedef EFI_STATUS (*EFI_TEXT_TEST_STRING)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *String);
typedef EFI_STATUS (*EFI_TEXT_QUERY_MODE)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN ModeNumber, UINTN *Columns, UINTN *Rows);
typedef EFI_STATUS (*EFI_TEXT_SET_MODE)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN ModeNumber);
typedef EFI_STATUS (*EFI_TEXT_SET_ATTRIBUTE)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN Attribute);
typedef EFI_STATUS (*EFI_TEXT_CLEAR_SCREEN)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This);
typedef EFI_STATUS (*EFI_TEXT_SET_CURSOR_POSITION)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN Column, UINTN Row);
typedef EFI_STATUS (*EFI_TEXT_ENABLE_CURSOR)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, BOOLEAN Visible);
struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_TEXT_RESET Reset;
    EFI_TEXT_STRING OutputString;
    EFI_TEXT_TEST_STRING TestString;
    EFI_TEXT_QUERY_MODE QueryMode;
    EFI_TEXT_SET_MODE SetMode;
    EFI_TEXT_SET_ATTRIBUTE SetAttribute;
    EFI_TEXT_CLEAR_SCREEN ClearScreen;
    EFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
    EFI_TEXT_ENABLE_CURSOR EnableCursor;
    SIMPLE_TEXT_OUTPUT_MODE *Mode;
};

typedef struct {
    UINT16 Year;
    UINT8 Month;
    UINT8 Day;
    UINT8 Hour;
    UINT8 Minute;
    UINT8 Second;
    UINT8 Pad1;
    UINT32 Nanosecond;
    INT16 TimeZone;
    UINT8 Daylight;
    UINT8 Pad2;
} EFI_TIME;


typedef struct EFI_BOOT_SERVICES EFI_BOOT_SERVICES;
typedef struct EFI_RUNTIME_SERVICES EFI_RUNTIME_SERVICES;

typedef EFI_STATUS (*EFI_WAIT_FOR_EVENT)(UINTN NumberOfEvents, EFI_EVENT *Event, UINTN *Index);
typedef EFI_STATUS (*EFI_CHECK_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS (*EFI_STALL)(UINTN Microseconds);
typedef EFI_STATUS (*EFI_SET_WATCHDOG_TIMER)(UINTN Timeout, UINT64 WatchdogCode, UINTN DataSize, CHAR16 *WatchdogData);
typedef EFI_STATUS (*EFI_EXIT)(EFI_HANDLE ImageHandle, EFI_STATUS ExitStatus, UINTN ExitDataSize, CHAR16 *ExitData);
typedef EFI_STATUS (*EFI_GET_TIME)(EFI_TIME *Time, VOID *Capabilities);
typedef VOID (*EFI_RESET_SYSTEM)(INTN ResetType, EFI_STATUS ResetStatus, UINTN DataSize, CHAR16 *ResetData);

struct EFI_BOOT_SERVICES {
    EFI_TABLE_HEADER Hdr;
    VOID *RaiseTPL;
    VOID *RestoreTPL;
    VOID *AllocatePages;
    VOID *FreePages;
    VOID *GetMemoryMap;
    VOID *AllocatePool;
    VOID *FreePool;
    VOID *CreateEvent;
    VOID *SetTimer;
    EFI_WAIT_FOR_EVENT WaitForEvent;
    VOID *SignalEvent;
    VOID *CloseEvent;
    EFI_CHECK_EVENT CheckEvent;
    VOID *InstallProtocolInterface;
    VOID *ReinstallProtocolInterface;
    VOID *UninstallProtocolInterface;
    VOID *HandleProtocol;
    VOID *Reserved;
    VOID *RegisterProtocolNotify;
    VOID *LocateHandle;
    VOID *LocateDevicePath;
    VOID *InstallConfigurationTable;
    VOID *LoadImage;
    VOID *StartImage;
    EFI_EXIT Exit;
    VOID *UnloadImage;
    VOID *ExitBootServices;
    VOID *GetNextMonotonicCount;
    EFI_STALL Stall;
    EFI_SET_WATCHDOG_TIMER SetWatchdogTimer;
    VOID *ConnectController;
    VOID *DisconnectController;
    VOID *OpenProtocol;
    VOID *CloseProtocol;
    VOID *OpenProtocolInformation;
    VOID *ProtocolsPerHandle;
    VOID *LocateHandleBuffer;
    VOID *LocateProtocol;
    VOID *InstallMultipleProtocolInterfaces;
    VOID *UninstallMultipleProtocolInterfaces;
    VOID *CalculateCrc32;
    VOID *CopyMem;
    VOID *SetMem;
    VOID *CreateEventEx;
};

struct EFI_RUNTIME_SERVICES {
    EFI_TABLE_HEADER Hdr;
    EFI_GET_TIME GetTime;
    VOID *SetTime;
    VOID *GetWakeupTime;
    VOID *SetWakeupTime;
    VOID *SetVirtualAddressMap;
    VOID *ConvertPointer;
    VOID *GetVariable;
    VOID *GetNextVariableName;
    VOID *SetVariable;
    VOID *GetNextHighMonotonicCount;
    EFI_RESET_SYSTEM ResetSystem;
    VOID *UpdateCapsule;
    VOID *QueryCapsuleCapabilities;
    VOID *QueryVariableInfo;
};


typedef struct {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8 Data4[8];
} EFI_GUID;

typedef enum {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
    UINT32 RedMask;
    UINT32 GreenMask;
    UINT32 BlueMask;
    UINT32 ReservedMask;
} EFI_PIXEL_BITMASK;

typedef struct {
    UINT32 Version;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
    EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    EFI_PIXEL_BITMASK PixelInformation;
    UINT32 PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
    UINT32 MaxMode;
    UINT32 Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    UINTN SizeOfInfo;
    EFI_PHYSICAL_ADDRESS FrameBufferBase;
    UINTN FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;
typedef EFI_STATUS (*EFI_GOP_QUERY_MODE)(EFI_GRAPHICS_OUTPUT_PROTOCOL *This, UINT32 ModeNumber, UINTN *SizeOfInfo, EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info);
typedef EFI_STATUS (*EFI_GOP_SET_MODE)(EFI_GRAPHICS_OUTPUT_PROTOCOL *This, UINT32 ModeNumber);
typedef EFI_STATUS (*EFI_GOP_BLT)(EFI_GRAPHICS_OUTPUT_PROTOCOL *This, VOID *BltBuffer, UINTN BltOperation, UINTN SourceX, UINTN SourceY, UINTN DestinationX, UINTN DestinationY, UINTN Width, UINTN Height, UINTN Delta);
struct EFI_GRAPHICS_OUTPUT_PROTOCOL {
    EFI_GOP_QUERY_MODE QueryMode;
    EFI_GOP_SET_MODE SetMode;
    EFI_GOP_BLT Blt;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
};

typedef EFI_STATUS (*EFI_LOCATE_PROTOCOL)(EFI_GUID *Protocol, VOID *Registration, VOID **Interface);

#define MAX_GOP_MODES 64
#define FONT_W 8
#define FONT_H 16

struct GopModeCache {
    UINT32 mode;
    UINT32 width;
    UINT32 height;
    UINT32 pps;
};

static EFI_GRAPHICS_OUTPUT_PROTOCOL *g_gop = NULL;
static int g_gfx = 0;
static UINT32 *g_fb = NULL;
static UINT32 g_px_w = 0;
static UINT32 g_px_h = 0;
static UINT32 g_pps = 0;
static EFI_GRAPHICS_PIXEL_FORMAT g_pixfmt = PixelBlueGreenRedReserved8BitPerColor;
static UINTN g_cur_col = 0;
static UINTN g_cur_row = 0;
static struct GopModeCache g_gop_modes[MAX_GOP_MODES];
static UINTN g_gop_mode_count = 0;
static UINT32 g_gop_current_mode = 0;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    CHAR16 *FirmwareVendor;
    UINT32 FirmwareRevision;
    EFI_HANDLE ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
    EFI_HANDLE ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
    EFI_HANDLE StandardErrorHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
    EFI_RUNTIME_SERVICES *RuntimeServices;
    EFI_BOOT_SERVICES *BootServices;
    UINTN NumberOfTableEntries;
    VOID *ConfigurationTable;
} EFI_SYSTEM_TABLE;

static EFI_HANDLE g_image;
static EFI_SYSTEM_TABLE *g_st;
static EFI_SIMPLE_TEXT_INPUT_PROTOCOL *g_in;
static EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *g_out;
static EFI_BOOT_SERVICES *g_bs;
static EFI_RUNTIME_SERVICES *g_rt;
static UINTN g_attr = EFI_TEXT_ATTR(EFI_LIGHTGREEN, EFI_BLACK);
__attribute__((used)) static VOID *g_reloc_anchor = (VOID*)&g_attr;
static UINTN g_cols = 80;
static UINTN g_rows = 25;
static UINTN g_text_mode = 0;

#define SOS_VERSION L"0.3.5"
#define SOS_LAST_UPDATE L"2026-05-26"
#define SOS_WIDE2(x) L##x
#define SOS_WIDE(x) SOS_WIDE2(x)
#define SOS_BUILD_DATE SOS_WIDE(__DATE__)
#define SOS_BUILD_TIME SOS_WIDE(__TIME__)

#define MAX_LINE 160
#define MAX_ARGS 8
#define MAX_FILES 8
#define MAX_FILE_NAME 32
#define MAX_FILE_LINES 64
#define MAX_FILE_COLS 120

static CHAR16 g_username[32] = L"user";
static CHAR16 g_line[MAX_LINE];
static CHAR16 g_args_storage[MAX_ARGS][MAX_LINE];
static CHAR16 *g_args[MAX_ARGS];

struct FileEntry {
    int used;
    CHAR16 name[MAX_FILE_NAME];
    int lines;
    CHAR16 text[MAX_FILE_LINES][MAX_FILE_COLS];
};
static struct FileEntry g_files[MAX_FILES];


static CHAR16 e_lines[MAX_FILE_LINES][MAX_FILE_COLS];
static int e_line_count;
static int e_cx;
static int e_cy;
static int e_top;
static int e_dirty;
static int e_left;
static CHAR16 e_status[80];
static CHAR16 e_name[MAX_FILE_NAME];

static UINTN u_strlen(const CHAR16 *s) {
    UINTN n = 0;
    if (!s) return 0;
    while (s[n]) n++;
    return n;
}

static int u_streq(const CHAR16 *a, const CHAR16 *b) {
    UINTN i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == 0 && b[i] == 0;
}

static void u_strcpy(CHAR16 *dst, const CHAR16 *src, UINTN max) {
    if (max == 0) return;
    UINTN i = 0;
    while (i + 1 < max && src[i]) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = 0;
}

static void u_strcat(CHAR16 *dst, const CHAR16 *src, UINTN max) {
    UINTN n = u_strlen(dst);
    UINTN i = 0;
    while (n + i + 1 < max && src[i]) {
        dst[n + i] = src[i];
        i++;
    }
    dst[n + i] = 0;
}

static CHAR16 u_tolower(CHAR16 c) {
    if (c >= L'A' && c <= L'Z') return c + 32;
    return c;
}

static int u_streq_ci(const CHAR16 *a, const CHAR16 *b) {
    UINTN i = 0;
    while (a[i] && b[i]) {
        if (u_tolower(a[i]) != u_tolower(b[i])) return 0;
        i++;
    }
    return a[i] == 0 && b[i] == 0;
}

static void u_uint_to_str(UINT64 v, CHAR16 *out, UINTN max) {
    CHAR16 buf[32];
    UINTN i = 0;
    if (max == 0) return;
    if (v == 0) {
        out[0] = L'0';
        if (max > 1) out[1] = 0;
        return;
    }
    while (v && i < 31) {
        buf[i++] = (CHAR16)(L'0' + (v % 10));
        v /= 10;
    }
    UINTN j = 0;
    while (i && j + 1 < max) out[j++] = buf[--i];
    out[j] = 0;
}

static int u_atoi(const CHAR16 *s) {
    int v = 0;
    UINTN i = 0;
    while (s[i] >= L'0' && s[i] <= L'9') {
        v = v * 10 + (int)(s[i] - L'0');
        i++;
    }
    return v;
}


static UINT8 bits5(const char *s) {
    UINT8 v = 0;
    for (int i = 0; i < 5; i++) {
        v <<= 1;
        if (s[i] == '1') v |= 1;
    }
    return v;
}

static UINT8 row5(int row, const char *r0, const char *r1, const char *r2, const char *r3, const char *r4, const char *r5, const char *r6) {
    const char *r = r0;
    if (row == 1) r = r1; else if (row == 2) r = r2; else if (row == 3) r = r3;
    else if (row == 4) r = r4; else if (row == 5) r = r5; else if (row == 6) r = r6;
    return bits5(r);
}

static UINT8 glyph5(CHAR16 c, int row) {
    if (row < 0 || row > 6) return 0;
    if (c >= L'a' && c <= L'z') c = (CHAR16)(c - 32);
    switch (c) {
        case L'0': return row5(row,"01110","10001","10011","10101","11001","10001","01110");
        case L'1': return row5(row,"00100","01100","00100","00100","00100","00100","01110");
        case L'2': return row5(row,"01110","10001","00001","00010","00100","01000","11111");
        case L'3': return row5(row,"11110","00001","00001","01110","00001","00001","11110");
        case L'4': return row5(row,"00010","00110","01010","10010","11111","00010","00010");
        case L'5': return row5(row,"11111","10000","10000","11110","00001","00001","11110");
        case L'6': return row5(row,"01110","10000","10000","11110","10001","10001","01110");
        case L'7': return row5(row,"11111","00001","00010","00100","01000","01000","01000");
        case L'8': return row5(row,"01110","10001","10001","01110","10001","10001","01110");
        case L'9': return row5(row,"01110","10001","10001","01111","00001","00001","01110");
        case L'A': return row5(row,"01110","10001","10001","11111","10001","10001","10001");
        case L'B': return row5(row,"11110","10001","10001","11110","10001","10001","11110");
        case L'C': return row5(row,"01111","10000","10000","10000","10000","10000","01111");
        case L'D': return row5(row,"11110","10001","10001","10001","10001","10001","11110");
        case L'E': return row5(row,"11111","10000","10000","11110","10000","10000","11111");
        case L'F': return row5(row,"11111","10000","10000","11110","10000","10000","10000");
        case L'G': return row5(row,"01111","10000","10000","10011","10001","10001","01111");
        case L'H': return row5(row,"10001","10001","10001","11111","10001","10001","10001");
        case L'I': return row5(row,"01110","00100","00100","00100","00100","00100","01110");
        case L'J': return row5(row,"00111","00010","00010","00010","00010","10010","01100");
        case L'K': return row5(row,"10001","10010","10100","11000","10100","10010","10001");
        case L'L': return row5(row,"10000","10000","10000","10000","10000","10000","11111");
        case L'M': return row5(row,"10001","11011","10101","10101","10001","10001","10001");
        case L'N': return row5(row,"10001","11001","10101","10011","10001","10001","10001");
        case L'O': return row5(row,"01110","10001","10001","10001","10001","10001","01110");
        case L'P': return row5(row,"11110","10001","10001","11110","10000","10000","10000");
        case L'Q': return row5(row,"01110","10001","10001","10001","10101","10010","01101");
        case L'R': return row5(row,"11110","10001","10001","11110","10100","10010","10001");
        case L'S': return row5(row,"01111","10000","10000","01110","00001","00001","11110");
        case L'T': return row5(row,"11111","00100","00100","00100","00100","00100","00100");
        case L'U': return row5(row,"10001","10001","10001","10001","10001","10001","01110");
        case L'V': return row5(row,"10001","10001","10001","10001","10001","01010","00100");
        case L'W': return row5(row,"10001","10001","10001","10101","10101","10101","01010");
        case L'X': return row5(row,"10001","10001","01010","00100","01010","10001","10001");
        case L'Y': return row5(row,"10001","10001","01010","00100","00100","00100","00100");
        case L'Z': return row5(row,"11111","00001","00010","00100","01000","10000","11111");
        case L' ': return 0;
        case L'.': return row5(row,"00000","00000","00000","00000","00000","01100","01100");
        case L',': return row5(row,"00000","00000","00000","00000","01100","01100","01000");
        case L':': return row5(row,"00000","01100","01100","00000","01100","01100","00000");
        case L';': return row5(row,"00000","01100","01100","00000","01100","01100","01000");
        case L'!': return row5(row,"00100","00100","00100","00100","00100","00000","00100");
        case L'?': return row5(row,"01110","10001","00001","00010","00100","00000","00100");
        case L'-': return row5(row,"00000","00000","00000","11111","00000","00000","00000");
        case L'_': return row5(row,"00000","00000","00000","00000","00000","00000","11111");
        case L'+': return row5(row,"00000","00100","00100","11111","00100","00100","00000");
        case L'=': return row5(row,"00000","00000","11111","00000","11111","00000","00000");
        case L'/': return row5(row,"00001","00010","00010","00100","01000","01000","10000");
        case L'\\': return row5(row,"10000","01000","01000","00100","00010","00010","00001");
        case L'|': return row5(row,"00100","00100","00100","00100","00100","00100","00100");
        case L'@': return row5(row,"01110","10001","10111","10101","10111","10000","01110");
        case L'#': return row5(row,"01010","01010","11111","01010","11111","01010","01010");
        case L'$': return row5(row,"00100","01111","10100","01110","00101","11110","00100");
        case L'%': return row5(row,"11001","11010","00010","00100","01000","01011","10011");
        case L'&': return row5(row,"01100","10010","10100","01000","10101","10010","01101");
        case L'*': return row5(row,"00000","10101","01110","11111","01110","10101","00000");
        case L'(': return row5(row,"00010","00100","01000","01000","01000","00100","00010");
        case L')': return row5(row,"01000","00100","00010","00010","00010","00100","01000");
        case L'[': return row5(row,"01110","01000","01000","01000","01000","01000","01110");
        case L']': return row5(row,"01110","00010","00010","00010","00010","00010","01110");
        case L'{': return row5(row,"00010","00100","00100","01000","00100","00100","00010");
        case L'}': return row5(row,"01000","00100","00100","00010","00100","00100","01000");
        case L'<': return row5(row,"00010","00100","01000","10000","01000","00100","00010");
        case L'>': return row5(row,"01000","00100","00010","00001","00010","00100","01000");
        case L'\'': return row5(row,"00100","00100","01000","00000","00000","00000","00000");
        case L'\"': return row5(row,"01010","01010","01010","00000","00000","00000","00000");
        case L'`': return row5(row,"01000","00100","00010","00000","00000","00000","00000");
        case L'~': return row5(row,"00000","00000","01000","10101","00010","00000","00000");
        case L'^': return row5(row,"00100","01010","10001","00000","00000","00000","00000");
        default: return row5(row,"11111","10001","00010","00100","00100","00000","00100");
    }
}

static UINT32 gfx_pack_rgb(UINT8 r, UINT8 g, UINT8 b) {
    if (g_pixfmt == PixelRedGreenBlueReserved8BitPerColor) return ((UINT32)b << 16) | ((UINT32)g << 8) | r;
    return ((UINT32)r << 16) | ((UINT32)g << 8) | b;
}

static UINT32 gfx_color_from_index(UINTN idx) {
    switch (idx & 0x0F) {
        case EFI_BLUE: return gfx_pack_rgb(0, 0, 170);
        case EFI_GREEN: return gfx_pack_rgb(0, 170, 0);
        case EFI_CYAN: return gfx_pack_rgb(0, 170, 170);
        case EFI_RED: return gfx_pack_rgb(170, 0, 0);
        case EFI_MAGENTA: return gfx_pack_rgb(170, 0, 170);
        case EFI_BROWN: return gfx_pack_rgb(170, 85, 0);
        case EFI_LIGHTGRAY: return gfx_pack_rgb(170, 170, 170);
        case EFI_DARKGRAY: return gfx_pack_rgb(85, 85, 85);
        case EFI_LIGHTBLUE: return gfx_pack_rgb(85, 85, 255);
        case EFI_LIGHTGREEN: return gfx_pack_rgb(85, 255, 85);
        case EFI_LIGHTCYAN: return gfx_pack_rgb(85, 255, 255);
        case EFI_LIGHTRED: return gfx_pack_rgb(255, 85, 85);
        case EFI_YELLOW: return gfx_pack_rgb(255, 255, 85);
        case EFI_WHITE: return gfx_pack_rgb(255, 255, 255);
        default: return gfx_pack_rgb(0, 0, 0);
    }
}

static void gfx_put_pixel(UINTN x, UINTN y, UINT32 color) {
    if (!g_gfx || !g_fb || x >= g_px_w || y >= g_px_h) return;
    g_fb[y * g_pps + x] = color;
}

static void gfx_fill_rect(UINTN x, UINTN y, UINTN w, UINTN h, UINT32 color) {
    if (!g_gfx || !g_fb) return;
    if (x >= g_px_w || y >= g_px_h) return;
    if (x + w > g_px_w) w = g_px_w - x;
    if (y + h > g_px_h) h = g_px_h - y;
    for (UINTN yy = 0; yy < h; yy++) {
        UINT32 *row = g_fb + (y + yy) * g_pps + x;
        for (UINTN xx = 0; xx < w; xx++) row[xx] = color;
    }
}

static void gfx_clear_screen(void) {
    gfx_fill_rect(0, 0, g_px_w, g_px_h, gfx_color_from_index(EFI_BLACK));
    g_cur_col = 0;
    g_cur_row = 0;
}

static void gfx_scroll_up(void) {
    if (!g_gfx || !g_fb || g_px_h <= FONT_H) return;
    UINTN move_rows = g_px_h - FONT_H;
    for (UINTN y = 0; y < move_rows; y++) {
        UINT32 *dst = g_fb + y * g_pps;
        UINT32 *src = g_fb + (y + FONT_H) * g_pps;
        for (UINTN x = 0; x < g_px_w; x++) dst[x] = src[x];
    }
    gfx_fill_rect(0, move_rows, g_px_w, FONT_H, gfx_color_from_index(EFI_BLACK));
}

static void gfx_draw_cell(UINTN col, UINTN row, CHAR16 c) {
    UINTN x0 = col * FONT_W;
    UINTN y0 = row * FONT_H;
    UINT32 fg = gfx_color_from_index(g_attr & 0x0F);
    UINT32 bg = gfx_color_from_index((g_attr >> 4) & 0x0F);
    gfx_fill_rect(x0, y0, FONT_W, FONT_H, bg);
    if (c < 32) return;
    for (int gr = 0; gr < 7; gr++) {
        UINT8 bits = glyph5(c, gr);
        for (int gc = 0; gc < 5; gc++) {
            if (bits & (1u << (4 - gc))) {
                UINTN px = x0 + 1 + (UINTN)gc;
                UINTN py = y0 + 1 + (UINTN)gr * 2;
                gfx_put_pixel(px, py, fg);
                gfx_put_pixel(px, py + 1, fg);
                if (gc < 4) gfx_put_pixel(px + 1, py, fg);
                if (gc < 4) gfx_put_pixel(px + 1, py + 1, fg);
            }
        }
    }
}

static void gfx_draw_caret_at(UINTN col, UINTN row) {
    UINT32 fg = gfx_color_from_index(g_attr & 0x0F);
    UINTN x = col * FONT_W;
    UINTN y = row * FONT_H;
    gfx_fill_rect(x, y + 1, 1, FONT_H - 2, fg);
}

static void gfx_newline(void) {
    g_cur_col = 0;
    g_cur_row++;
    if (g_cur_row >= g_rows) {
        gfx_scroll_up();
        g_cur_row = g_rows - 1;
    }
}

static void gfx_put_char(CHAR16 c) {
    if (c == L'\r') { g_cur_col = 0; return; }
    if (c == L'\n') { gfx_newline(); return; }
    if (c == L'\b') {
        if (g_cur_col > 0) g_cur_col--;
        else if (g_cur_row > 0) { g_cur_row--; g_cur_col = g_cols - 1; }
        return;
    }
    gfx_draw_cell(g_cur_col, g_cur_row, c);
    g_cur_col++;
    if (g_cur_col >= g_cols) gfx_newline();
}

static void print(const CHAR16 *s) {
    if (!s) return;
    if (g_gfx) {
        for (UINTN i = 0; s[i]; i++) gfx_put_char(s[i]);
        return;
    }
    if (g_out) g_out->OutputString(g_out, (CHAR16*)s);
}

static void println(const CHAR16 *s) {
    print(s);
    print(L"\r\n");
}

static void print_uint(UINT64 v) {
    CHAR16 buf[32];
    u_uint_to_str(v, buf, 32);
    print(buf);
}

static void print_spaces(UINTN n) {
    while (n--) print(L" ");
}

static void clear_line(void) {
    if (g_gfx) {
        UINTN col = g_cur_col;
        UINTN row = g_cur_row;
        if (row < g_rows && col < g_cols) {
            gfx_fill_rect(col * FONT_W, row * FONT_H, (g_cols - col) * FONT_W, FONT_H, gfx_color_from_index(EFI_BLACK));
        }
        return;
    }
    UINTN col = 0;
    if (g_out && g_out->Mode && g_out->Mode->CursorColumn >= 0) col = (UINTN)g_out->Mode->CursorColumn;
    if (g_cols > col + 1) print_spaces(g_cols - col - 1);
}

static void clear_screen(void) {
    if (g_gfx) { gfx_clear_screen(); return; }
    g_out->SetAttribute(g_out, g_attr);
    g_out->ClearScreen(g_out);
}

static void set_cursor(UINTN col, UINTN row) {
    if (col >= g_cols) col = g_cols - 1;
    if (row >= g_rows) row = g_rows - 1;
    if (g_gfx) { g_cur_col = col; g_cur_row = row; return; }
    g_out->SetCursorPosition(g_out, col, row);
}

static void update_screen_size(void) {
    if (g_gfx) {
        if (g_px_w >= FONT_W) g_cols = g_px_w / FONT_W;
        if (g_px_h >= FONT_H) g_rows = g_px_h / FONT_H;
        if (g_cols < 80) g_cols = 80;
        if (g_rows < 25) g_rows = 25;
        g_text_mode = (UINTN)g_gop_current_mode;
        return;
    }
    UINTN cols = 0, rows = 0;
    if (g_out->Mode && g_out->QueryMode(g_out, (UINTN)g_out->Mode->Mode, &cols, &rows) == EFI_SUCCESS) {
        if (cols >= 40) g_cols = cols;
        if (rows >= 20) g_rows = rows;
        g_text_mode = (UINTN)g_out->Mode->Mode;
    }
}

static int init_graphics_console(void) {
    /*
     * Boot-safe GOP activation.
     * v0.3.4 tried to enumerate and switch GOP modes during startup.
     * Some real UEFI firmwares hang or black-screen during QueryMode/SetMode.
     * v0.3.5 does NOT probe/switch modes automatically. It only attaches to
     * the CURRENT framebuffer mode when the user explicitly runs `mode large`
     * or `mode auto`. This keeps real UEFI boot reliable.
     */
    if (!g_bs || !g_bs->LocateProtocol) return 0;
    EFI_GUID gop_guid = {0x9042a9de, 0x23dc, 0x4a38, {0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}};
    VOID *iface = NULL;
    EFI_LOCATE_PROTOCOL locate = (EFI_LOCATE_PROTOCOL)g_bs->LocateProtocol;
    if (locate(&gop_guid, NULL, &iface) != EFI_SUCCESS || !iface) return 0;
    g_gop = (EFI_GRAPHICS_OUTPUT_PROTOCOL*)iface;
    if (!g_gop || !g_gop->Mode || !g_gop->Mode->Info || !g_gop->Mode->FrameBufferBase) return 0;

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *cur = g_gop->Mode->Info;
    if (cur->PixelFormat == PixelBltOnly) return 0;
    if (cur->HorizontalResolution < 640 || cur->VerticalResolution < 400) return 0;

    g_pixfmt = cur->PixelFormat;
    g_px_w = cur->HorizontalResolution;
    g_px_h = cur->VerticalResolution;
    g_pps = cur->PixelsPerScanLine;
    g_fb = (UINT32*)(UINTN)g_gop->Mode->FrameBufferBase;
    g_gop_current_mode = g_gop->Mode->Mode;

    g_gop_mode_count = 1;
    g_gop_modes[0].mode = g_gop_current_mode;
    g_gop_modes[0].width = g_px_w;
    g_gop_modes[0].height = g_px_h;
    g_gop_modes[0].pps = g_pps;

    g_gfx = 1;
    update_screen_size();
    gfx_clear_screen();
    return 1;
}

/* Firmware text-mode probing intentionally removed in v0.3.5. */
static EFI_INPUT_KEY read_key(void) {
    EFI_INPUT_KEY key;
    key.ScanCode = 0;
    key.UnicodeChar = 0;
    UINTN index = 0;
    g_bs->WaitForEvent(1, &g_in->WaitForKey, &index);
    g_in->ReadKeyStroke(g_in, &key);
    return key;
}

static int key_available(void) {
    if (!g_bs->CheckEvent) return 0;
    return g_bs->CheckEvent(g_in->WaitForKey) == EFI_SUCCESS;
}

static void drain_keys(void) {
    EFI_INPUT_KEY key;
    int guard = 64;
    while (key_available() && guard-- > 0) g_in->ReadKeyStroke(g_in, &key);
}

static void prompt(void) {
    g_out->SetAttribute(g_out, g_attr);
    print(g_username);
    print(L"@sacramentuOS> ");
}

static int read_line(CHAR16 *buf, UINTN max) {
    UINTN len = 0;
    buf[0] = 0;
    while (1) {
        EFI_INPUT_KEY key = read_key();
        if (key.UnicodeChar == 3) {
            println(L"^C");
            buf[0] = 0;
            return -1;
        }
        if (key.UnicodeChar == L'\r' || key.UnicodeChar == L'\n') {
            println(L"");
            buf[len] = 0;
            return (int)len;
        }
        if (key.UnicodeChar == L'\b') {
            if (len > 0) {
                len--;
                buf[len] = 0;
                print(L"\b \b");
            }
            continue;
        }
        if (key.ScanCode == SCAN_DELETE) {
            continue;
        }
        if (key.UnicodeChar >= 32 && key.UnicodeChar < 127) {
            if (len + 1 < max) {
                buf[len++] = key.UnicodeChar;
                buf[len] = 0;
                CHAR16 s[2]; s[0] = key.UnicodeChar; s[1] = 0;
                print(s);
            }
        }
    }
}

static int parse_args(CHAR16 *line) {
    int argc = 0;
    UINTN i = 0;
    while (line[i]) {
        while (line[i] == L' ' || line[i] == L'\t') i++;
        if (!line[i]) break;
        UINTN j = 0;
        while (line[i] && line[i] != L' ' && line[i] != L'\t' && j + 1 < MAX_LINE) {
            g_args_storage[argc][j++] = line[i++];
        }
        g_args_storage[argc][j] = 0;
        g_args[argc] = g_args_storage[argc];
        argc++;
        if (argc >= MAX_ARGS) break;
        while (line[i] && line[i] != L' ' && line[i] != L'\t') i++;
    }
    return argc;
}

static int file_find(const CHAR16 *name) {
    for (int i = 0; i < MAX_FILES; i++) if (g_files[i].used && u_streq(g_files[i].name, name)) return i;
    return -1;
}

static int file_alloc(const CHAR16 *name) {
    int existing = file_find(name);
    if (existing >= 0) return existing;
    for (int i = 0; i < MAX_FILES; i++) {
        if (!g_files[i].used) {
            g_files[i].used = 1;
            u_strcpy(g_files[i].name, name, MAX_FILE_NAME);
            g_files[i].lines = 1;
            g_files[i].text[0][0] = 0;
            return i;
        }
    }
    return -1;
}

static void show_help(void) {
    println(L"SacramentuOS help");
    println(L"core : help docs commands clear clean ver fetch whoami time date mode color exit reboot shutdown");
    println(L"file : edit <file> | files | show/cat <file> | rm <file>");
    println(L"edit : arrows Home End Enter Backspace Delete | F7 save | F8 save+exit | F9 quit | Ctrl+C cancel");
    println(L"look : mode | mode list | mode large/auto | color list | color <0..15|name>");
}

static void show_commands(void) {
    println(L"help docs commands clear clean ver fetch whoami time date mode color");
    println(L"edit files show cat rm exit reboot shutdown");
}

static void cmd_ver(void) {
    println(L"SacramentuOS");
    print  (L"Version     : "); println(SOS_VERSION);
    print  (L"Last build  : "); print(SOS_BUILD_DATE); print(L" "); println(SOS_BUILD_TIME);
    print  (L"Last update : "); println(SOS_LAST_UPDATE);
}

static void cmd_fetch(void) {
    println(L"+----------------------------------------------------------+");
    println(L"|                     SacramentuOS                         |");
    println(L"+----------------------------------------------------------+");
    print  (L"Version   : "); println(SOS_VERSION);
    print  (L"Build     : "); print(SOS_BUILD_DATE); print(L" "); println(SOS_BUILD_TIME);
    print  (L"Updated   : "); println(SOS_LAST_UPDATE);
    print  (L"User      : "); println(g_username);
    print  (L"Console   : mode "); print_uint(g_text_mode); print(L", "); print_uint(g_cols); print(L"x"); print_uint(g_rows); println(L" text cells");
    println(L"Boot      : native x86_64 BOOTX64.EFI, no GRUB/CSM/Legacy");
    println(L"Input     : UEFI keyboard protocol, Ctrl+C supported");
    println(L"Editor    : RAM editor, F7/F8/F9, arrows/Home/End");
}

static void cmd_time(void) {
    if (!g_rt || !g_rt->GetTime) { println(L"Runtime time service is unavailable."); return; }
    EFI_TIME t;
    if (g_rt->GetTime(&t, NULL) != EFI_SUCCESS) { println(L"Cannot read UEFI time."); return; }
    print_uint(t.Year); print(L"-");
    if (t.Month < 10) print(L"0"); print_uint(t.Month); print(L"-");
    if (t.Day < 10) print(L"0"); print_uint(t.Day); print(L" ");
    if (t.Hour < 10) print(L"0"); print_uint(t.Hour); print(L":");
    if (t.Minute < 10) print(L"0"); print_uint(t.Minute); print(L":");
    if (t.Second < 10) print(L"0"); print_uint(t.Second); println(L"");
}



static void cmd_mode(int argc) {
    if (argc < 2) {
        if (g_gfx) {
            print(L"Current graphics console: mode "); print_uint(g_gop_current_mode);
            print(L" ("); print_uint(g_px_w); print(L"x"); print_uint(g_px_h);
            print(L", console "); print_uint(g_cols); print(L"x"); print_uint(g_rows); println(L")");
        } else {
            print(L"Current safe firmware text console: "); print_uint(g_cols); print(L"x"); print_uint(g_rows); println(L"");
            println(L"UEFI boot-safe text mode is used by default for maximum compatibility.");
        }
        println(L"Usage: mode list | mode large | mode auto | mode safe");
        return;
    }

    if (u_streq_ci(g_args[1], L"safe") || u_streq_ci(g_args[1], L"text")) {
        g_gfx = 0;
        g_cols = 80;
        g_rows = 25;
        g_text_mode = 0;
        if (g_out) {
            g_out->SetAttribute(g_out, g_attr);
            g_out->ClearScreen(g_out);
        }
        println(L"Returned to safe firmware text console.");
        return;
    }

    if (u_streq_ci(g_args[1], L"large") || u_streq_ci(g_args[1], L"gop") || u_streq_ci(g_args[1], L"auto")) {
        if (init_graphics_console()) {
            print(L"Large graphics console enabled: "); print_uint(g_px_w); print(L"x"); print_uint(g_px_h);
            print(L" -> "); print_uint(g_cols); print(L"x"); print_uint(g_rows); println(L" cells");
            println(L"This uses the current firmware GOP framebuffer without unsafe mode probing.");
        } else {
            println(L"GOP framebuffer is unavailable on this firmware. Safe text mode remains active.");
        }
        return;
    }

    if (u_streq_ci(g_args[1], L"list")) {
        if (g_gfx) {
            println(L"Available safe graphics console mode:");
            print(L"mode "); print_uint(g_gop_current_mode); print(L" = ");
            print_uint(g_px_w); print(L"x"); print_uint(g_px_h);
            print(L"  console "); print_uint(g_cols); print(L"x"); print_uint(g_rows); println(L"  current");
            println(L"Unsafe GOP QueryMode/SetMode probing is disabled to prevent UEFI freezes.");
        } else {
            println(L"Safe firmware text console is active.");
            print(L"Current console: "); print_uint(g_cols); print(L"x"); print_uint(g_rows); println(L"");
            println(L"For a larger monitor-fitted console, run: mode large");
            println(L"mode large uses the current UEFI GOP framebuffer and avoids unsafe mode probing.");
        }
        return;
    }

    println(L"Unsupported mode command. Use: mode list | mode large | mode auto | mode safe");
}

static void color_list(void) {
    static const CHAR16 *names[16] = {
        L"black", L"blue", L"green", L"cyan", L"red", L"magenta", L"brown", L"light-grey",
        L"dark-grey", L"light-blue", L"light-green", L"light-cyan", L"light-red", L"light-magenta", L"amber", L"white"
    };
    println(L"Color codes:");
    for (int i = 0; i < 16; i++) {
        print_uint((UINT64)i); print(L" = "); println(names[i]);
    }
}

static int color_index_from_name(const CHAR16 *name, UINTN *out) {
    if (u_streq_ci(name, L"black")) *out = EFI_BLACK;
    else if (u_streq_ci(name, L"blue")) *out = EFI_LIGHTBLUE;
    else if (u_streq_ci(name, L"green")) *out = EFI_LIGHTGREEN;
    else if (u_streq_ci(name, L"cyan")) *out = EFI_LIGHTCYAN;
    else if (u_streq_ci(name, L"red")) *out = EFI_LIGHTRED;
    else if (u_streq_ci(name, L"magenta")) *out = EFI_MAGENTA;
    else if (u_streq_ci(name, L"amber") || u_streq_ci(name, L"yellow")) *out = EFI_YELLOW;
    else if (u_streq_ci(name, L"white")) *out = EFI_WHITE;
    else if (u_streq_ci(name, L"grey") || u_streq_ci(name, L"gray") || u_streq_ci(name, L"light-grey")) *out = EFI_LIGHTGRAY;
    else return 0;
    return 1;
}

static void cmd_color(int argc) {
    if (argc < 2) {
        println(L"Usage: color list | color <0..15|green|amber|white|cyan|red|blue|magenta|grey>");
        return;
    }
    if (u_streq_ci(g_args[1], L"list")) { color_list(); return; }
    UINTN fg = 0;
    if (g_args[1][0] >= L'0' && g_args[1][0] <= L'9') {
        int code = u_atoi(g_args[1]);
        if (code < 0 || code > 15) { println(L"Color number must be 0..15. Use: color list"); return; }
        fg = (UINTN)code;
    } else if (!color_index_from_name(g_args[1], &fg)) {
        println(L"Unknown color. Use: color list");
        return;
    }
    g_attr = EFI_TEXT_ATTR(fg, EFI_BLACK);
    g_out->SetAttribute(g_out, g_attr);
    println(L"Color updated.");
}




static void cmd_files(void) {
    int count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (g_files[i].used) {
            print(g_files[i].name); print(L"  lines="); print_uint(g_files[i].lines); println(L"");
            count++;
        }
    }
    if (!count) println(L"No RAM files.");
}

static void cmd_show(int argc) {
    if (argc < 2) { println(L"Usage: show <file>"); return; }
    int id = file_find(g_args[1]);
    if (id < 0) { println(L"File not found."); return; }
    for (int i = 0; i < g_files[id].lines; i++) {
        print_uint((UINT64)(i + 1)); print(L": "); println(g_files[id].text[i]);
    }
}

static void cmd_rm(int argc) {
    if (argc < 2) { println(L"Usage: rm <file>"); return; }
    int id = file_find(g_args[1]);
    if (id < 0) { println(L"File not found."); return; }
    g_files[id].used = 0;
    println(L"Removed.");
}




static void e_set_status(const CHAR16 *s) { u_strcpy(e_status, s, 80); }

static int e_visible_rows(void) {
    int visible = (int)g_rows - 3;
    if (visible < 5) visible = 5;
    return visible;
}

static int e_visible_cols(void) {
    int visible = (int)g_cols - 6;
    if (visible < 20) visible = 20;
    return visible;
}

static void e_clamp_cursor(void) {
    if (e_cy < 0) e_cy = 0;
    if (e_cy >= e_line_count) e_cy = e_line_count - 1;
    if (e_cy < 0) { e_line_count = 1; e_cy = 0; e_lines[0][0] = 0; }
    int len = (int)u_strlen(e_lines[e_cy]);
    if (e_cx < 0) e_cx = 0;
    if (e_cx > len) e_cx = len;
    int visible = e_visible_rows();
    if (e_cy < e_top) e_top = e_cy;
    if (e_cy >= e_top + visible) e_top = e_cy - visible + 1;
    if (e_top < 0) e_top = 0;
    int vcols = e_visible_cols();
    if (e_cx < e_left) e_left = e_cx;
    if (e_cx >= e_left + vcols) e_left = e_cx - vcols + 1;
    if (e_left < 0) e_left = 0;
}

static void e_draw_header(void) {
    set_cursor(0, 0);
    print(L"SacramentuOS Editor | "); print(e_name);
    print(L" | F7 save  F8 save+exit  F9 quit  Ctrl+C");
    clear_line();
    set_cursor(0, 1);
    UINTN limit = g_cols > 0 ? g_cols - 1 : 79;
    for (UINTN i = 0; i < limit; i++) print(L"-");
}

static void e_draw_line(int screen_row) {
    int li = e_top + screen_row;
    set_cursor(0, (UINTN)(2 + screen_row));
    if (li >= e_line_count) { print(L"~"); clear_line(); return; }
    if (li + 1 < 10) print(L" ");
    print_uint((UINT64)(li + 1));
    print(L" | ");
    int max_chars = e_visible_cols();
    int n = (int)u_strlen(e_lines[li]);
    for (int i = 0; i < max_chars; i++) {
        int idx = e_left + i;
        if (idx < n) { CHAR16 ch[2]; ch[0] = e_lines[li][idx]; ch[1] = 0; print(ch); }
        else print(L" ");
    }
    clear_line();
}

static void e_draw_status(void) {
    set_cursor(0, g_rows - 1);
    print(L"Ln "); print_uint((UINT64)(e_cy + 1)); print(L", Col "); print_uint((UINT64)(e_cx + 1));
    print(L" | ");
    if (e_dirty) print(L"modified"); else print(L"saved");
    print(L" | mode "); print_uint(g_text_mode); print(L" "); print_uint(g_cols); print(L"x"); print_uint(g_rows);
    print(L" | "); print(e_status);
    clear_line();
}

static void e_place_cursor(void) {
    e_clamp_cursor();
    int col = 5 + e_cx - e_left;
    int row = 2 + e_cy - e_top;
    if (col < 5) col = 5;
    if (row < 2) row = 2;
    set_cursor((UINTN)col, (UINTN)row);
    if (g_gfx) gfx_draw_caret_at((UINTN)col, (UINTN)row);
}

static void e_redraw(void) {
    g_out->SetAttribute(g_out, g_attr);
    e_clamp_cursor();
    e_draw_header();
    int visible = e_visible_rows();
    for (int row = 0; row < visible; row++) e_draw_line(row);
    e_draw_status();
    e_place_cursor();
}

static void e_insert_char(CHAR16 c) {
    int len = (int)u_strlen(e_lines[e_cy]);
    if (len + 1 >= MAX_FILE_COLS) { e_set_status(L"line limit reached"); return; }
    for (int i = len; i >= e_cx; i--) e_lines[e_cy][i + 1] = e_lines[e_cy][i];
    e_lines[e_cy][e_cx] = c;
    e_cx++;
    e_dirty = 1;
}

static void e_enter(void) {
    if (e_line_count >= MAX_FILE_LINES) { e_set_status(L"file line limit reached"); return; }
    int len = (int)u_strlen(e_lines[e_cy]);
    for (int l = e_line_count; l > e_cy + 1; l--) u_strcpy(e_lines[l], e_lines[l - 1], MAX_FILE_COLS);
    for (int i = 0; i < MAX_FILE_COLS; i++) e_lines[e_cy + 1][i] = 0;
    int j = 0;
    for (int i = e_cx; i <= len && j + 1 < MAX_FILE_COLS; i++) e_lines[e_cy + 1][j++] = e_lines[e_cy][i];
    e_lines[e_cy + 1][j] = 0;
    e_lines[e_cy][e_cx] = 0;
    e_line_count++;
    e_cy++; e_cx = 0;
    e_dirty = 1;
}

static void e_backspace(void) {
    if (e_cx > 0) {
        int len = (int)u_strlen(e_lines[e_cy]);
        for (int i = e_cx - 1; i <= len; i++) e_lines[e_cy][i] = e_lines[e_cy][i + 1];
        e_cx--; e_dirty = 1; return;
    }
    if (e_cy > 0) {
        int prev_len = (int)u_strlen(e_lines[e_cy - 1]);
        int cur_len = (int)u_strlen(e_lines[e_cy]);
        if (prev_len + cur_len + 1 >= MAX_FILE_COLS) { e_set_status(L"cannot merge: line too long"); return; }
        u_strcat(e_lines[e_cy - 1], e_lines[e_cy], MAX_FILE_COLS);
        for (int l = e_cy; l < e_line_count - 1; l++) u_strcpy(e_lines[l], e_lines[l + 1], MAX_FILE_COLS);
        e_line_count--; e_cy--; e_cx = prev_len; e_dirty = 1;
    }
}

static void e_delete(void) {
    int len = (int)u_strlen(e_lines[e_cy]);
    if (e_cx < len) {
        for (int i = e_cx; i <= len; i++) e_lines[e_cy][i] = e_lines[e_cy][i + 1];
        e_dirty = 1; return;
    }
    if (e_cy + 1 < e_line_count) {
        int next_len = (int)u_strlen(e_lines[e_cy + 1]);
        if (len + next_len + 1 >= MAX_FILE_COLS) { e_set_status(L"cannot merge: line too long"); return; }
        u_strcat(e_lines[e_cy], e_lines[e_cy + 1], MAX_FILE_COLS);
        for (int l = e_cy + 1; l < e_line_count - 1; l++) u_strcpy(e_lines[l], e_lines[l + 1], MAX_FILE_COLS);
        e_line_count--; e_dirty = 1;
    }
}

static int e_save(void) {
    int id = file_alloc(e_name);
    if (id < 0) { e_set_status(L"cannot save: RAM file limit"); return 0; }
    g_files[id].lines = e_line_count;
    for (int i = 0; i < e_line_count; i++) u_strcpy(g_files[id].text[i], e_lines[i], MAX_FILE_COLS);
    e_dirty = 0;
    e_set_status(L"saved");
    return 1;
}

static void editor_run(const CHAR16 *name) {
    u_strcpy(e_name, name, MAX_FILE_NAME);
    int id = file_find(name);
    e_line_count = 1;
    for (int i = 0; i < MAX_FILE_LINES; i++) e_lines[i][0] = 0;
    if (id >= 0) {
        e_line_count = g_files[id].lines;
        for (int i = 0; i < e_line_count; i++) u_strcpy(e_lines[i], g_files[id].text[i], MAX_FILE_COLS);
    }
    if (e_line_count < 1) e_line_count = 1;
    e_cx = 0; e_cy = 0; e_top = 0; e_left = 0; e_dirty = 0; e_set_status(L"ready");
    if (g_in && g_in->Reset) g_in->Reset(g_in, 0);
    drain_keys();
    clear_screen();
    while (1) {
        e_clamp_cursor();
        e_redraw();
        EFI_INPUT_KEY key = read_key();
        if (key.UnicodeChar == 3 || key.ScanCode == SCAN_F9) { clear_screen(); println(L"Editor closed without saving."); return; }
        if (key.ScanCode == SCAN_F7) { e_save(); continue; }
        if (key.ScanCode == SCAN_F8) { if (e_save()) { clear_screen(); println(L"Saved and closed."); return; } continue; }
        if (key.ScanCode == SCAN_UP) { e_cy--; e_clamp_cursor(); continue; }
        if (key.ScanCode == SCAN_DOWN) { e_cy++; e_clamp_cursor(); continue; }
        if (key.ScanCode == SCAN_LEFT) { if (e_cx > 0) e_cx--; else if (e_cy > 0) { e_cy--; e_cx = (int)u_strlen(e_lines[e_cy]); } continue; }
        if (key.ScanCode == SCAN_RIGHT) { int len = (int)u_strlen(e_lines[e_cy]); if (e_cx < len) e_cx++; else if (e_cy + 1 < e_line_count) { e_cy++; e_cx = 0; } continue; }
        if (key.ScanCode == SCAN_HOME) { e_cx = 0; continue; }
        if (key.ScanCode == SCAN_END) { e_cx = (int)u_strlen(e_lines[e_cy]); continue; }
        if (key.ScanCode == SCAN_DELETE) { e_delete(); continue; }
        if (key.UnicodeChar == L'\b') { e_backspace(); continue; }
        if (key.UnicodeChar == L'\r' || key.UnicodeChar == L'\n') { e_enter(); continue; }
        if (key.UnicodeChar >= 32 && key.UnicodeChar < 127) { e_insert_char(key.UnicodeChar); continue; }
    }
}


static void execute(int argc) {
    if (argc <= 0) return;
    CHAR16 *cmd = g_args[0];
    if (u_streq_ci(cmd, L"help") || u_streq_ci(cmd, L"docs")) show_help();
    else if (u_streq_ci(cmd, L"commands")) show_commands();
    else if (u_streq_ci(cmd, L"clear") || u_streq_ci(cmd, L"clean")) clear_screen();
    else if (u_streq_ci(cmd, L"ver")) cmd_ver();
    else if (u_streq_ci(cmd, L"fetch")) cmd_fetch();
    else if (u_streq_ci(cmd, L"whoami")) println(g_username);
    else if (u_streq_ci(cmd, L"mode")) cmd_mode(argc);
    else if (u_streq_ci(cmd, L"color")) cmd_color(argc);
    else if (u_streq_ci(cmd, L"time") || u_streq_ci(cmd, L"date")) cmd_time();
    else if (u_streq_ci(cmd, L"files")) cmd_files();
    else if (u_streq_ci(cmd, L"show") || u_streq_ci(cmd, L"cat")) cmd_show(argc);
    else if (u_streq_ci(cmd, L"rm")) cmd_rm(argc);
    else if (u_streq_ci(cmd, L"edit")) { if (argc < 2) println(L"Usage: edit <file>"); else editor_run(g_args[1]); }
    else if (u_streq_ci(cmd, L"reboot")) { if (g_rt && g_rt->ResetSystem) g_rt->ResetSystem(1, EFI_SUCCESS, 0, NULL); }
    else if (u_streq_ci(cmd, L"shutdown")) { if (g_rt && g_rt->ResetSystem) g_rt->ResetSystem(2, EFI_SUCCESS, 0, NULL); }
    else if (u_streq_ci(cmd, L"exit")) { if (g_bs && g_bs->Exit) g_bs->Exit(g_image, EFI_SUCCESS, 0, NULL); }
    else { print(L"Unknown command: "); println(cmd); println(L"Type help or commands."); }
}

static void boot_screen(void) {
    clear_screen();
    println(L"============================================================");
    println(L"                         SacramentuOS");
    println(L"============================================================");
    println(L"[ OK ] Native x86_64 UEFI boot");
    println(L"[ OK ] Firmware console output");
    println(L"[ OK ] USB keyboard through UEFI input protocol");
    println(L"[ OK ] SacramentuOS system is embedded in this EFI application");
    println(L"[ OK ] RAM shell + RAM editor");
    println(L"");
    println(L"Pure UEFI mode: no CSM, no BIOS interrupts, no VGA 0xB8000.");
    print(L"Console: safe firmware text "); print_uint(g_cols); print(L"x"); print_uint(g_rows); println(L". Run mode large for framebuffer console.");
    println(L"");
}

static void ask_username(void) {
    print(L"Choose username: ");
    if (read_line(g_line, sizeof(g_line)/sizeof(g_line[0])) > 0) {
        if (u_strlen(g_line) > 0) u_strcpy(g_username, g_line, 32);
    }
    println(L"");
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    g_image = ImageHandle;
    g_st = SystemTable;
    g_in = SystemTable->ConIn;
    g_out = SystemTable->ConOut;
    g_bs = SystemTable->BootServices;
    g_rt = SystemTable->RuntimeServices;
    if (g_bs && g_bs->SetWatchdogTimer) g_bs->SetWatchdogTimer(0, 0, 0, NULL);
    g_out->Reset(g_out, 0);
    g_in->Reset(g_in, 0);
    /* Boot-safe default: do not probe or switch video modes during startup. */
    g_cols = 80;
    g_rows = 25;
    g_text_mode = 0;
    g_gfx = 0;
    g_out->SetAttribute(g_out, g_attr);
    g_out->ClearScreen(g_out);
    boot_screen();
    ask_username();
    println(L"BOOTX64.EFI contains the SacramentuOS shell and editor.");
    println(L"Type help to open compact documentation. Type edit <file> to open the editor.");
    while (1) {
        prompt();
        int n = read_line(g_line, MAX_LINE);
        if (n < 0) continue;
        int argc = parse_args(g_line);
        execute(argc);
    }
    return EFI_SUCCESS;
}

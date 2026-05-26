// SacramentuOS 1.0.1 native UEFI GOP pixel desktop.
// Uses UEFI Graphics Output Protocol and firmware pointer protocols.

#define NULL ((void*)0)

typedef unsigned char      UINT8;
typedef signed char        INT8;
typedef unsigned short     UINT16;
typedef short              INT16;
typedef unsigned int       UINT32;
typedef int                INT32;
typedef unsigned long long UINT64;
typedef long long          INT64;
typedef unsigned long long UINTN;
typedef long long          INTN;
typedef UINT16             CHAR16;
typedef void               VOID;
typedef VOID*              EFI_HANDLE;
typedef VOID*              EFI_EVENT;
typedef UINT64             EFI_STATUS;
typedef UINT64             EFI_PHYSICAL_ADDRESS;
typedef unsigned char      BOOLEAN;

#define EFI_SUCCESS 0
#define EFI_NOT_READY 6
#define EFI_UNSUPPORTED 3
#define TRUE 1
#define FALSE 0

#define SCAN_NULL        0x00
#define SCAN_UP          0x01
#define SCAN_DOWN        0x02
#define SCAN_RIGHT       0x03
#define SCAN_LEFT        0x04
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

#define KEY_ESC 27
#define KEY_TAB 9
#define KEY_ENTER '\n'
#define KEY_BS '\b'
#define KEY_UP 1001
#define KEY_DOWN 1002
#define KEY_LEFT 1003
#define KEY_RIGHT 1004
#define KEY_F1 1101
#define KEY_F2 1102
#define KEY_F3 1103
#define KEY_F4 1104
#define KEY_F5 1105
#define KEY_F6 1106
#define KEY_F7 1107
#define KEY_F8 1108
#define KEY_F9 1109
#define KEY_F10 1110

#define EFI_TEXT_ATTR(fg,bg) ((UINTN)((fg) | ((bg) << 4)))
#define EFI_WHITE 0x0F
#define EFI_RED 0x04
#define EFI_BLACK 0x00

typedef struct { UINT32 Data1; UINT16 Data2; UINT16 Data3; UINT8 Data4[8]; } EFI_GUID;
typedef struct { UINT64 Signature; UINT32 Revision; UINT32 HeaderSize; UINT32 CRC32; UINT32 Reserved; } EFI_TABLE_HEADER;
typedef struct { UINT16 ScanCode; CHAR16 UnicodeChar; } EFI_INPUT_KEY;

typedef struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef EFI_STATUS (*EFI_INPUT_RESET)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL*, BOOLEAN);
typedef EFI_STATUS (*EFI_INPUT_READ_KEY)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL*, EFI_INPUT_KEY*);
struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL { EFI_INPUT_RESET Reset; EFI_INPUT_READ_KEY ReadKeyStroke; EFI_EVENT WaitForKey; };

typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef EFI_STATUS (*EFI_TEXT_RESET)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, BOOLEAN);
typedef EFI_STATUS (*EFI_TEXT_STRING)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, CHAR16*);
typedef EFI_STATUS (*EFI_TEXT_SET_ATTRIBUTE)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, UINTN);
typedef EFI_STATUS (*EFI_TEXT_CLEAR_SCREEN)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*);
struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_TEXT_RESET Reset; EFI_TEXT_STRING OutputString; VOID* TestString; VOID* QueryMode; VOID* SetMode;
    EFI_TEXT_SET_ATTRIBUTE SetAttribute; EFI_TEXT_CLEAR_SCREEN ClearScreen; VOID* SetCursorPosition; VOID* EnableCursor; VOID* Mode;
};

typedef EFI_STATUS (*EFI_STALL)(UINTN);
typedef EFI_STATUS (*EFI_SET_WATCHDOG_TIMER)(UINTN, UINT64, UINTN, CHAR16*);
typedef EFI_STATUS (*EFI_LOCATE_PROTOCOL)(EFI_GUID*, VOID*, VOID**);
typedef EFI_STATUS (*EFI_HANDLE_PROTOCOL)(EFI_HANDLE, EFI_GUID*, VOID**);
typedef EFI_STATUS (*EFI_LOCATE_HANDLE_BUFFER)(UINTN, EFI_GUID*, VOID*, UINTN*, EFI_HANDLE**);
typedef EFI_STATUS (*EFI_FREE_POOL)(VOID*);
#define EFI_BY_PROTOCOL 2

typedef struct EFI_BOOT_SERVICES {
    EFI_TABLE_HEADER Hdr;
    VOID *RaiseTPL; VOID *RestoreTPL; VOID *AllocatePages; VOID *FreePages; VOID *GetMemoryMap; VOID *AllocatePool; VOID *FreePool;
    VOID *CreateEvent; VOID *SetTimer; VOID *WaitForEvent; VOID *SignalEvent; VOID *CloseEvent; VOID *CheckEvent;
    VOID *InstallProtocolInterface; VOID *ReinstallProtocolInterface; VOID *UninstallProtocolInterface; VOID *HandleProtocol;
    VOID *Reserved; VOID *RegisterProtocolNotify; VOID *LocateHandle; VOID *LocateDevicePath; VOID *InstallConfigurationTable;
    VOID *LoadImage; VOID *StartImage; VOID *Exit; VOID *UnloadImage; VOID *ExitBootServices; VOID *GetNextMonotonicCount;
    EFI_STALL Stall; EFI_SET_WATCHDOG_TIMER SetWatchdogTimer; VOID *ConnectController; VOID *DisconnectController;
    VOID *OpenProtocol; VOID *CloseProtocol; VOID *OpenProtocolInformation; VOID *ProtocolsPerHandle; VOID *LocateHandleBuffer;
    EFI_LOCATE_PROTOCOL LocateProtocol;
    VOID *InstallMultipleProtocolInterfaces; VOID *UninstallMultipleProtocolInterfaces; VOID *CalculateCrc32; VOID *CopyMem; VOID *SetMem; VOID *CreateEventEx;
} EFI_BOOT_SERVICES;

typedef VOID (*EFI_RESET_SYSTEM)(INTN, EFI_STATUS, UINTN, CHAR16*);
typedef struct EFI_RUNTIME_SERVICES { EFI_TABLE_HEADER Hdr; VOID* GetTime; VOID* SetTime; VOID* GetWakeupTime; VOID* SetWakeupTime; VOID* SetVirtualAddressMap; VOID* ConvertPointer; VOID* GetVariable; VOID* GetNextVariableName; VOID* SetVariable; VOID* GetNextVariableName2; VOID* ResetSystem; } EFI_RUNTIME_SERVICES_MIN;

typedef struct EFI_SYSTEM_TABLE {
    EFI_TABLE_HEADER Hdr; CHAR16 *FirmwareVendor; UINT32 FirmwareRevision; EFI_HANDLE ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn; EFI_HANDLE ConsoleOutHandle; EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
    EFI_HANDLE StandardErrorHandle; EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr; EFI_RUNTIME_SERVICES_MIN *RuntimeServices;
    EFI_BOOT_SERVICES *BootServices; UINTN NumberOfTableEntries; VOID *ConfigurationTable;
} EFI_SYSTEM_TABLE;

typedef struct { UINT32 RedMask; UINT32 GreenMask; UINT32 BlueMask; UINT32 ReservedMask; } EFI_PIXEL_BITMASK;
typedef enum { PixelRedGreenBlueReserved = 0, PixelBlueGreenRedReserved = 1, PixelBitMask = 2, PixelBltOnly = 3 } EFI_GRAPHICS_PIXEL_FORMAT;
typedef struct { UINT32 Version; UINT32 HorizontalResolution; UINT32 VerticalResolution; EFI_GRAPHICS_PIXEL_FORMAT PixelFormat; EFI_PIXEL_BITMASK PixelInformation; UINT32 PixelsPerScanLine; } EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;
typedef struct { UINT32 MaxMode; UINT32 Mode; EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info; UINTN SizeOfInfo; EFI_PHYSICAL_ADDRESS FrameBufferBase; UINTN FrameBufferSize; } EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;
typedef struct EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;
typedef EFI_STATUS (*EFI_GOP_QUERY_MODE)(EFI_GRAPHICS_OUTPUT_PROTOCOL*, UINT32, UINTN*, EFI_GRAPHICS_OUTPUT_MODE_INFORMATION**);
typedef EFI_STATUS (*EFI_GOP_SET_MODE)(EFI_GRAPHICS_OUTPUT_PROTOCOL*, UINT32);
typedef EFI_STATUS (*EFI_GOP_BLT)(EFI_GRAPHICS_OUTPUT_PROTOCOL*, VOID*, UINTN, UINTN, UINTN, UINTN, UINTN, UINTN, UINTN, UINTN);
struct EFI_GRAPHICS_OUTPUT_PROTOCOL { EFI_GOP_QUERY_MODE QueryMode; EFI_GOP_SET_MODE SetMode; EFI_GOP_BLT Blt; EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode; };

typedef struct EFI_SIMPLE_POINTER_PROTOCOL EFI_SIMPLE_POINTER_PROTOCOL;
typedef struct { UINT64 ResolutionX; UINT64 ResolutionY; UINT64 ResolutionZ; BOOLEAN LeftButton; BOOLEAN RightButton; } EFI_SIMPLE_POINTER_MODE;
typedef struct { INT32 RelativeMovementX; INT32 RelativeMovementY; INT32 RelativeMovementZ; BOOLEAN LeftButton; BOOLEAN RightButton; } EFI_SIMPLE_POINTER_STATE;
typedef EFI_STATUS (*EFI_SIMPLE_POINTER_RESET)(EFI_SIMPLE_POINTER_PROTOCOL*, BOOLEAN);
typedef EFI_STATUS (*EFI_SIMPLE_POINTER_GET_STATE)(EFI_SIMPLE_POINTER_PROTOCOL*, EFI_SIMPLE_POINTER_STATE*);
struct EFI_SIMPLE_POINTER_PROTOCOL { EFI_SIMPLE_POINTER_RESET Reset; EFI_SIMPLE_POINTER_GET_STATE GetState; EFI_EVENT WaitForInput; EFI_SIMPLE_POINTER_MODE *Mode; };

typedef struct EFI_ABSOLUTE_POINTER_PROTOCOL EFI_ABSOLUTE_POINTER_PROTOCOL;
typedef struct { UINT64 AbsoluteMinX; UINT64 AbsoluteMinY; UINT64 AbsoluteMinZ; UINT64 AbsoluteMaxX; UINT64 AbsoluteMaxY; UINT64 AbsoluteMaxZ; UINT32 Attributes; } EFI_ABSOLUTE_POINTER_MODE;
typedef struct { UINT64 CurrentX; UINT64 CurrentY; UINT64 CurrentZ; UINT32 ActiveButtons; } EFI_ABSOLUTE_POINTER_STATE;
typedef EFI_STATUS (*EFI_ABSOLUTE_POINTER_RESET)(EFI_ABSOLUTE_POINTER_PROTOCOL*, BOOLEAN);
typedef EFI_STATUS (*EFI_ABSOLUTE_POINTER_GET_STATE)(EFI_ABSOLUTE_POINTER_PROTOCOL*, EFI_ABSOLUTE_POINTER_STATE*);
struct EFI_ABSOLUTE_POINTER_PROTOCOL { EFI_ABSOLUTE_POINTER_RESET Reset; EFI_ABSOLUTE_POINTER_GET_STATE GetState; EFI_EVENT WaitForInput; EFI_ABSOLUTE_POINTER_MODE *Mode; };

static EFI_GUID GOP_GUID = {0x9042a9de,0x23dc,0x4a38,{0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}};
static EFI_GUID SIMPLE_POINTER_GUID = {0x31878c87,0x0b75,0x11d5,{0x9a,0x4f,0x00,0x90,0x27,0x3f,0xc1,0x4d}};
static EFI_GUID ABS_POINTER_GUID = {0x8d59d32b,0xc655,0x4ae9,{0x9b,0x15,0xf2,0x59,0x04,0x99,0x2a,0x43}};

void *memcpy(void *dst, const void *src, UINTN n){UINT8*d=(UINT8*)dst;const UINT8*s=(const UINT8*)src;while(n--)*d++=*s++;return dst;}
static EFI_SYSTEM_TABLE* st;
static EFI_BOOT_SERVICES* bs;
static EFI_SIMPLE_TEXT_INPUT_PROTOCOL* conin;
static EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conout;
static EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
static EFI_SIMPLE_POINTER_PROTOCOL* sp;
static EFI_ABSOLUTE_POINTER_PROTOCOL* ap;
static UINT8* fb;
static UINT8* drawfb;
static UINT32 W, H, PITCH, PIXFMT;
static EFI_PIXEL_BITMASK MASK;
#define BACK_MAX_W 1280u
#define BACK_MAX_H 1024u
static UINT32 backbuffer[BACK_MAX_W * BACK_MAX_H];

static int ps2_ok=0, ps2_cycle=0;
static UINT8 ps2_packet[3];
static const char* pointer_backend="none";
static int mx=400,my=300,mb=0;

static UINT32 C_DESK1,C_DESK2,C_PANEL,C_TEXT,C_WHITE,C_BLACK,C_BLUE,C_ACCENT,C_WINDOW,C_BORDER,C_TITLE,C_INACTIVE,C_SHADOW,C_SELECT,C_ERROR,C_GOOD,C_CON,C_CONTXT;

static void fail_text(CHAR16* msg){ if(conout){ conout->SetAttribute(conout, EFI_TEXT_ATTR(EFI_WHITE,EFI_RED)); conout->OutputString(conout,msg); } }
static void stall(UINTN us){ if(bs && bs->Stall) bs->Stall(us); }
static UINT32 mask_shift(UINT32 m){ UINT32 s=0; if(!m) return 0; while(((m>>s)&1u)==0 && s<32) s++; return s; }
static UINT32 mask_bits(UINT32 m){ UINT32 n=0; while(m){ if(m&1u)n++; m>>=1; } return n; }
static UINT32 scale(UINT8 v, UINT32 bits){ if(bits>=8) return v; if(!bits) return 0; return (UINT32)(v>>(8-bits)); }
static UINT32 rgb(UINT8 r, UINT8 g, UINT8 b){
    if(PIXFMT==PixelRedGreenBlueReserved) return (UINT32)r | ((UINT32)g<<8) | ((UINT32)b<<16);
    if(PIXFMT==PixelBlueGreenRedReserved) return (UINT32)b | ((UINT32)g<<8) | ((UINT32)r<<16);
    if(PIXFMT==PixelBitMask){ return (scale(r,mask_bits(MASK.RedMask))<<mask_shift(MASK.RedMask)) | (scale(g,mask_bits(MASK.GreenMask))<<mask_shift(MASK.GreenMask)) | (scale(b,mask_bits(MASK.BlueMask))<<mask_shift(MASK.BlueMask)); }
    return (UINT32)b | ((UINT32)g<<8) | ((UINT32)r<<16);
}


static UINT8 port_in8(UINT16 port){ UINT8 v; __asm__ __volatile__("inb %1, %0" : "=a"(v) : "Nd"(port)); return v; }
static void port_out8(UINT16 port, UINT8 v){ __asm__ __volatile__("outb %0, %1" :: "a"(v), "Nd"(port)); }
static UINT32 port_in32(UINT16 port){ UINT32 v; __asm__ __volatile__("inl %1, %0" : "=a"(v) : "Nd"(port)); return v; }
static void port_out32(UINT16 port, UINT32 v){ __asm__ __volatile__("outl %0, %1" :: "a"(v), "Nd"(port)); }
static int ps2_wait_input_clear(void){ for(int i=0;i<200000;i++){ if((port_in8(0x64)&0x02)==0) return 1; } return 0; }
static int ps2_wait_output_full(void){ for(int i=0;i<200000;i++){ if(port_in8(0x64)&0x01) return 1; } return 0; }
static void ps2_drain(void){ for(int i=0;i<64;i++){ if(!(port_in8(0x64)&0x01)) break; (void)port_in8(0x60); } }
static int ps2_read_byte(UINT8* out){ if(!ps2_wait_output_full()) return 0; *out=port_in8(0x60); return 1; }
static void ps2_dev_write(UINT8 v){ ps2_wait_input_clear(); port_out8(0x64,0xD4); ps2_wait_input_clear(); port_out8(0x60,v); }
static void ps2_controller_write(UINT8 v){ ps2_wait_input_clear(); port_out8(0x64,v); }
static int ps2_init_polling(void){
    UINT8 cfg=0;
    ps2_drain();
    ps2_controller_write(0xA8); /* enable second PS/2 port */
    ps2_controller_write(0x20); if(ps2_read_byte(&cfg)){ cfg &= (UINT8)~0x20; cfg &= (UINT8)~0x02; ps2_controller_write(0x60); ps2_wait_input_clear(); port_out8(0x60,cfg); }
    ps2_dev_write(0xFF); UINT8 ack=0; ps2_read_byte(&ack); /* reset if a PS/2 mouse exists */
    for(int i=0;i<4;i++){ UINT8 tmp; if(!ps2_read_byte(&tmp)) break; if(tmp==0xAA) break; }
    ps2_dev_write(0xF6); ps2_read_byte(&ack); /* defaults */
    ps2_dev_write(0xF4); ps2_read_byte(&ack); /* enable streaming */
    ps2_cycle=0;
    return 1;
}
static void ps2_handle_packet_byte(UINT8 d){
    if(ps2_cycle==0 && !(d&0x08)) return;
    ps2_packet[ps2_cycle++]=d;
    if(ps2_cycle<3) return;
    ps2_cycle=0;
    int dx=(INT8)ps2_packet[1];
    int dy=(INT8)ps2_packet[2];
    if(ps2_packet[0]&0x40) dx=0;
    if(ps2_packet[0]&0x80) dy=0;
    mx += dx;
    my -= dy;
    if(mx<0)mx=0;if(my<0)my=0;if(mx>=(int)W)mx=(int)W-1;if(my>=(int)H)my=(int)H-1;
    mb = (ps2_packet[0] & 0x01) ? 1 : 0;
}
static int ps2_poll_mouse(void){
    int changed=0;
    if(!ps2_ok) return 0;
    for(int i=0;i<32;i++){
        UINT8 stt=port_in8(0x64);
        if(!(stt&0x01)) break;
        if(!(stt&0x20)) break; /* keep keyboard bytes for firmware */
        UINT8 d=port_in8(0x60);
        ps2_handle_packet_byte(d);
        changed=1;
    }
    return changed;
}
static VOID* locate_protocol_any(EFI_GUID* guid){
    VOID* out=NULL;
    if(bs->LocateProtocol && bs->LocateProtocol(guid,NULL,&out)==EFI_SUCCESS && out) return out;
    EFI_LOCATE_HANDLE_BUFFER locate=(EFI_LOCATE_HANDLE_BUFFER)bs->LocateHandleBuffer;
    EFI_HANDLE_PROTOCOL handle_protocol=(EFI_HANDLE_PROTOCOL)bs->HandleProtocol;
    EFI_FREE_POOL free_pool=(EFI_FREE_POOL)bs->FreePool;
    if(locate && handle_protocol){
        UINTN count=0; EFI_HANDLE* handles=NULL;
        if(locate(EFI_BY_PROTOCOL,guid,NULL,&count,&handles)==EFI_SUCCESS && handles){
            for(UINTN i=0;i<count;i++){
                out=NULL;
                if(handle_protocol(handles[i],guid,&out)==EFI_SUCCESS && out) break;
            }
            if(free_pool) free_pool(handles);
        }
    }
    return out;
}
static void init_pointer_backends(void){
    ap=(EFI_ABSOLUTE_POINTER_PROTOCOL*)locate_protocol_any(&ABS_POINTER_GUID);
    if(ap && ap->Reset(ap,FALSE)==EFI_SUCCESS) pointer_backend="absolute pointer"; else ap=NULL;
    sp=(EFI_SIMPLE_POINTER_PROTOCOL*)locate_protocol_any(&SIMPLE_POINTER_GUID);
    if(sp && sp->Reset(sp,FALSE)==EFI_SUCCESS){ if(!ap) pointer_backend="simple pointer"; } else sp=NULL;
    ps2_ok=ps2_init_polling();
    if(ps2_ok && !ap && !sp) pointer_backend="raw ps/2 polling";
    if(ps2_ok && (ap || sp)) pointer_backend="uefi pointer + ps/2 fallback";
}

static void putpx(int x,int y,UINT32 c){ if(x<0||y<0||(UINT32)x>=W||(UINT32)y>=H) return; ((UINT32*)drawfb)[(UINTN)y*(UINTN)W+(UINTN)x]=c; }
static void fill(int x,int y,int w,int h,UINT32 c){ if(w<=0||h<=0)return; if(x<0){w+=x;x=0;} if(y<0){h+=y;y=0;} if(x+w>(int)W)w=(int)W-x; if(y+h>(int)H)h=(int)H-y; if(w<=0||h<=0)return; for(int yy=0;yy<h;yy++){ UINT32* row=((UINT32*)drawfb)+(UINTN)(y+yy)*(UINTN)W+(UINTN)x; for(int xx=0;xx<w;xx++) row[xx]=c; } }
static void present(void){ if(!fb||!drawfb)return; for(UINT32 y=0;y<H;y++){ volatile UINT32* dst=(volatile UINT32*)(fb+((UINTN)y*(UINTN)PITCH)*4); UINT32* src=((UINT32*)drawfb)+(UINTN)y*(UINTN)W; for(UINT32 x=0;x<W;x++) dst[x]=src[x]; } }
static void rect(int x,int y,int w,int h,UINT32 c){ fill(x,y,w,1,c); fill(x,y+h-1,w,1,c); fill(x,y,1,h,c); fill(x+w-1,y,1,h,c); }
static int abs_i(int v){return v<0?-v:v;}
static void line(int x0,int y0,int x1,int y1,UINT32 c){int dx=abs_i(x1-x0),sx=x0<x1?1:-1;int dy=-abs_i(y1-y0),sy=y0<y1?1:-1;int err=dx+dy;for(;;){putpx(x0,y0,c);if(x0==x1&&y0==y1)break;int e2=2*err;if(e2>=dy){err+=dy;x0+=sx;}if(e2<=dx){err+=dx;y0+=sy;}}}

static const UINT8* glyph(char ch){
 static const UINT8 sp[7]={0,0,0,0,0,0,0}, unk[7]={14,17,1,6,4,0,4}, dot[7]={0,0,0,0,0,0,4}, colon[7]={0,4,4,0,4,4,0}, dash[7]={0,0,0,31,0,0,0}, slash[7]={1,2,4,8,16,0,0}, bsx[7]={16,8,4,2,1,0,0};
 static const UINT8 zero[7]={14,17,19,21,25,17,14},one[7]={4,12,4,4,4,4,14},two[7]={14,17,1,2,4,8,31},three[7]={30,1,1,14,1,1,30},four[7]={2,6,10,18,31,2,2},five[7]={31,16,16,30,1,1,30},six[7]={6,8,16,30,17,17,14},seven[7]={31,1,2,4,8,8,8},eight[7]={14,17,17,14,17,17,14},nine[7]={14,17,17,15,1,2,12};
 static const UINT8 A[7]={14,17,17,31,17,17,17},B[7]={30,17,17,30,17,17,30},C[7]={14,17,16,16,16,17,14},D[7]={30,17,17,17,17,17,30},E[7]={31,16,16,30,16,16,31},F[7]={31,16,16,30,16,16,16},G[7]={14,17,16,23,17,17,15},Hh[7]={17,17,17,31,17,17,17},I[7]={14,4,4,4,4,4,14},J[7]={7,2,2,2,18,18,12},K[7]={17,18,20,24,20,18,17},L[7]={16,16,16,16,16,16,31},M[7]={17,27,21,21,17,17,17},N[7]={17,25,21,19,17,17,17},O[7]={14,17,17,17,17,17,14},P[7]={30,17,17,30,16,16,16},Q[7]={14,17,17,17,21,18,13},R[7]={30,17,17,30,20,18,17},S[7]={15,16,16,14,1,1,30},T[7]={31,4,4,4,4,4,4},U[7]={17,17,17,17,17,17,14},V[7]={17,17,17,17,17,10,4},Ww[7]={17,17,17,21,21,21,10},X[7]={17,17,10,4,10,17,17},Y[7]={17,17,10,4,4,4,4},Z[7]={31,1,2,4,8,16,31};
 if(ch>='a'&&ch<='z') ch-=32; switch(ch){case ' ':return sp;case '.':return dot;case ':':return colon;case '-':return dash;case '/':return slash;case '\\':return bsx;case '0':return zero;case '1':return one;case '2':return two;case '3':return three;case '4':return four;case '5':return five;case '6':return six;case '7':return seven;case '8':return eight;case '9':return nine;case 'A':return A;case 'B':return B;case 'C':return C;case 'D':return D;case 'E':return E;case 'F':return F;case 'G':return G;case 'H':return Hh;case 'I':return I;case 'J':return J;case 'K':return K;case 'L':return L;case 'M':return M;case 'N':return N;case 'O':return O;case 'P':return P;case 'Q':return Q;case 'R':return R;case 'S':return S;case 'T':return T;case 'U':return U;case 'V':return V;case 'W':return Ww;case 'X':return X;case 'Y':return Y;case 'Z':return Z;default:return unk;}}
static void text_s(int x,int y,const char* s,UINT32 fg,int sc){ while(*s){ const UINT8* g=glyph(*s++); for(int r=0;r<7;r++)for(int c=0;c<5;c++)if(g[r]&(1<<(4-c)))fill(x+c*sc,y+r*sc,sc,sc,fg); x+=6*sc; } }
static void text(int x,int y,const char*s,UINT32 fg){ text_s(x,y,s,fg,2); }
static void small(int x,int y,const char*s,UINT32 fg){ text_s(x,y,s,fg,1); }
static int inr(int px,int py,int x,int y,int w,int h){return px>=x&&py>=y&&px<x+w&&py<y+h;}

#define APP_COUNT 8
#define APP_EXPLORER 1
#define APP_CONSOLE 2
#define APP_CODE 3
#define APP_GPUX 4
#define APP_SETTINGS 5
#define APP_ABOUT 6
#define APP_HELP 7
#define FILE_MAX 10
#define CODE_LINES 72
#define CODE_COLS 112
#define CON_LINES 42
#define CON_LEN 120
#define IN_LEN 96
#define START_N 8
#define THEME_N 4

typedef struct{int open,x,y,w,h;const char* title;} Win;
typedef struct{const char* name;UINT8 r1,g1,b1,r2,g2,b2,ar,ag,ab,ir,ig,ib;} Theme;
typedef struct{UINT16 Year;UINT8 Month;UINT8 Day;UINT8 Hour;UINT8 Minute;UINT8 Second;} MiniTime;
typedef EFI_STATUS (*EFI_GET_TIME_FN)(MiniTime*, VOID*);

static Theme themes[THEME_N]={{"classic",24,112,170,8,64,132,0,120,215,0,96,190},{"graphite",54,64,76,28,32,38,95,125,155,95,110,130},{"olive",71,92,82,34,50,44,112,136,92,98,130,96},{"wine",92,48,70,45,28,42,145,76,108,132,70,105}};
static Win win[APP_COUNT]={{0,0,0,0,0,""},{1,70,105,560,405,"Explorer"},{1,290,165,650,390,"Console"},{0,180,95,760,530,"Code Studio"},{0,380,118,560,420,"GPUx"},{0,430,150,470,360,"Settings"},{0,330,110,575,400,"About"},{0,220,90,670,430,"Help"}};
static int active=APP_CONSOLE,start_open=0,start_sel=0,file_sel=0,path_id=0,drag=0,drag_dx=0,drag_dy=0,frame=0,theme_id=0,set_sel=0;
static int mp=0,mr=0;
static char lines_buf[CON_LINES][CON_LEN]; static int line_count=0; static char input[IN_LEN]; static int input_len=0;
static char hist[12][IN_LEN]; static int hist_n=0;
static char edit_names[FILE_MAX][28]; static char edit[FILE_MAX][CODE_LINES][CODE_COLS]; static int edit_counts[FILE_MAX]; static int file_n=0;
static int code_file=-1,code_x=0,code_y=0,code_top=0,code_dirty=0;
static char notes[8][80]; static int notes_n=0; static char todos[8][80]; static UINT8 todo_done[8]; static int todo_n=0;
static char cpu_vendor[13]="unknown"; static char cpu_brand[49]="unknown CPU"; static UINT32 cpu_sig=0,pci_n=0; static UINT16 vga_v=0,vga_d=0,storage_v=0,storage_d=0,net_v=0,net_d=0; static UINT8 usb_n=0; static char usb_desc[80]="none";
static const char* start_items[START_N]={"Explorer","Console","Code Studio","GPUx","Settings","About","Help","Reboot"};

static int slen(const char*s){int n=0;while(s&&s[n])n++;return n;} static void scpy(char*d,const char*s){while((*d++=*s++));} static void scat(char*d,const char*s){while(*d)d++;while((*d++=*s++));}
static void rev(char*s){int i=0,j=slen(s)-1;while(i<j){char t=s[i];s[i]=s[j];s[j]=t;i++;j--;}}
static void utoa2(UINT32 v,char*b,int base){const char*dig="0123456789ABCDEF";int i=0;if(v==0){b[i++]='0';b[i]=0;return;}while(v){b[i++]=dig[v%(UINT32)base];v/=(UINT32)base;}b[i]=0;rev(b);} static int atoi2(const char*s){int v=0,sg=1;while(*s==' ')s++;if(*s=='-'){sg=-1;s++;}while(*s>='0'&&*s<='9'){v=v*10+(*s-'0');s++;}return v*sg;}
static int streq(const char*a,const char*b){int i=0;while(a[i]&&b[i]){char ca=a[i],cb=b[i]; if(ca>='A'&&ca<='Z')ca+=32; if(cb>='A'&&cb<='Z')cb+=32; if(ca!=cb)return 0;i++;} return a[i]==0&&b[i]==0;}
static int contains(const char*a,const char*b){if(!b||!b[0])return 1;for(int i=0;a&&a[i];i++){int j=0;while(a[i+j]&&b[j]){char ca=a[i+j],cb=b[j];if(ca>='A'&&ca<='Z')ca+=32;if(cb>='A'&&cb<='Z')cb+=32;if(ca!=cb)break;j++;}if(!b[j])return 1;}return 0;}
static char* trim(char*s){while(*s==' '||*s=='\t')s++;return s;} static void rtrim(char*s){int n=slen(s);while(n>0&&(s[n-1]==' '||s[n-1]=='\t'||s[n-1]=='\n'||s[n-1]=='\r'))s[--n]=0;}
static char* tok(char**c){char*s=trim(*c);if(!*s){*c=s;return NULL;}char*st=s;while(*s&&*s!=' '&&*s!='\t')s++;if(*s){*s=0;s++;}*c=s;return st;} static void anum(char*l,UINT32 v){char b[20];utoa2(v,b,10);scat(l,b);} static void ahex(char*l,UINT32 v){char b[20];scat(l,"0x");utoa2(v,b,16);scat(l,b);} static void idstr(char*l,UINT16 v,UINT16 d){scat(l,"ven=");ahex(l,v);scat(l," dev=");ahex(l,d);}

static UINT32 pci_read(UINT8 bus,UINT8 slot,UINT8 func,UINT8 off){UINT32 addr=(1u<<31)|((UINT32)bus<<16)|((UINT32)slot<<11)|((UINT32)func<<8)|(off&0xFC);port_out32(0xCF8,addr);return port_in32(0xCFC);} static const char* usbtype(UINT8 pi){if(pi==0)return "UHCI";if(pi==0x10)return "OHCI";if(pi==0x20)return "EHCI";if(pi==0x30)return "xHCI";return "USB";}
static void cpuid2(UINT32 leaf,UINT32 sub,UINT32*a,UINT32*b,UINT32*c,UINT32*d){__asm__ __volatile__("cpuid":"=a"(*a),"=b"(*b),"=c"(*c),"=d"(*d):"a"(leaf),"c"(sub));} static void regcpy(char*d,UINT32 v){d[0]=v&255;d[1]=(v>>8)&255;d[2]=(v>>16)&255;d[3]=(v>>24)&255;}
static void detect_hw(void){UINT32 a,b,c,d;cpuid2(0,0,&a,&b,&c,&d);if(a){regcpy(cpu_vendor,b);regcpy(cpu_vendor+4,d);regcpy(cpu_vendor+8,c);cpu_vendor[12]=0;cpuid2(1,0,&a,&b,&c,&d);cpu_sig=a;UINT32 max;cpuid2(0x80000000u,0,&max,&b,&c,&d);if(max>=0x80000004u){UINT32 r[12];cpuid2(0x80000002u,0,&r[0],&r[1],&r[2],&r[3]);cpuid2(0x80000003u,0,&r[4],&r[5],&r[6],&r[7]);cpuid2(0x80000004u,0,&r[8],&r[9],&r[10],&r[11]);for(int i=0;i<12;i++)regcpy(cpu_brand+i*4,r[i]);cpu_brand[48]=0;}} usb_desc[0]=0;for(int bus=0;bus<256;bus++)for(int sl=0;sl<32;sl++)for(int fn=0;fn<8;fn++){UINT32 id=pci_read(bus,sl,fn,0);if(id==0xFFFFFFFFu)continue;UINT16 ven=id&0xFFFF,dev=(id>>16)&0xFFFF;UINT32 cr=pci_read(bus,sl,fn,8);UINT8 pi=(cr>>8)&255,sub=(cr>>16)&255,cl=(cr>>24)&255;pci_n++;if(cl==3&&!vga_v){vga_v=ven;vga_d=dev;}if(cl==1&&!storage_v){storage_v=ven;storage_d=dev;}if(cl==2&&!net_v){net_v=ven;net_d=dev;}if(cl==0x0C&&sub==3&&usb_n<8){if(usb_desc[0])scat(usb_desc,", ");scat(usb_desc,usbtype(pi));usb_n++;}if(fn==0){UINT32 hdr=pci_read(bus,sl,fn,0x0C);if(!((hdr>>23)&1))break;}} if(!usb_desc[0])scpy(usb_desc,"none");}

static void addline(const char* s){ if(line_count<CON_LINES){int i=0;for(;s[i]&&i<CON_LEN-1;i++)lines_buf[line_count][i]=s[i];lines_buf[line_count][i]=0;line_count++;return;} for(int i=1;i<CON_LINES;i++){int j=0;for(;lines_buf[i][j]&&j<CON_LEN-1;j++)lines_buf[i-1][j]=lines_buf[i][j];lines_buf[i-1][j]=0;} int k=0;for(;s[k]&&k<CON_LEN-1;k++)lines_buf[CON_LINES-1][k]=s[k];lines_buf[CON_LINES-1][k]=0;}
static void hist_add(const char*s){if(hist_n<12){scpy(hist[hist_n++],s);return;}for(int i=1;i<12;i++)scpy(hist[i-1],hist[i]);scpy(hist[11],s);} static void open_app(int a){if(a>0&&a<APP_COUNT){win[a].open=1;active=a;start_open=0;}}
static void close_app(int a){if(a>0&&a<APP_COUNT){win[a].open=0;if(active==a){active=0;for(int i=1;i<APP_COUNT;i++)if(win[i].open){active=i;break;}}}}
static void fit(Win*w){ if(w->w>(int)W-20)w->w=W-20;if(w->h>(int)H-62)w->h=H-62;if(w->x<4)w->x=4;if(w->y<4)w->y=4;if(w->x+w->w>(int)W-4)w->x=(int)W-w->w-4;if(w->y+w->h>(int)H-44)w->y=(int)H-44-w->h;}
static int file_find(const char*n){for(int i=0;i<file_n;i++)if(streq(edit_names[i],n))return i;return -1;} static int file_alloc(const char*n){int i=file_find(n);if(i>=0)return i;if(file_n>=FILE_MAX)return -1;i=file_n++;scpy(edit_names[i],n);edit_counts[i]=1;for(int l=0;l<CODE_LINES;l++)edit[i][l][0]=0;return i;} static void code_open(const char*n){if(!n||!n[0])n="main.c";int i=file_alloc(n);if(i<0){addline("CODE STORAGE FULL");return;}code_file=i;code_x=code_y=code_top=0;code_dirty=0;open_app(APP_CODE);} static int linelen(char*s){return slen(s);} static void insch(char*l,int p,char c){int n=linelen(l);if(n>=CODE_COLS-1)return;if(p<0)p=0;if(p>n)p=n;for(int i=n;i>=p;i--)l[i+1]=l[i];l[p]=c;} static void delch(char*l,int p){int n=linelen(l);if(p<0||p>=n)return;for(int i=p;i<n;i++)l[i]=l[i+1];}
static void init_colors(void);
static void prompt(char* out){const char*p="C:\\> ";int i=0;while(p[i]){out[i]=p[i];i++;}for(int j=0;input[j]&&i<CON_LEN-1;j++,i++)out[i]=input[j];out[i]=0;}
static void show_file(const char*n){int i=file_find(n);if(i>=0){for(int l=0;l<edit_counts[i];l++)addline(edit[i][l]);return;} if(streq(n,"os-release")||streq(n,"/etc/os-release")){addline("NAME=SacramentuOS");addline("VERSION=1.0.1");addline("UI=UEFI GOP PIXEL");return;} if(streq(n,"cpuinfo")){char l[CON_LEN];scpy(l,"CPU: ");scat(l,cpu_brand);addline(l);return;} addline("FILE NOT FOUND");}
static void exec_cmd(void){char p[CON_LEN];prompt(p);addline(p);char raw[IN_LEN];scpy(raw,input);rtrim(raw);char* in=trim(raw);if(!in[0]){input[0]=0;input_len=0;return;}hist_add(in);char* c=in;char* cmd=tok(&c);char* args=trim(c);if(streq(cmd,"help")||streq(cmd,"docs")){addline("COMMANDS: help commands fetch ver ls cat clear calc edit files show rm notes todo top gpux settings");addline("APPS: explorer console code gpux settings about help. F1-F7 hotkeys.");}
else if(streq(cmd,"commands")){addline("help docs commands find clear clean fetch sysinfo about echo uname whoami mem uptime ticks date time calc color theme history len upper lower reverse repeat ascii hexdump ls dir cat pwd ver note notes noteclr todo done todoclr base hash rot13 count edit code files show rm ps top kernel log explorer settings gpux reboot halt shutdown");}
else if(streq(cmd,"clear")||streq(cmd,"clean")){line_count=0;} else if(streq(cmd,"ver")){addline("SacramentuOS version 1.0.1");}
else if(streq(cmd,"fetch")||streq(cmd,"sysinfo")){char l[CON_LEN];addline("SACRAMENTUOS FETCH");scpy(l,"CPU: ");scat(l,cpu_brand);addline(l);scpy(l,"DISPLAY: GOP ");anum(l,W);scat(l,"x");anum(l,H);addline(l);scpy(l,"PCI devices: ");anum(l,pci_n);addline(l);scpy(l,"USB: ");scat(l,usb_desc);addline(l);} else if(streq(cmd,"echo")){addline(args);} else if(streq(cmd,"uname")){addline("SacramentuOS 1.0.1 x86_64-uefi Pixel Desktop");} else if(streq(cmd,"whoami")){addline("user");}
else if(streq(cmd,"mem")){addline("UEFI memory map not owned after boot services; static desktop buffers active.");} else if(streq(cmd,"uptime")||streq(cmd,"ticks")){char l[60];scpy(l,"frames: ");anum(l,frame);addline(l);} else if(streq(cmd,"date")||streq(cmd,"time")){MiniTime t;EFI_GET_TIME_FN gt=(EFI_GET_TIME_FN)st->RuntimeServices->GetTime;if(gt&&gt(&t,NULL)==EFI_SUCCESS){char l[64];l[0]=0;anum(l,t.Day);scat(l,".");anum(l,t.Month);scat(l,".");anum(l,t.Year);scat(l," ");anum(l,t.Hour);scat(l,":");anum(l,t.Minute);addline(l);}else addline("TIME UNAVAILABLE");}
else if(streq(cmd,"calc")){char* cc=args;char*a=tok(&cc);char*op=tok(&cc);char*b=tok(&cc);if(!a||!op||!b)addline("usage: calc 2 + 2");else{int x=atoi2(a),y=atoi2(b),r=0,ok=1;if(op[0]=='+')r=x+y;else if(op[0]=='-')r=x-y;else if(op[0]=='*')r=x*y;else if(op[0]=='/'){if(!y){addline("division by zero");ok=0;}else r=x/y;}else ok=0;if(ok){char l[30];utoa2((UINT32)r,l,10);addline(l);}}}
else if(streq(cmd,"ls")||streq(cmd,"dir")){addline("Desktop/ Documents/ Apps/ System/ Proc/ README.TXT");if(file_n){char l[CON_LEN];scpy(l,"RAM: ");for(int i=0;i<file_n;i++){scat(l,edit_names[i]);scat(l," ");}addline(l);}}
else if(streq(cmd,"cat")||streq(cmd,"show")){show_file(args);} else if(streq(cmd,"pwd")){addline("/");} else if(streq(cmd,"edit")||streq(cmd,"code")){code_open(args);addline("CODE STUDIO OPENED");}
else if(streq(cmd,"files")){if(!file_n)addline("RAM files: <none>");for(int i=0;i<file_n;i++){char l[60];scpy(l,edit_names[i]);scat(l," lines=");anum(l,edit_counts[i]);addline(l);}}
else if(streq(cmd,"rm")){int i=file_find(args);if(i<0)addline("RAM FILE NOT FOUND");else{for(int f=i+1;f<file_n;f++){scpy(edit_names[f-1],edit_names[f]);edit_counts[f-1]=edit_counts[f];for(int l=0;l<CODE_LINES;l++)scpy(edit[f-1][l],edit[f][l]);}file_n--;addline("removed");}}
else if(streq(cmd,"note")||streq(cmd,"notes")){if(streq(cmd,"notes")||streq(args,"list")||!args[0]){if(!notes_n)addline("no notes");for(int i=0;i<notes_n;i++)addline(notes[i]);}else{char* cc=args;char* a=tok(&cc);cc=trim(cc);if(a&&streq(a,"add")&&cc[0]&&notes_n<8){scpy(notes[notes_n++],cc);addline("note saved");}else if(a&&streq(a,"clear")){notes_n=0;addline("notes cleared");}else addline("usage: note add/list/clear");}}
else if(streq(cmd,"todo")){char* cc=args;char* a=tok(&cc);cc=trim(cc);if(!a||streq(a,"list")){if(!todo_n)addline("todo empty");for(int i=0;i<todo_n;i++){char l[100];scpy(l,todo_done[i]?"[x] ":"[ ] ");scat(l,todos[i]);addline(l);}}else if(streq(a,"add")&&cc[0]&&todo_n<8){scpy(todos[todo_n],cc);todo_done[todo_n++]=0;addline("todo added");}else if(streq(a,"clear")){todo_n=0;addline("todo cleared");}}
else if(streq(cmd,"done")){int i=atoi2(args)-1;if(i>=0&&i<todo_n){todo_done[i]=1;addline("done");}else addline("usage: done N");}
else if(streq(cmd,"history")){for(int i=0;i<hist_n;i++)addline(hist[i]);} else if(streq(cmd,"len")){char l[20];utoa2(slen(args),l,10);addline(l);} else if(streq(cmd,"upper")||streq(cmd,"lower")){char l[CON_LEN];int i=0;for(;args[i]&&i<CON_LEN-1;i++){char ch=args[i];if(streq(cmd,"upper")&&ch>='a'&&ch<='z')ch-=32;if(streq(cmd,"lower")&&ch>='A'&&ch<='Z')ch+=32;l[i]=ch;}l[i]=0;addline(l);} else if(streq(cmd,"reverse")){char l[CON_LEN];scpy(l,args);rev(l);addline(l);}
else if(streq(cmd,"repeat")){char* cc=args;char*n=tok(&cc);cc=trim(cc);int c=n?atoi2(n):0;if(c<1||!cc[0])addline("usage: repeat N text");else{if(c>20)c=20;for(int i=0;i<c;i++)addline(cc);}}
else if(streq(cmd,"ascii")){for(int i=32;i<127;i+=8){char l[CON_LEN];l[0]=0;for(int j=0;j<8&&i+j<127;j++){char n[8];utoa2(i+j,n,10);scat(l,n);scat(l,"='");char ch[2]={(char)(i+j),0};scat(l,ch);scat(l,"' ");}addline(l);}}
else if(streq(cmd,"hexdump")){char l[CON_LEN];l[0]=0;for(int i=0;args[i]&&slen(l)<CON_LEN-5;i++){char h[8];utoa2((UINT8)args[i],h,16);if(slen(h)==1)scat(l,"0");scat(l,h);scat(l," ");}addline(l);}
else if(streq(cmd,"base")){UINT32 v=(UINT32)atoi2(args);char l[64];scpy(l,"dec: ");anum(l,v);addline(l);scpy(l,"hex: ");ahex(l,v);addline(l);}
else if(streq(cmd,"hash")){UINT32 h=2166136261u;for(int i=0;args[i];i++){h^=(UINT8)args[i];h*=16777619u;}char l[64];scpy(l,"fnv1a32: ");ahex(l,h);addline(l);}
else if(streq(cmd,"rot13")){char l[CON_LEN];int i=0;for(;args[i]&&i<CON_LEN-1;i++){char ch=args[i];if(ch>='a'&&ch<='z')ch='a'+((ch-'a'+13)%26);else if(ch>='A'&&ch<='Z')ch='A'+((ch-'A'+13)%26);l[i]=ch;}l[i]=0;addline(l);}
else if(streq(cmd,"count")){int chars=0,words=0,iw=0;for(int i=0;args[i];i++){chars++;if(args[i]==' '||args[i]=='\t')iw=0;else if(!iw){words++;iw=1;}}char l[64];scpy(l,"chars=");anum(l,chars);scat(l," words=");anum(l,words);addline(l);}
else if(streq(cmd,"find")){const char* names="help docs commands clear clean fetch sysinfo about echo uname whoami mem uptime ticks date time calc color theme history len upper lower reverse repeat ascii hexdump ls dir cat pwd ver note notes todo done edit code files show rm ps top kernel log explorer settings gpux reboot halt shutdown"; if(contains(names,args))addline(names);else addline("nothing matched");}
else if(streq(cmd,"color")||streq(cmd,"theme")){if(streq(args,"list")){for(int i=0;i<THEME_N;i++){char l[60];utoa2(i,l,10);scat(l," - ");scat(l,themes[i].name);addline(l);}}else{int id=-1;if(args[0]>='0'&&args[0]<='9')id=atoi2(args);else for(int i=0;i<THEME_N;i++)if(contains(themes[i].name,args))id=i;if(id>=0&&id<THEME_N){theme_id=id;set_sel=id;init_colors();addline("theme applied");}else addline("usage: theme classic|graphite|olive|wine or color 0..3");}}
else if(streq(cmd,"noteclr")){notes_n=0;addline("notes cleared");}
else if(streq(cmd,"todoclr")){todo_n=0;addline("todo cleared");}
else if(streq(cmd,"ps")){addline("PID STATE NAME");addline("1 run kernel");addline("2 run desktop-shell");addline("3 idle idle-loop");}
else if(streq(cmd,"kernel")){addline("kernel: SacramentuOS 1.0.1 UEFI GOP");addline("modules: gop desktop pointer ps2 cpuid pci console code");}
else if(streq(cmd,"log")){addline("UEFI log: console history is available with history");}
else if(streq(cmd,"top")||streq(cmd,"gpux")){open_app(APP_GPUX);addline("GPUX OPENED");} else if(streq(cmd,"explorer")){open_app(APP_EXPLORER);} else if(streq(cmd,"settings")){open_app(APP_SETTINGS);} else if(streq(cmd,"about")){open_app(APP_ABOUT);} else if(streq(cmd,"reboot")){EFI_RESET_SYSTEM rs=(EFI_RESET_SYSTEM)st->RuntimeServices->ResetSystem;rs(0,EFI_SUCCESS,0,NULL);} else if(streq(cmd,"halt")||streq(cmd,"shutdown")){while(1)__asm__ __volatile__("hlt");} else addline("UNKNOWN COMMAND. TYPE HELP."); input[0]=0;input_len=0;}

static void init_colors(void){Theme*t=&themes[theme_id];C_DESK1=rgb(t->r1,t->g1,t->b1);C_DESK2=rgb(t->r2,t->g2,t->b2);C_PANEL=rgb(238,238,238);C_TEXT=rgb(20,20,20);C_WHITE=rgb(255,255,255);C_BLACK=rgb(0,0,0);C_BLUE=rgb(t->ar,t->ag,t->ab);C_ACCENT=C_BLUE;C_WINDOW=rgb(244,244,244);C_BORDER=rgb(100,100,100);C_TITLE=C_BLUE;C_INACTIVE=rgb(125,150,175);C_SHADOW=rgb(26,26,26);C_SELECT=rgb(64,120,216);C_ERROR=rgb(190,30,30);C_GOOD=rgb(30,145,70);C_CON=rgb(7,16,7);C_CONTXT=rgb(135,255,135);}
static void button(int x,int y,int w,int h,const char*l,int hot){fill(x,y,w,h,hot?C_BLUE:C_PANEL);rect(x,y,w,h,hot?C_BLUE:C_BORDER);small(x+10,y+(h-7)/2,l,hot?C_WHITE:C_TEXT);} static void bg(void){Theme*t=&themes[theme_id];for(int y=0;y<(int)H;y+=4){UINT8 r=t->r1+((int)t->r2-(int)t->r1)*y/(H?H:1);UINT8 g=t->g1+((int)t->g2-(int)t->g1)*y/(H?H:1);UINT8 b=t->b1+((int)t->b2-(int)t->b1)*y/(H?H:1);fill(0,y,W,4,rgb(r,g,b));} line(W-320,90,W-35,330,rgb(t->ar,t->ag,t->ab)); text(22,18,"SacramentuOS",C_WHITE); small(24,42,"version 1.0.1",rgb(205,235,255));}
static void icon(int x,int y,const char*l,UINT32 c){fill(x+8,y,36,28,C_WHITE);rect(x+8,y,36,28,c);fill(x+14,y+7,24,14,c);small(x,y+38,l,C_WHITE);} static void frame_win(int a){Win*w=&win[a];fit(w);fill(w->x+8,w->y+8,w->w,w->h,C_SHADOW);fill(w->x,w->y,w->w,w->h,C_WINDOW);rect(w->x,w->y,w->w,w->h,C_BORDER);fill(w->x+1,w->y+1,w->w-2,28,a==active?C_TITLE:C_INACTIVE);small(w->x+10,w->y+10,w->title,C_WHITE);fill(w->x+w->w-34,w->y+3,30,22,a==active?C_ERROR:C_BORDER);small(w->x+w->w-23,w->y+10,"X",C_WHITE);} 
static const char* item_name(int p,int i,int*dir,int*app){*dir=0;*app=0;if(p==0){static const char*n[]={"Desktop","Documents","Apps","System","Proc","README.TXT"};if(i<5)*dir=1;return i<6?n[i]:NULL;} if(i==0){*dir=1;return "..";} if(p==1){static const char*n[]={"..","Explorer.app","Console.app","CodeStudio.app","GPUx.app","Settings.app"};int a[]={0,APP_EXPLORER,APP_CONSOLE,APP_CODE,APP_GPUX,APP_SETTINGS};*app=a[i];return i<6?n[i]:NULL;} if(p==2){static const char*n[]={"..","NOTES.TXT","TODO.TXT","CODE-FILES"};return i<4?n[i]:NULL;} if(p==3){static const char*n[]={"..","Explorer.app","Console.app","CodeStudio.app","GPUx.app","Settings.app","About.app","Help.app"};int a[]={0,APP_EXPLORER,APP_CONSOLE,APP_CODE,APP_GPUX,APP_SETTINGS,APP_ABOUT,APP_HELP};*app=a[i];return i<8?n[i]:NULL;} if(p==4){static const char*n[]={"..","os-release.txt","display.txt","input.txt","build.txt"};return i<5?n[i]:NULL;} static const char*n[]={"..","cpuinfo","meminfo","pci","usb","uptime"};return i<6?n[i]:NULL;}
static int item_count(int p){return p==0?6:p==1?6:p==2?4:p==3?8:p==4?5:6;} static const char* pathn(void){static const char*p[]={"C:\\SACRAMENTUOS","C:\\SACRAMENTUOS\\DESKTOP","C:\\SACRAMENTUOS\\DOCUMENTS","C:\\SACRAMENTUOS\\APPS","C:\\SACRAMENTUOS\\SYSTEM","C:\\SACRAMENTUOS\\PROC"};return p[path_id];}
static void explorer_open(void){int dir,app;const char*n=item_name(path_id,file_sel,&dir,&app);if(!n)return;if(dir){if(streq(n,".."))path_id=0;else if(streq(n,"Desktop"))path_id=1;else if(streq(n,"Documents"))path_id=2;else if(streq(n,"Apps"))path_id=3;else if(streq(n,"System"))path_id=4;else if(streq(n,"Proc"))path_id=5;file_sel=0;}else if(app)open_app(app);}
static void explorer(void){Win*w=&win[APP_EXPLORER];frame_win(APP_EXPLORER);int x=w->x+14,y=w->y+42;fill(x,y,w->w-28,24,C_WHITE);rect(x,y,w->w-28,24,C_BORDER);small(x+8,y+9,pathn(),C_TEXT);int ly=y+38,lw=230;fill(x,ly,lw,w->h-98,C_WHITE);rect(x,ly,lw,w->h-98,C_BORDER);int cnt=item_count(path_id);for(int i=0;i<cnt;i++){int dir,app;const char*n=item_name(path_id,i,&dir,&app);int iy=ly+8+i*23;fill(x+4,iy-3,lw-8,19,i==file_sel?C_SELECT:C_WHITE);char lab[80];lab[0]=0;scat(lab,dir?"[DIR] ":"      ");scat(lab,n);small(x+10,iy+2,lab,i==file_sel?C_WHITE:C_TEXT);}int px=x+lw+18;fill(px,ly,w->w-lw-60,w->h-98,C_WHITE);rect(px,ly,w->w-lw-60,w->h-98,C_BORDER);text(px+18,ly+20,"Preview",C_TEXT);small(px+20,ly+58,"Folders are navigable. Apps launch from here.",C_TEXT);small(px+20,ly+82,"Enter or click an item.",C_TEXT);}
static void console(void){Win*w=&win[APP_CONSOLE];frame_win(APP_CONSOLE);int x=w->x+12,y=w->y+40,cw=w->w-24,ch=w->h-56;fill(x,y,cw,ch,C_CON);rect(x,y,cw,ch,rgb(35,80,35));int rows=(ch-34)/12;int stt=line_count>rows?line_count-rows:0;for(int i=0;i<rows;i++){int idx=stt+i;if(idx<line_count)small(x+8,y+8+i*12,lines_buf[idx],C_CONTXT);}char p[CON_LEN];prompt(p);fill(x+4,y+ch-24,cw-8,18,C_BLACK);small(x+8,y+ch-19,p,C_CONTXT);if(active==APP_CONSOLE&&((frame/20)&1))fill(x+8+(5+input_len)*6,y+ch-20,6,10,C_CONTXT);} 
static void code(void){Win*w=&win[APP_CODE];frame_win(APP_CODE);int x=w->x+10,y=w->y+38,cw=w->w-20,ch=w->h-50;fill(x,y,cw,ch,rgb(28,31,36));rect(x,y,cw,ch,rgb(70,75,85));fill(x,y,cw,26,rgb(38,42,50));char title[80];scpy(title,"Code Studio - ");scat(title,code_file>=0?edit_names[code_file]:"<no file>");if(code_dirty)scat(title," *");small(x+10,y+10,title,rgb(220,225,230));fill(x,y+26,46,ch-52,rgb(42,45,52));if(code_file>=0){if(code_y<0)code_y=0;if(code_y>=edit_counts[code_file])code_y=edit_counts[code_file]-1;if(code_y<code_top)code_top=code_y;if(code_y>=code_top+28)code_top=code_y-27;for(int r=0;r<(ch-62)/12;r++){int li=code_top+r;if(li<edit_counts[code_file]){char n[8];utoa2(li+1,n,10);small(x+8,y+36+r*12,n,rgb(145,150,158));small(x+56,y+36+r*12,edit[code_file][li],rgb(220,225,230));}}if(active==APP_CODE&&((frame/18)&1))fill(x+56+code_x*6,y+36+(code_y-code_top)*12,2,10,C_WHITE);}else text(x+70,y+90,"No file",rgb(220,225,230));fill(x,y+ch-24,cw,24,rgb(38,42,50));small(x+10,y+ch-16,"F7 SAVE | F8 SAVE+CLOSE | F9 CLOSE | ARROWS | ENTER",rgb(190,200,210));}
static void gpux(void){Win*w=&win[APP_GPUX];frame_win(APP_GPUX);int x=w->x+24,y=w->y+52;char l[CON_LEN];text(x,y,"GPUx",C_TEXT);small(x,y+34,"CPUID and PCI IDs are real. GPU load needs vendor driver counters.",C_TEXT);scpy(l,"CPU: ");scat(l,cpu_brand);small(x,y+64,l,C_TEXT);scpy(l,"CPU vendor: ");scat(l,cpu_vendor);scat(l," sig=");ahex(l,cpu_sig);small(x,y+86,l,C_TEXT);scpy(l,"Display PCI: ");if(vga_v)idstr(l,vga_v,vga_d);else scat(l,"not detected");small(x,y+112,l,C_TEXT);scpy(l,"Storage PCI: ");if(storage_v)idstr(l,storage_v,storage_d);else scat(l,"not detected");small(x,y+134,l,C_TEXT);scpy(l,"Network PCI: ");if(net_v)idstr(l,net_v,net_d);else scat(l,"not detected");small(x,y+156,l,C_TEXT);scpy(l,"USB controllers: ");scat(l,usb_desc);small(x,y+178,l,C_TEXT);int cpu=(frame*7)%40+3,gui=frame%100,ram=(file_n*9+line_count/2)%100;small(x,y+218,"CPU activity",C_TEXT);fill(x+120,y+216,250,12,C_BORDER);fill(x+121,y+217,248*cpu/100,10,C_GOOD);small(x,y+246,"GUI renderer",C_TEXT);fill(x+120,y+244,250,12,C_BORDER);fill(x+121,y+245,248*gui/100,10,C_BLUE);small(x,y+274,"RAM buffers",C_TEXT);fill(x+120,y+272,250,12,C_BORDER);fill(x+121,y+273,248*ram/100,10,rgb(120,100,190));}
static void settings(void){Win*w=&win[APP_SETTINGS];frame_win(APP_SETTINGS);int x=w->x+24,y=w->y+52;text(x,y,"Settings",C_TEXT);for(int i=0;i<THEME_N;i++){int iy=y+54+i*42;fill(x,iy,280,30,i==set_sel?C_SELECT:C_PANEL);rect(x,iy,280,30,C_BORDER);fill(x+10,iy+7,42,16,rgb(themes[i].ir,themes[i].ig,themes[i].ib));small(x+66,iy+11,themes[i].name,i==set_sel?C_WHITE:C_TEXT);}small(x,y+250,"Enter/click changes background and icon colors.",C_TEXT);} 
static void about(void){Win*w=&win[APP_ABOUT];frame_win(APP_ABOUT);int x=w->x+28,y=w->y+54;text(x,y,"SacramentuOS 1.0.1",C_TEXT);const char*ls[]={"UEFI BOOTX64.EFI pixel desktop using GOP framebuffer.","Windows-like shell: windows, taskbar, Start menu, Explorer, Console, Code Studio, GPUx.","Console includes old commands: fetch, calc, notes, todo, editor files and system tools.","Explorer has folders and app launch. Settings changes classic themes.","Input: UEFI pointer protocols plus raw PS/2 polling fallback.","Hardware: CPUID and PCI config-space probe for real component IDs.","Storage is RAM-only virtual FS. Full disk FS and scheduler are future work."};for(int i=0;i<7;i++)small(x,y+42+i*24,ls[i],C_TEXT);} static void help(void){Win*w=&win[APP_HELP];frame_win(APP_HELP);int x=w->x+24,y=w->y+52;text(x,y,"Help",C_TEXT);const char*ls[]={"F1 Start, F2 Explorer, F3 Console, F4 About, F5 Settings, F6 Code, F7 GPUx.","Explorer: Enter/click opens folders and apps. Use '..' to go back.","Code Studio: type code, arrows move, Enter newline, Backspace/Delete edit.","Console: help, commands, fetch, edit, files, show, rm, note, todo, top, gpux.","Mouse: click window title and drag. X closes windows.","QEMU: Ctrl+Alt+G grabs/releases mouse if needed."};for(int i=0;i<6;i++)small(x,y+45+i*24,ls[i],C_TEXT);} 
static void windows_draw(void){for(int i=1;i<APP_COUNT;i++)if(win[i].open&&i!=active){if(i==1)explorer();else if(i==2)console();else if(i==3)code();else if(i==4)gpux();else if(i==5)settings();else if(i==6)about();else if(i==7)help();}if(active>0&&win[active].open){if(active==1)explorer();else if(active==2)console();else if(active==3)code();else if(active==4)gpux();else if(active==5)settings();else if(active==6)about();else if(active==7)help();}}
static void taskbar(void){int y=H-40;fill(0,y,W,40,C_PANEL);fill(0,y,W,1,C_WHITE);button(8,y+6,92,28,"START",start_open);int bx=112;for(int i=1;i<APP_COUNT;i++)if(win[i].open){button(bx,y+7,118,26,win[i].title,i==active);bx+=126;}MiniTime t;EFI_GET_TIME_FN gt=(EFI_GET_TIME_FN)st->RuntimeServices->GetTime;char l[40];l[0]=0;if(gt&&gt(&t,NULL)==EFI_SUCCESS){if(t.Hour<10)scat(l,"0");anum(l,t.Hour);scat(l,":");if(t.Minute<10)scat(l,"0");anum(l,t.Minute);scat(l,"  ");if(t.Day<10)scat(l,"0");anum(l,t.Day);scat(l,".");if(t.Month<10)scat(l,"0");anum(l,t.Month);}else scpy(l,"--:--");small(W-120,y+10,l,C_TEXT);} static void startmenu(void){if(!start_open)return;int x=8,y=H-355,w=270,h=310;fill(x+5,y+5,w,h,C_SHADOW);fill(x,y,w,h,C_PANEL);rect(x,y,w,h,C_BORDER);fill(x,y,w,46,C_BLUE);text(x+18,y+15,"SacramentuOS",C_WHITE);for(int i=0;i<START_N;i++){int iy=y+60+i*29;fill(x+8,iy,w-16,25,i==start_sel?C_SELECT:C_PANEL);small(x+20,iy+9,start_items[i],i==start_sel?C_WHITE:C_TEXT);}}
static void cursor(void){line(mx,my,mx,my+20,C_BLACK);line(mx,my,mx+14,my+14,C_BLACK);line(mx,my+20,mx+5,my+16,C_BLACK);line(mx+5,my+16,mx+8,my+24,C_BLACK);line(mx+8,my+24,mx+12,my+22,C_BLACK);line(mx+12,my+22,mx+9,my+14,C_BLACK);line(mx+9,my+14,mx+14,my+14,C_BLACK);fill(mx+2,my+4,2,12,C_WHITE);fill(mx+4,my+6,2,10,C_WHITE);fill(mx+6,my+8,2,8,C_WHITE);fill(mx+8,my+10,2,4,C_WHITE);} static void draw(void){bg();UINT32 ic=rgb(themes[theme_id].ir,themes[theme_id].ig,themes[theme_id].ib);icon(36,78,"EXPLORER",ic);icon(36,150,"CONSOLE",C_GOOD);icon(36,222,"CODE",rgb(92,110,135));icon(36,294,"GPUX",rgb(80,150,130));icon(36,366,"SETTINGS",rgb(150,150,150));icon(36,438,"ABOUT",rgb(150,90,210));windows_draw();taskbar();startmenu();cursor();present();}
static int top(int x,int y){if(active>0&&win[active].open&&inr(x,y,win[active].x,win[active].y,win[active].w,win[active].h))return active;for(int i=APP_COUNT-1;i>=1;i--)if(i!=active&&win[i].open&&inr(x,y,win[i].x,win[i].y,win[i].w,win[i].h))return i;return 0;} static void start_do(int s){if(s==0)open_app(APP_EXPLORER);else if(s==1)open_app(APP_CONSOLE);else if(s==2)code_open("main.c");else if(s==3)open_app(APP_GPUX);else if(s==4)open_app(APP_SETTINGS);else if(s==5)open_app(APP_ABOUT);else if(s==6)open_app(APP_HELP);else if(s==7){EFI_RESET_SYSTEM rs=(EFI_RESET_SYSTEM)st->RuntimeServices->ResetSystem;rs(0,EFI_SUCCESS,0,NULL);}}
static void mouse_click(void){int ty=H-40;if(inr(mx,my,8,ty+6,92,28)){start_open=!start_open;return;} if(start_open){int x=8,y=H-355;for(int i=0;i<START_N;i++){int iy=y+60+i*29;if(inr(mx,my,x+8,iy,254,25)){start_sel=i;start_do(i);return;}}start_open=0;} if(inr(mx,my,36,78,70,60)){open_app(APP_EXPLORER);return;} if(inr(mx,my,36,150,70,60)){open_app(APP_CONSOLE);return;} if(inr(mx,my,36,222,70,60)){code_open("main.c");return;} if(inr(mx,my,36,294,70,60)){open_app(APP_GPUX);return;} if(inr(mx,my,36,366,70,60)){open_app(APP_SETTINGS);return;} if(inr(mx,my,36,438,70,60)){open_app(APP_ABOUT);return;} int h=top(mx,my); if(h){active=h;Win*w=&win[h];if(inr(mx,my,w->x+w->w-38,w->y,38,30)){close_app(h);return;} if(inr(mx,my,w->x,w->y,w->w,30)){drag=h;drag_dx=mx-w->x;drag_dy=my-w->y;return;} if(h==APP_EXPLORER){int lx=w->x+14,ly=w->y+42+38;int cnt=item_count(path_id);for(int i=0;i<cnt;i++){int iy=ly+8+i*23;if(inr(mx,my,lx+4,iy-3,222,19)){file_sel=i;explorer_open();return;}}} if(h==APP_SETTINGS){int sx=w->x+24,sy=w->y+52+54;for(int i=0;i<THEME_N;i++){int iy=sy+i*42;if(inr(mx,my,sx,iy,280,30)){set_sel=i;theme_id=i;init_colors();return;}}}}}
static void key_code(int k){if(code_file<0){if(k>=32&&k<=126)code_open("main.c");return;}char(*ln)[CODE_COLS]=edit[code_file];int cnt=edit_counts[code_file];if(k==KEY_F7){code_dirty=0;return;}if(k==KEY_F8){code_dirty=0;close_app(APP_CODE);return;}if(k==KEY_F9){close_app(APP_CODE);return;}if(k==KEY_UP&&code_y>0)code_y--;else if(k==KEY_DOWN&&code_y+1<cnt)code_y++;else if(k==KEY_LEFT){if(code_x>0)code_x--;else if(code_y>0){code_y--;code_x=linelen(ln[code_y]);}}else if(k==KEY_RIGHT){int le=linelen(ln[code_y]);if(code_x<le)code_x++;else if(code_y+1<cnt){code_y++;code_x=0;}}else if(k==KEY_ENTER){if(cnt<CODE_LINES){char tail[CODE_COLS];scpy(tail,ln[code_y]+code_x);ln[code_y][code_x]=0;for(int i=cnt;i>code_y+1;i--)scpy(ln[i],ln[i-1]);scpy(ln[code_y+1],tail);edit_counts[code_file]++;code_y++;code_x=0;code_dirty=1;}}else if(k==KEY_BS){if(code_x>0){delch(ln[code_y],code_x-1);code_x--;code_dirty=1;}else if(code_y>0){int prev=linelen(ln[code_y-1]);if(prev+linelen(ln[code_y])<CODE_COLS){scat(ln[code_y-1],ln[code_y]);for(int i=code_y+1;i<cnt;i++)scpy(ln[i-1],ln[i]);edit_counts[code_file]--;code_y--;code_x=prev;code_dirty=1;}}}else if(k>=32&&k<=126){insch(ln[code_y],code_x,(char)k);code_x++;code_dirty=1;}int le=linelen(ln[code_y]);if(code_x>le)code_x=le;}
static void handle_key(int k){if(start_open){if(k==KEY_UP&&start_sel>0)start_sel--;else if(k==KEY_DOWN&&start_sel<START_N-1)start_sel++;else if(k==KEY_ENTER)start_do(start_sel);else if(k==KEY_ESC||k==KEY_F1)start_open=0;return;} if(k==KEY_F1){start_open=1;return;} if(k==KEY_F2){open_app(APP_EXPLORER);return;} if(k==KEY_F3){open_app(APP_CONSOLE);return;} if(k==KEY_F4){open_app(APP_ABOUT);return;} if(k==KEY_F5){open_app(APP_SETTINGS);return;} if(k==KEY_F6){code_open("main.c");return;} if(k==KEY_F7){open_app(APP_GPUX);return;} if(k==KEY_TAB){for(int s=1;s<APP_COUNT;s++){int i=(active+s)%APP_COUNT;if(i&&win[i].open){active=i;break;}}return;} if(k==KEY_ESC){close_app(active);return;} if(active==APP_EXPLORER){int cnt=item_count(path_id);if(k==KEY_UP&&file_sel>0)file_sel--;else if(k==KEY_DOWN&&file_sel<cnt-1)file_sel++;else if(k==KEY_ENTER)explorer_open();} else if(active==APP_CONSOLE){if(k==KEY_ENTER)exec_cmd();else if(k==KEY_BS&&input_len>0){input[--input_len]=0;}else if(k>=32&&k<=126&&input_len+1<IN_LEN){input[input_len++]=(char)k;input[input_len]=0;}} else if(active==APP_CODE)key_code(k); else if(active==APP_SETTINGS){if(k==KEY_UP&&set_sel>0)set_sel--;else if(k==KEY_DOWN&&set_sel<THEME_N-1)set_sel++;else if(k==KEY_ENTER){theme_id=set_sel;init_colors();}}}

static int read_key(void){EFI_INPUT_KEY key; if(conin->ReadKeyStroke(conin,&key)!=EFI_SUCCESS)return 0; if(key.UnicodeChar){if(key.UnicodeChar==13)return KEY_ENTER; if(key.UnicodeChar==8)return KEY_BS; if(key.UnicodeChar==9)return KEY_TAB; if(key.UnicodeChar<128)return (int)key.UnicodeChar;} switch(key.ScanCode){case SCAN_UP:return KEY_UP;case SCAN_DOWN:return KEY_DOWN;case SCAN_LEFT:return KEY_LEFT;case SCAN_RIGHT:return KEY_RIGHT;case SCAN_F1:return KEY_F1;case SCAN_F2:return KEY_F2;case SCAN_F3:return KEY_F3;case SCAN_F4:return KEY_F4;case SCAN_F5:return KEY_F5;case SCAN_F6:return KEY_F6;case SCAN_F7:return KEY_F7;case SCAN_F8:return KEY_F8;case SCAN_F9:return KEY_F9;case SCAN_F10:return KEY_F10;case SCAN_ESC:return KEY_ESC;default:return 0;}}
static int rel_delta(INT32 v, UINT64 res){
    if(v==0) return 0;
    if(res>0 && (v>256 || v<-256)){ INT64 q=(INT64)v/(INT64)res; if(q==0) q=(v>0)?1:-1; if(q>64)q=64; if(q<-64)q=-64; return (int)q; }
    if(v>64) return (int)(v/8);
    if(v<-64) return (int)(v/8);
    return (int)v;
}
static void poll_pointer(void){
    int old=mb;
    int got=0;
    if(ap){
        EFI_ABSOLUTE_POINTER_STATE s;
        if(ap->GetState(ap,&s)==EFI_SUCCESS){
            UINT64 minx=ap->Mode->AbsoluteMinX,maxx=ap->Mode->AbsoluteMaxX,miny=ap->Mode->AbsoluteMinY,maxy=ap->Mode->AbsoluteMaxY;
            if(maxx>minx)mx=(int)(((s.CurrentX-minx)*(UINT64)(W-1))/(maxx-minx));
            if(maxy>miny)my=(int)(((s.CurrentY-miny)*(UINT64)(H-1))/(maxy-miny));
            mb=(s.ActiveButtons&0x3)?1:0;
            got=1;
        }
    }
    if(sp){
        EFI_SIMPLE_POINTER_STATE s;
        if(sp->GetState(sp,&s)==EFI_SUCCESS){
            UINT64 rx=sp->Mode?sp->Mode->ResolutionX:0, ry=sp->Mode?sp->Mode->ResolutionY:0;
            mx+=rel_delta(s.RelativeMovementX,rx);
            my+=rel_delta(s.RelativeMovementY,ry);
            if(mx<0)mx=0;if(my<0)my=0;if(mx>=(int)W)mx=(int)W-1;if(my>=(int)H)my=(int)H-1;
            mb=s.LeftButton?1:0;
            got=1;
        }
    }
    if(!got || ps2_ok) ps2_poll_mouse();
    mp=(mb&&!old); mr=(!mb&&old);
    if(drag&&mb){ win[drag].x=mx-drag_dx; win[drag].y=my-drag_dy; fit(&win[drag]); }
    if(mr)drag=0;
    if(mp)mouse_click();
}


static int init_gop(void){ if(bs->LocateProtocol(&GOP_GUID,NULL,(VOID**)&gop)!=EFI_SUCCESS || !gop || !gop->Mode) return 0; UINT32 best=gop->Mode->Mode; UINT32 bestscore=0; for(UINT32 m=0;m<gop->Mode->MaxMode;m++){UINTN sz=0; EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info=NULL; if(gop->QueryMode(gop,m,&sz,&info)!=EFI_SUCCESS||!info)continue; if(info->PixelFormat==PixelBltOnly)continue; UINT32 ww=info->HorizontalResolution,hh=info->VerticalResolution; UINT32 score=ww*hh; if(score>bestscore && ww<=1280 && hh<=1024){bestscore=score;best=m;}} gop->SetMode(gop,best); W=gop->Mode->Info->HorizontalResolution; H=gop->Mode->Info->VerticalResolution; PITCH=gop->Mode->Info->PixelsPerScanLine; PIXFMT=gop->Mode->Info->PixelFormat; MASK=gop->Mode->Info->PixelInformation; fb=(UINT8*)(UINTN)gop->Mode->FrameBufferBase; drawfb=(UINT8*)backbuffer; return fb&&drawfb&&W&&H&&W<=BACK_MAX_W&&H<=BACK_MAX_H&&PIXFMT!=PixelBltOnly; }

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable){(void)ImageHandle; st=SystemTable; bs=st->BootServices; conin=st->ConIn; conout=st->ConOut; if(bs&&bs->SetWatchdogTimer)bs->SetWatchdogTimer(0,0,0,NULL); if(!init_gop()){fail_text(L"SacramentuOS: GOP framebuffer not available.\r\n");return EFI_UNSUPPORTED;} init_colors(); mx=W/2;my=H/2; init_pointer_backends(); detect_hw(); addline("SacramentuOS UEFI Pixel Console [Version 1.0.1]"); addline("Type HELP or click Start."); addline(pointer_backend); for(int i=0;i<100;i+=5){bg();fill(W/2-220,H/2+25,440,26,rgb(180,180,180));fill(W/2-218,H/2+27,(436*i)/100,22,C_ACCENT);text(W/2-150,H/2-40,"SacramentuOS",C_WHITE);small(W/2-135,H/2-8,"LOADING UEFI GOP DESKTOP 1.0.1",rgb(210,235,255));present();stall(18000);} while(1){int k;while((k=read_key())!=0)handle_key(k);poll_pointer();draw();frame++;stall(16000);} return EFI_SUCCESS; }

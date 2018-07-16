#pragma once

#ifndef __GNUC__
#define __extension__
#define __attribute__(x)
#define __inline__
#define __asm__ __asm
#endif

#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#if !defined(MIDL_PASS)
typedef int INT;
#endif
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#define far
#define near

#undef FAR
#undef  NEAR
#define FAR                 far
#define NEAR                near
#ifndef CONST
#define CONST               const
#endif

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL near           *PBOOL;
typedef BOOL far            *LPBOOL;
typedef BYTE near           *PBYTE;
typedef BYTE far            *LPBYTE;
typedef int near            *PINT;
typedef int far             *LPINT;
typedef WORD near           *PWORD;
typedef WORD far            *LPWORD;
typedef long far            *LPLONG;
typedef DWORD near          *PDWORD;
typedef DWORD far           *LPDWORD;
typedef void far            *LPVOID;
typedef CONST void far      *LPCVOID;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

typedef UINT FAR   *LPUINT;

#ifndef _MAC
typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character
#else
// some Macintosh compilers don't define wchar_t in a convenient location, or define it as a char
typedef unsigned short WCHAR;    // wc,   16-bit UNICODE character
#endif

typedef CHAR *PCHAR, *LPCH, *PCH;
typedef CONST CHAR *LPCCH, *PCCH;

typedef WCHAR *PWCHAR, *LPWCH, *PWCH;
typedef CONST WCHAR *LPCWCH, *PCWCH;

typedef CHAR *NPSTR, *LPSTR, *PSTR;
typedef PSTR *PZPSTR;
typedef CONST PSTR *PCZPSTR;
typedef CONST CHAR *LPCSTR, *PCSTR;
typedef PCSTR *PZPCSTR;

typedef WCHAR *NWPSTR, *LPWSTR, *PWSTR;
typedef CONST WCHAR *LPCWSTR, *PCWSTR;
typedef PCWSTR *PZPCWSTR;

typedef LPCH LPTCH, PTCH;
typedef LPCCH LPCTCH, PCTCH;
typedef LPSTR PTSTR, LPTSTR, PUTSTR, LPUTSTR;
typedef LPCSTR PCTSTR, LPCTSTR, PCUTSTR, LPCUTSTR;

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

#define _strnicmp strnicmp

#define _T(x)       (x)
#define _W64

typedef _W64 int INT_PTR, *PINT_PTR;
typedef _W64 unsigned int UINT_PTR, *PUINT_PTR;

typedef _W64 long LONG_PTR, *PLONG_PTR;
typedef _W64 unsigned long ULONG_PTR, *PULONG_PTR;

typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

typedef unsigned long ULONG;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;

#ifdef SORTPP_PASS
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
#else
#define C_ASSERT(e) /* nothing */
#endif

typedef struct _DVD_DESCRIPTOR_HEADER {
	USHORT Length;
	UCHAR Reserved[2];
#if !defined(__midl)
	UCHAR Data[0];
#endif
} DVD_DESCRIPTOR_HEADER, *PDVD_DESCRIPTOR_HEADER;
C_ASSERT(sizeof(DVD_DESCRIPTOR_HEADER) == 4);

// format 0x00 - DvdPhysicalDescriptor
typedef struct _DVD_LAYER_DESCRIPTOR {
	UCHAR BookVersion : 4;      // in MMC 5 :   Part Version
	UCHAR BookType : 4;         //              Disk Category
	UCHAR MinimumRate : 4;      //              Maximum Rate
	UCHAR DiskSize : 4;
	UCHAR LayerType : 4;
	UCHAR TrackPath : 1;
	UCHAR NumberOfLayers : 2;
	UCHAR Reserved1 : 1;
	UCHAR TrackDensity : 4;
	UCHAR LinearDensity : 4;
	ULONG StartingDataSector;   //              3bytes + 1 zeroed byte
	ULONG EndDataSector;        //              3bytes + 1 zeroed byte
	ULONG EndLayerZeroSector;   //              3bytes + 1 zeroed byte
	UCHAR Reserved5 : 7;
	UCHAR BCAFlag : 1;
	// The large Media Specific field is not declared here to enable stack allocation
} DVD_LAYER_DESCRIPTOR, *PDVD_LAYER_DESCRIPTOR;
C_ASSERT(sizeof(DVD_LAYER_DESCRIPTOR) == 17);
typedef struct _DVD_FULL_LAYER_DESCRIPTOR {
	DVD_LAYER_DESCRIPTOR commonHeader;
	UCHAR MediaSpecific[2031];
} DVD_FULL_LAYER_DESCRIPTOR, *PDVD_FULL_LAYER_DESCRIPTOR;
C_ASSERT(sizeof(DVD_FULL_LAYER_DESCRIPTOR) == 2048);

// format 0x01 - DvdCopyrightDescriptor
typedef struct _DVD_COPYRIGHT_DESCRIPTOR {
	UCHAR CopyrightProtectionType;
	UCHAR RegionManagementInformation;
	USHORT Reserved;
} DVD_COPYRIGHT_DESCRIPTOR, *PDVD_COPYRIGHT_DESCRIPTOR;
C_ASSERT(sizeof(DVD_COPYRIGHT_DESCRIPTOR) == 4);

// format 0x02 - DvdDiskKeyDescriptor
typedef struct _DVD_DISK_KEY_DESCRIPTOR {
	UCHAR DiskKeyData[2048];
} DVD_DISK_KEY_DESCRIPTOR, *PDVD_DISK_KEY_DESCRIPTOR;
C_ASSERT(sizeof(DVD_DISK_KEY_DESCRIPTOR) == 2048);

// format 0x03 - DvdBCADescriptor
typedef struct _DVD_BCA_DESCRIPTOR {
	UCHAR BCAInformation[0];
} DVD_BCA_DESCRIPTOR, *PDVD_BCA_DESCRIPTOR;

// format 0x04 - DvdManufacturerDescriptor
typedef struct _DVD_MANUFACTURER_DESCRIPTOR {
	UCHAR ManufacturingInformation[2048];
} DVD_MANUFACTURER_DESCRIPTOR, *PDVD_MANUFACTURER_DESCRIPTOR;
C_ASSERT(sizeof(DVD_MANUFACTURER_DESCRIPTOR) == 2048);

// format 0x05 - not defined in enum
typedef struct _DVD_COPYRIGHT_MANAGEMENT_DESCRIPTOR {
	union {
		struct {
			UCHAR CopyProtectionMode : 4;
			UCHAR ContentGenerationManagementSystem : 2;
			UCHAR CopyProtectedSector : 1;
			UCHAR CopyProtectedMaterial : 1;
		} Dvdrom;
		struct {
			UCHAR Reserved0001 : 4;
			UCHAR ContentGenerationManagementSystem : 2;
			UCHAR Reserved0002 : 1;
			UCHAR CopyProtectedMaterial : 1;
		} DvdRecordable_Version1;
		struct {
			UCHAR Reserved0003;
		} Dvdram;
		struct {
			UCHAR Reserved0004 : 2;
			UCHAR ADP_TY : 2; // what is this mean?
			UCHAR Reserved0005 : 4;
		} DvdRecordable;
		UCHAR CPR_MAI;
	};
	UCHAR Reserved0[3];
} DVD_COPYRIGHT_MANAGEMENT_DESCRIPTOR, *PDVD_COPYRIGHT_MANAGEMENT_DESCRIPTOR;
C_ASSERT(FIELD_OFFSET(DVD_COPYRIGHT_MANAGEMENT_DESCRIPTOR, Reserved0) == 1);
C_ASSERT(sizeof(DVD_COPYRIGHT_MANAGEMENT_DESCRIPTOR) == 4);
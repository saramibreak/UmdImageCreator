/**
 * Copyright 2018-2022 sarami
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include "windef.h"
#include "forwardDeclaration.h"
#include <stdio.h>

typedef struct _LOG_FILE {
	FILE* fpDrive;
	FILE* fpDisc;
	FILE* fpVolDesc;
	FILE* fpMainInfo;
	FILE* fpMainError;
} LOG_FILE, *PLOG_FILE;

typedef struct _VOLUME_DESCRIPTOR {
	struct _ISO_9660 {
		UINT uiLogicalBlkCoef;
		UINT uiPathTblSize;
		UINT uiPathTblPos;
		UINT uiRootDataLen;
	} ISO_9660;
	struct _JOLIET {
		UINT uiLogicalBlkCoef;
		UINT uiPathTblSize;
		UINT uiPathTblPos;
		UINT uiRootDataLen;
	} JOLIET;
} VOLUME_DESCRIPTOR, *PVOLUME_DESCRIPTOR;

typedef struct _PATH_TABLE_RECORD {
	UINT uiDirNameLen;
	UINT uiPosOfDir;
	UINT uiNumOfUpperDir;
	CHAR szDirName[MAX_FNAME_FOR_VOLUME];
	CHAR padding[3];
	UINT uiDirSize; // This is actually DIRECTORY RECORD info
} PATH_TABLE_RECORD, * PPATH_TABLE_RECORD;

typedef struct _MS_INFO {
	unsigned long cluster_max;	//総クラスタ数
	unsigned long cluster_free;	//空きクラスタ数
	int sector_max;				//きりのいい空きクラスタ数
	int sector_size;			//セクタあたりのバイト数
	int sector_count;			//クラスタあたりのセクタ数
	unsigned long long smax;	//総容量
	unsigned long long sfree;	//空き容量
	unsigned long long sused;	//使用容量
} MS_INFO, *PMS_INFO;

/// <summary>
///  from scsi.h
/// </summary>
//typedef USHORT VERSION_DESCRIPTOR, * PVERSION_DESCRIPTOR;

#pragma pack(push, inquiry, 1)
typedef struct _INQUIRYDATA {
    UCHAR DeviceType : 5;
    UCHAR DeviceTypeQualifier : 3;
    UCHAR DeviceTypeModifier : 7;
    UCHAR RemovableMedia : 1;
    union {
        UCHAR Versions;
        struct {
            UCHAR ANSIVersion : 3;
            UCHAR ECMAVersion : 3;
            UCHAR ISOVersion : 2;
        };
    };
    UCHAR ResponseDataFormat : 4;
    UCHAR HiSupport : 1;
    UCHAR NormACA : 1;
    UCHAR TerminateTask : 1;
    UCHAR AERC : 1;
    UCHAR AdditionalLength;
    union {
        UCHAR Reserved;
        struct {
            UCHAR PROTECT : 1;
            UCHAR Reserved_1 : 2;
            UCHAR ThirdPartyCoppy : 1;
            UCHAR TPGS : 2;
            UCHAR ACC : 1;
            UCHAR SCCS : 1;
        };
    };
    UCHAR Addr16 : 1;               // defined only for SIP devices.
    UCHAR Addr32 : 1;               // defined only for SIP devices.
    UCHAR AckReqQ : 1;               // defined only for SIP devices.
    UCHAR MediumChanger : 1;
    UCHAR MultiPort : 1;
    UCHAR ReservedBit2 : 1;
    UCHAR EnclosureServices : 1;
    UCHAR ReservedBit3 : 1;
    UCHAR SoftReset : 1;
    UCHAR CommandQueue : 1;
    UCHAR TransferDisable : 1;      // defined only for SIP devices.
    UCHAR LinkedCommands : 1;
    UCHAR Synchronous : 1;          // defined only for SIP devices.
    UCHAR Wide16Bit : 1;            // defined only for SIP devices.
    UCHAR Wide32Bit : 1;            // defined only for SIP devices.
    UCHAR RelativeAddressing : 1;
    UCHAR VendorId[8];
    UCHAR ProductId[16];
    UCHAR ProductRevisionLevel[4];
    UCHAR VendorSpecific[20];
    UCHAR Reserved3[2];
//    VERSION_DESCRIPTOR VersionDescriptors[8];
    UCHAR Reserved4[30];
} INQUIRYDATA, * PINQUIRYDATA;
#pragma pack(pop, inquiry)

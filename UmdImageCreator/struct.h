/**
 * Copyright 2018 sarami
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
	FILE* fpDisc;
	FILE* fpVolDesc;
	FILE* fpMainInfo;
	FILE* fpMainError;
} LOG_FILE, *PLOG_FILE;

typedef struct _VOLUME_DESCRIPTOR {
	struct _ISO_9660 {
		DWORD dwLogicalBlkCoef;
		DWORD dwPathTblSize;
		DWORD dwPathTblPos;
		DWORD dwRootDataLen;
	} ISO_9660;
	struct _JOLIET {
		DWORD dwLogicalBlkCoef;
		DWORD dwPathTblSize;
		DWORD dwPathTblPos;
		DWORD dwRootDataLen;
	} JOLIET;
} VOLUME_DESCRIPTOR, *PVOLUME_DESCRIPTOR;

typedef struct _DIRECTORY_RECORD {
	UINT uiDirNameLen;
	UINT uiPosOfDir;
	UINT uiNumOfUpperDir;
	CHAR szDirName[MAX_FNAME_FOR_VOLUME];
	UINT uiDirSize;
} DIRECTORY_RECORD, *PDIRECTORY_RECORD;

typedef struct _MS_INFO {
	unsigned long cluster_max;	//総クラスタ数
	unsigned long cluster_free;	//空きクラスタ数
	int sector_max;				//きりのいい空きクラスタ数
	int sector_size;			//セクタあたりのバイト数
	int sector_count;			//クラスタあたりのセクタ数
	unsigned long long smax;					//総容量
	unsigned long long sfree;					//空き容量
	unsigned long long sused;					//使用容量
} MS_INFO, *PMS_INFO;

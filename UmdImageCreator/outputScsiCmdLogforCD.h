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
#include "enum.h"
#include <stdlib.h>

VOID OutputFsVolumeDescriptor(
	LPBYTE lpBuf,
	INT nLBA
);

VOID OutputFsVolumeDescriptorForISO9660(
	LPBYTE lpBuf
);
#if 0
VOID OutputFsVolumeDescriptorForJoliet(
	LPBYTE lpBuf
);
#endif
VOID OutputFsDirectoryRecord(
	LPBYTE lpBuf,
	UINT uiExtentPos,
	UINT uiDataLen,
	LPSTR fname,
	PPATH_TABLE_RECORD pPathTblRec,
	UINT uiPathTblIdx
);

BOOL OutputFsPathTableRecord(
	LPBYTE lpBuf,
	UINT uiLogicalBlkCoef,
	UINT uiPathTblPos,
	UINT uiPathTblSize,
	PPATH_TABLE_RECORD pPathTblRec,
	LPUINT uiDirPosNum
);

VOID OutputCDMain(
	LOG_TYPE type,
	LPBYTE lpBuf,
	INT nLBA,
	INT nSize
);

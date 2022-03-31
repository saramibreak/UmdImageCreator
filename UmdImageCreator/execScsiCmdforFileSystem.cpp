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
#include <errno.h>
#include <string.h>

#include "define.h"
#include "convert.h"
#include "execScsiCmdforFileSystem.h"
#include "get.h"
#include "output.h"
#include "outputScsiCmdLogforCD.h"

BOOL ExecReadDisc(
	SceUID uid,
	INT nLBA,
	LPBYTE lpBuf,
	BYTE byTransferLen
) {
	INT seek[4] = { 0 };
	seek[0] = nLBA;
	// 0x01F100A6
	INT result = sceIoIoctl(uid, IOCTL_UMD_SEEK, seek, sizeof(seek), NULL, 0);
	if (result < 0) {
		OutputLastErrorNumAndString(errno, _T(__FUNCTION__), __LINE__);
		return 0;
	}
	INT numberOfSectors = byTransferLen;
	// 0x01F30003
	result = sceIoIoctl(uid, IOCTL_UMD_GET_MAIN_CHANNEL, &numberOfSectors, sizeof(INT), lpBuf, numberOfSectors);
	if (result < 0) {
		OutputLastErrorNumAndString(errno, _T(__FUNCTION__), __LINE__);
		return 0;
	}
	return result;
}

VOID ManageEndOfDirectoryRecord(
	LPINT nSectorNum,
	BYTE byTransferLen,
	UINT uiPaddingLen,
	LPUINT nOfs
) {
	if (*nSectorNum < byTransferLen) {
		*nOfs += uiPaddingLen;
		(*nSectorNum)++;
		return;
	}
}

BOOL ReadDirectoryRecordDetail(
	SceUID uid,
	INT nLBA,
	LPBYTE lpBuf,
	BYTE byTransferLen,
	UINT uiDirPosNum,
	DWORD dwLogicalBlkCoef,
	PPATH_TABLE_RECORD pPathTblRec,
	UINT uiPathTblIdx
) {
	if (!ExecReadDisc(uid, nLBA, lpBuf, byTransferLen)) {
		return FALSE;
	}
	BYTE byRoop = byTransferLen;
	for (BYTE i = 0; i < byRoop; i++) {
		OutputMainChannel(fileMainInfo, lpBuf + DISC_MAIN_DATA_SIZE * i, nLBA + i, DISC_MAIN_DATA_SIZE);
	}

	UINT uiOfs = 0;
	for (INT nSectorNum = 0; nSectorNum < byRoop;) {
		if (*(lpBuf + uiOfs) == 0) {
			break;
		}
		OutputVolDescWithLBALog1("Directory Record", nLBA + nSectorNum);
		for (;;) {
			CHAR szCurDirName[MAX_FNAME_FOR_VOLUME] = { 0 };
			LPBYTE lpPathTblRec = lpBuf + uiOfs;
			if (lpPathTblRec[0] >= MIN_LEN_DR) {
				UINT uiExtentPos = GetSizeOrUintForVolDesc(lpPathTblRec + 2, 0x10000000) / dwLogicalBlkCoef;
				UINT uiDataLen = GetSizeOrUintForVolDesc(lpPathTblRec + 10, 0x10000000);
				OutputFsDirectoryRecord(
					lpPathTblRec, uiExtentPos, uiDataLen, szCurDirName, pPathTblRec, uiPathTblIdx);
				OutputVolDescLog("\n");
				uiOfs += lpPathTblRec[0];

				// not upper and current directory
				if (!(lpPathTblRec[32] == 1 && szCurDirName[0] == 0) &&
					!(lpPathTblRec[32] == 1 && szCurDirName[0] == 1)) {
					if (lpPathTblRec[25] & 0x02) {
						for (UINT i = 1; i < uiDirPosNum; i++) {
							if (uiExtentPos == pPathTblRec[i].uiPosOfDir &&
								!_strnicmp(szCurDirName, pPathTblRec[i].szDirName, MAX_FNAME_FOR_VOLUME)) {
								pPathTblRec[i].uiDirSize = PadSizeForVolDesc(uiDataLen);
								break;
							}
						}
					}
				}
				else {
					OutputVolDescLog("\n");
				}

				if (uiOfs == (UINT)(DISC_MAIN_DATA_SIZE * (nSectorNum + 1))) {
					nSectorNum++;
					break;
				}
			}
			else {
				UINT uiPaddingLen = DISC_MAIN_DATA_SIZE * (nSectorNum + 1) - uiOfs;
				if (uiPaddingLen > MIN_LEN_DR) {
					BYTE byNextLenDR = lpPathTblRec[MIN_LEN_DR];
					if (byNextLenDR >= MIN_LEN_DR) {
						// Amiga Tools 4 : The second of Direcory Record (0x22 - 0x43) is corrupt...
						// ========== LBA[040915, 0x09fd3]: Main Channel ==========
						//        +0 +1 +2 +3 +4 +5 +6 +7  +8 +9 +A +B +C +D +E +F
						// 0000 : 22 00 D3 9F 00 00 00 00  9F D3 00 08 00 00 00 00   "...............
						// 0010 : 08 00 60 02 1D 17 18 2C  00 02 00 00 01 00 00 01   ..`....,........
						// 0020 : 01 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ................
						// 0030 : 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ................
						// 0040 : 00 00 01 01 2E 00 09 A0  00 00 00 00 A0 09 D8 01   ................
						OutputVolDescLog(
							"LBA %d: Direcory Record is corrupt. Skip reading from %u to %u byte\n"
							, nLBA, uiOfs, uiOfs + MIN_LEN_DR - 1);
						uiOfs += MIN_LEN_DR;
						break;
					}
					else {
						ManageEndOfDirectoryRecord(&nSectorNum, byRoop, uiPaddingLen, &uiOfs);
						break;
					}
				}
				else {
					ManageEndOfDirectoryRecord(&nSectorNum, byRoop, uiPaddingLen, &uiOfs);
					break;
				}
			}
		}
	}
	return TRUE;
}

BOOL ReadDirectoryRecord(
	SceUID uid,
	LPBYTE lpBuf,
	DWORD dwLogicalBlkCoef,
	DWORD dwRootDataLen,
	PPATH_TABLE_RECORD pPathTblRec,
	UINT uiDirPosNum
) {
	BYTE byTransferLen = 1;
	pPathTblRec[0].uiDirSize = dwRootDataLen;

	for (UINT uiPathTblIdx = 0; uiPathTblIdx < uiDirPosNum; uiPathTblIdx++) {
		INT nLBA = (INT)pPathTblRec[uiPathTblIdx].uiPosOfDir;
		if (pPathTblRec[uiPathTblIdx].uiDirSize > DISC_MAIN_DATA_SIZE) {
			DWORD dwAdditionalTransferLen = pPathTblRec[uiPathTblIdx].uiDirSize / DISC_MAIN_DATA_SIZE;
			byTransferLen = 1;
			OutputMainInfoLog("nLBA %d, uiDirSize: %u, byTransferLen: %lu [L:%d]\n"
				, nLBA, pPathTblRec[uiPathTblIdx].uiDirSize, dwAdditionalTransferLen, (INT)__LINE__);

			for (DWORD n = 0; n < dwAdditionalTransferLen; n++) {
				if (!ReadDirectoryRecordDetail(uid, nLBA
					, lpBuf, byTransferLen, uiDirPosNum, dwLogicalBlkCoef, pPathTblRec, uiPathTblIdx)) {
					continue;
				}
				nLBA += byTransferLen;
			}
		}
		else {
			if (pPathTblRec[uiPathTblIdx].uiDirSize == 0 || byTransferLen == 0) {
				OutputMainErrorLog("Directory Record is invalid\n");
				return FALSE;
			}
			byTransferLen = pPathTblRec[uiPathTblIdx].uiDirSize / DISC_MAIN_DATA_SIZE;
			OutputMainInfoLog("nLBA %d, uiDirSize: %u, byTransferLen: %d [L:%d]\n"
				, nLBA, pPathTblRec[uiPathTblIdx].uiDirSize, byTransferLen, (INT)__LINE__);

			if (!ReadDirectoryRecordDetail(uid, nLBA
				, lpBuf, byTransferLen, uiDirPosNum, dwLogicalBlkCoef, pPathTblRec, uiPathTblIdx)) {
				continue;
			}
		}
#ifdef PBP
		OutputString(_T("\rReading DirectoryRecord %4d/%4d"), uiPathTblIdx + 1, uiDirPosNum);
#else
		pspDebugScreenSetXY(0, pspDebugScreenGetY());
		pspPrintf("Reading DirectoryRecord %4d/%4d", uiPathTblIdx + 1, uiDirPosNum);
#endif
	}
	pspPrintf("\n");
	return TRUE;
}

BOOL ReadPathTableRecord(
	SceUID uid,
	UINT uiLogicalBlkCoef,
	UINT uiPathTblSize,
	UINT uiPathTblPos,
	PPATH_TABLE_RECORD pPathTblRec,
	LPUINT uiDirPosNum
) {
	DWORD dwBufSize = DISC_MAIN_DATA_SIZE - (uiPathTblSize % DISC_MAIN_DATA_SIZE) + uiPathTblSize;
	BYTE byTransferLen = dwBufSize / DISC_MAIN_DATA_SIZE;

	LPBYTE lpBuf = (LPBYTE)calloc(dwBufSize, sizeof(BYTE));
	if (!lpBuf) {
		OutputLastErrorNumAndString(errno, _T(__FUNCTION__), __LINE__);
		return FALSE;
	}
	CONST UINT uiMaxPTSize = 32768;
	BOOL bRet = TRUE;
	if (uiPathTblSize > uiMaxPTSize) {
		DWORD dwAdditionalTransferLen = uiPathTblSize / uiMaxPTSize;
		OutputMainInfoLog("PathTblSize: %u, byTransferLen: %d [L:%d]\n", uiMaxPTSize, byTransferLen, (INT)__LINE__);

		for (DWORD n = 0; n < dwAdditionalTransferLen; n++) {
			if (!ExecReadDisc(uid, (INT)uiPathTblPos, lpBuf + 65535 * n, byTransferLen)) {
				return FALSE;
			}
			for (BYTE i = 0; i < byTransferLen; i++) {
				OutputMainChannel(fileMainInfo, lpBuf + DISC_MAIN_DATA_SIZE * i, (INT)uiPathTblPos + i, DISC_MAIN_DATA_SIZE);
			}
			uiPathTblPos += byTransferLen;
		}
		UINT uiLastPathTblSize = uiPathTblSize % uiMaxPTSize;
		byTransferLen = uiLastPathTblSize / DISC_MAIN_DATA_SIZE;
		DWORD dwBufOfs = uiMaxPTSize * dwAdditionalTransferLen;

		OutputMainInfoLog(
			"PathTblSize: %u, byTransferLen: %d [L:%d]\n", uiLastPathTblSize, byTransferLen, (INT)__LINE__);
		if (!ExecReadDisc(uid, (INT)uiPathTblPos, lpBuf + dwBufOfs, byTransferLen)) {
			return FALSE;
		}
		for (BYTE i = 0; i < byTransferLen; i++) {
			OutputMainChannel(fileMainInfo, lpBuf + dwBufOfs + DISC_MAIN_DATA_SIZE * i, (INT)uiPathTblPos + i, DISC_MAIN_DATA_SIZE);
		}
		if (!OutputFsPathTableRecord(lpBuf, uiLogicalBlkCoef, uiPathTblPos, uiPathTblSize, pPathTblRec, uiDirPosNum)) {
			return FALSE;
		}
	}
	else {
		
		OutputMainInfoLog(
			"PathTblSize: %u, byTransferLen: %d [L:%d]\n", uiPathTblSize, byTransferLen, (INT)__LINE__);
		if (!ExecReadDisc(uid, (INT)uiPathTblPos, lpBuf, byTransferLen)) {
			return FALSE;
		}
		for (BYTE i = 0; i < byTransferLen; i++) {
			OutputMainChannel(fileMainInfo, lpBuf + DISC_MAIN_DATA_SIZE * i, (INT)uiPathTblPos + i, DISC_MAIN_DATA_SIZE);
		}
		if (!OutputFsPathTableRecord(lpBuf, uiLogicalBlkCoef, uiPathTblPos, uiPathTblSize, pPathTblRec, uiDirPosNum)) {
			return FALSE;
		}
	}
	OutputVolDescLog("Directory Num: %u\n", *uiDirPosNum);
	FreeAndNull(lpBuf);
	return bRet;
}

BOOL ReadVolumeDescriptor(
	SceUID uid,
	LPBYTE lpBuf,
	INT nPVD,
	LPBOOL lpReadVD,
	PVOLUME_DESCRIPTOR pVolDesc,
	LPINT nVolumeSpaceSize
) {
	INT nTmpLBA = nPVD;
	for (;;) {
		if (!ExecReadDisc(uid, nTmpLBA, lpBuf, 1)) {
			break;
		}
		if (!strncmp((LPCH)&lpBuf[1], "CD001", 5)) {
			if (nTmpLBA == nPVD) {
				*nVolumeSpaceSize = GetSizeOrUintForVolDesc(lpBuf + 80, DWORD(0xa0000 * DISC_MAIN_DATA_SIZE));
				WORD wLogicalBlkSize = GetSizeOrWordForVolDesc(lpBuf + 128);
				pVolDesc->ISO_9660.uiLogicalBlkCoef = (BYTE)(DISC_MAIN_DATA_SIZE / wLogicalBlkSize);
				pVolDesc->ISO_9660.uiPathTblSize =
					GetSizeOrUintForVolDesc(lpBuf + 132, (DWORD)(0xa0000 * DISC_MAIN_DATA_SIZE));
				pVolDesc->ISO_9660.uiPathTblPos = MAKEUINT(MAKEWORD(lpBuf[140], lpBuf[141]),
					MAKEWORD(lpBuf[142], lpBuf[143])) / pVolDesc->ISO_9660.uiLogicalBlkCoef;
				if (pVolDesc->ISO_9660.uiPathTblPos == 0) {
					pVolDesc->ISO_9660.uiPathTblPos = MAKEUINT(MAKEWORD(lpBuf[151], lpBuf[150]),
						MAKEWORD(lpBuf[149], lpBuf[148]));
				}
				pVolDesc->ISO_9660.uiRootDataLen =
					GetSizeOrUintForVolDesc(lpBuf + 166, (DWORD)(0xa0000 * DISC_MAIN_DATA_SIZE));
				if (pVolDesc->ISO_9660.uiRootDataLen > 0) {
					pVolDesc->ISO_9660.uiRootDataLen = PadSizeForVolDesc(pVolDesc->ISO_9660.uiRootDataLen);
				}
				*lpReadVD = TRUE;
			}
			else if (lpBuf[0] == 2) {
				WORD wLogicalBlkSize = GetSizeOrWordForVolDesc(lpBuf + 128);
				pVolDesc->JOLIET.uiLogicalBlkCoef = (BYTE)(DISC_MAIN_DATA_SIZE / wLogicalBlkSize);
				pVolDesc->JOLIET.uiPathTblSize =
					GetSizeOrUintForVolDesc(lpBuf + 132, (DWORD)(0xa0000 * DISC_MAIN_DATA_SIZE));
				pVolDesc->JOLIET.uiPathTblPos = MAKEUINT(MAKEWORD(lpBuf[140], lpBuf[141]),
					MAKEWORD(lpBuf[142], lpBuf[143])) / pVolDesc->JOLIET.uiLogicalBlkCoef;
				if (pVolDesc->JOLIET.uiPathTblPos == 0) {
					pVolDesc->JOLIET.uiPathTblPos = MAKEUINT(MAKEWORD(lpBuf[151], lpBuf[150]),
						MAKEWORD(lpBuf[149], lpBuf[148]));
				}
				pVolDesc->JOLIET.uiRootDataLen =
					GetSizeOrUintForVolDesc(lpBuf + 166, (DWORD)(0xa0000 * DISC_MAIN_DATA_SIZE));
				if (pVolDesc->JOLIET.uiRootDataLen > 0) {
					pVolDesc->JOLIET.uiRootDataLen = PadSizeForVolDesc(pVolDesc->JOLIET.uiRootDataLen);
				}
				*lpReadVD = TRUE;
			}
			OutputMainChannel(fileMainInfo, lpBuf, nTmpLBA, DISC_MAIN_DATA_SIZE);
			OutputFsVolumeDescriptor(lpBuf, nTmpLBA++);
		}
		else {
			break;
		}
	}
	return TRUE;
}

BOOL ReadDVDForFileSystem(
	SceUID uid,
	LPINT nVolumeSpaceSize
) {
	BOOL bPVD = FALSE;
	VOLUME_DESCRIPTOR volDesc;
	BYTE buf[DISC_MAIN_DATA_SIZE] = { 0 };

	pspPrintf("Reading VolumeDescriptor\n");
	if (!ReadVolumeDescriptor(uid, buf, 16, &bPVD, &volDesc, nVolumeSpaceSize)) {
		return FALSE;
	}
	if (bPVD) {
		PPATH_TABLE_RECORD pPathTblRec = (PPATH_TABLE_RECORD)calloc(PATH_TABLE_RECORD_SIZE, sizeof(PATH_TABLE_RECORD));
		if (!pPathTblRec) {
			OutputLastErrorNumAndString(errno, _T(__FUNCTION__), __LINE__);
			return FALSE;
		}
		UINT uiDirPosNum = 0;
		pspPrintf("Reading PathTableRecord\n");
		if (!ReadPathTableRecord(uid
			, volDesc.ISO_9660.uiLogicalBlkCoef, volDesc.ISO_9660.uiPathTblSize
			, volDesc.ISO_9660.uiPathTblPos, pPathTblRec, &uiDirPosNum)) {
			FreeAndNull(pPathTblRec);
			return FALSE;
		}
		if (!ReadDirectoryRecord(uid, buf
			, volDesc.ISO_9660.uiLogicalBlkCoef, volDesc.ISO_9660.uiRootDataLen, pPathTblRec, uiDirPosNum)) {
			FreeAndNull(pPathTblRec);
			return FALSE;
		}
		FreeAndNull(pPathTblRec);
	}

	return TRUE;
}

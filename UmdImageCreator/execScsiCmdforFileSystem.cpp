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
#include "define.h"
#include "convert.h"
#include "execScsiCmdforFileSystem.h"
#include "get.h"
#include "output.h"
#include "outputScsiCmdLogforCD.h"

#include <errno.h>
#include <string.h>

extern int nWriteToY;

BOOL ExecReadDisc(
	SceUID uid,
	INT nLBA,
	LPBYTE lpBuf,
	BYTE byTransferLen
) {
	INT seek[4] = { 0 };
	seek[0] = nLBA;
	INT result = sceIoIoctl(uid, 0x01F100A6, seek, sizeof(seek), NULL, 0);
	if (result < 0) {
		OutputLastErrorNumAndString(errno, _T(__FUNCTION__), __LINE__);
		return 0;
	}
	INT numberOfSectors = byTransferLen;
	result = sceIoIoctl(uid, 0x01F30003, &numberOfSectors, sizeof(INT), lpBuf, numberOfSectors);
	if (result < 0) {
		OutputLastErrorNumAndString(errno, _T(__FUNCTION__), __LINE__);
		return 0;
	}
	return result;
}

VOID ManageEndOfDirectoryRecord(
	LPINT nSectorNum,
	BYTE byTransferLen,
	UINT uiZeroPaddingNum,
	LPBYTE lpDirRec,
	LPUINT nOfs
) {
	if (*nSectorNum < byTransferLen) {
		UINT j = 0;
		for (; j < uiZeroPaddingNum; j++) {
			if (lpDirRec[j] != 0) {
				break;
			}
		}
		if (j == uiZeroPaddingNum) {
			*nOfs += uiZeroPaddingNum;
			(*nSectorNum)++;
			return;
		}
	}
	else {
		return;
	}
}

BOOL ReadDirectoryRecordDetail(
	SceUID uid,
	INT nLBA,
	LPBYTE lpBuf,
	BYTE byTransferLen,
	INT nDirPosNum,
	DWORD dwLogicalBlkCoef,
	PDIRECTORY_RECORD pDirRec
) {
	if (!ExecReadDisc(uid, nLBA, lpBuf, byTransferLen)) {
		return FALSE;
	}
	BYTE byRoop = byTransferLen;
	for (BYTE i = 0; i < byRoop; i++) {
		OutputCDMain(fileMainInfo, lpBuf + DISC_RAW_READ_SIZE * i, nLBA + i, DISC_RAW_READ_SIZE);
	}

	UINT uiOfs = 0;
	for (INT nSectorNum = 0; nSectorNum < byRoop;) {
		if (*(lpBuf + uiOfs) == 0) {
			break;
		}
		OutputVolDescLogA(
			OUTPUT_DHYPHEN_PLUS_STR_WITH_LBA_F(Directory Record), nLBA + nSectorNum, nLBA + nSectorNum);
		for (;;) {
			CHAR szCurDirName[MAX_FNAME_FOR_VOLUME] = { 0 };
			LPBYTE lpDirRec = lpBuf + uiOfs;
			if (lpDirRec[0] >= MIN_LEN_DR) {
				if (lpDirRec[0] == MIN_LEN_DR && uiOfs > 0 && uiOfs % DISC_RAW_READ_SIZE == 0) {
					// SimCity 3000 (USA)
					OutputVolDescLogA(
						"Direcory record size of the %d sector maybe incorrect. Skip the reading of this sector\n", nLBA);
					nSectorNum++;
					break;
				}
				DWORD dwExtentPos = GetSizeOrDwordForVolDesc(lpDirRec + 2, 0x10000000) / dwLogicalBlkCoef;
				DWORD dwDataLen = GetSizeOrDwordForVolDesc(lpDirRec + 10, 0x10000000);
				OutputFsDirectoryRecord(
					lpDirRec, dwExtentPos, dwDataLen, szCurDirName);
				OutputVolDescLogA("\n");
				uiOfs += lpDirRec[0];

				if ((lpDirRec[25] & 0x02)
					&& !(lpDirRec[32] == 1 && szCurDirName[0] == 0)
					&& !(lpDirRec[32] == 1 && szCurDirName[0] == 1)) {
					// not upper and current directory 
					for (INT i = 1; i < nDirPosNum; i++) {
						if (dwExtentPos == pDirRec[i].uiPosOfDir &&
							!_strnicmp(szCurDirName, pDirRec[i].szDirName, MAX_FNAME_FOR_VOLUME)) {
							pDirRec[i].uiDirSize = PadSizeForVolDesc(dwDataLen);
							break;
						}
					}
				}
				if (uiOfs == (UINT)(DISC_RAW_READ_SIZE * (nSectorNum + 1))) {
					nSectorNum++;
					break;
				}
			}
			else {
				UINT uiZeroPaddingNum = DISC_RAW_READ_SIZE * (nSectorNum + 1) - uiOfs;
				if (uiZeroPaddingNum > MIN_LEN_DR) {
					BYTE byNextLenDR = lpDirRec[MIN_LEN_DR];
					if (byNextLenDR >= MIN_LEN_DR) {
						// Amiga Tools 4 : The second of Direcory Record (0x22 - 0x43) is corrupt...
						// ========== LBA[040915, 0x09fd3]: Main Channel ==========
						//        +0 +1 +2 +3 +4 +5 +6 +7  +8 +9 +A +B +C +D +E +F
						// 0000 : 22 00 D3 9F 00 00 00 00  9F D3 00 08 00 00 00 00   "...............
						// 0010 : 08 00 60 02 1D 17 18 2C  00 02 00 00 01 00 00 01   ..`....,........
						// 0020 : 01 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ................
						// 0030 : 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ................
						// 0040 : 00 00 01 01 2E 00 09 A0  00 00 00 00 A0 09 D8 01   ................
						OutputMainErrorWithLBALogA(
							"Direcory Record is corrupt. Skip reading from %d to %d byte. See _mainInfo.txt\n"
							, nLBA, 0, uiOfs, uiOfs + MIN_LEN_DR - 1);
						uiOfs += MIN_LEN_DR;
						break;
					}
					else {
						ManageEndOfDirectoryRecord(&nSectorNum, byRoop, uiZeroPaddingNum, lpDirRec, &uiOfs);
						break;
					}
				}
				else {
					ManageEndOfDirectoryRecord(&nSectorNum, byRoop, uiZeroPaddingNum, lpDirRec, &uiOfs);
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
	PDIRECTORY_RECORD pDirRec,
	INT nDirPosNum
) {
	BYTE byTransferLen = 1;
	pDirRec[0].uiDirSize = dwRootDataLen;

#ifdef PRX
	nWriteToY += 4;
#endif
	for (INT nDirRecIdx = 0; nDirRecIdx < nDirPosNum; nDirRecIdx++) {
		INT nLBA = (INT)pDirRec[nDirRecIdx].uiPosOfDir;
		if (pDirRec[nDirRecIdx].uiDirSize > DISC_RAW_READ_SIZE) {
			DWORD dwAdditionalTransferLen = pDirRec[nDirRecIdx].uiDirSize / DISC_RAW_READ_SIZE;
			byTransferLen = 1;
			OutputMainInfoLogA("nLBA %d, uiDirSize: %u, byTransferLen: %lu [L:%d]\n"
				, nLBA, pDirRec[nDirRecIdx].uiDirSize, dwAdditionalTransferLen, (INT)__LINE__);

			for (DWORD n = 0; n < dwAdditionalTransferLen; n++) {
				if (!ReadDirectoryRecordDetail(uid, nLBA
					, lpBuf, byTransferLen, nDirPosNum, dwLogicalBlkCoef, pDirRec)) {
					continue;
				}
				nLBA += byTransferLen;
			}
		}
		else {
			if (pDirRec[nDirRecIdx].uiDirSize == 0 || byTransferLen == 0) {
				OutputMainErrorLogA("Directory Record is invalid\n");
				return FALSE;
			}
			byTransferLen = pDirRec[nDirRecIdx].uiDirSize / DISC_RAW_READ_SIZE;
			OutputMainInfoLogA("nLBA %d, uiDirSize: %u, byTransferLen: %d [L:%d]\n"
				, nLBA, pDirRec[nDirRecIdx].uiDirSize, byTransferLen, (INT)__LINE__);

			if (!ReadDirectoryRecordDetail(uid, nLBA
				, lpBuf, byTransferLen, nDirPosNum, dwLogicalBlkCoef, pDirRec)) {
				continue;
			}
		}
#ifdef PBP
		OutputString(_T("\rReading DirectoryRecord %4d/%4d"), nDirRecIdx + 1, nDirPosNum);
#else
		pspDebugScreenSetXY(0, nWriteToY);
		OutputString(_T("Reading DirectoryRecord %4d/%4d"), nDirRecIdx + 1, nDirPosNum);
#endif
	}
	OutputString(_T("\n"));
	return TRUE;
}

BOOL ReadPathTableRecord(
	SceUID uid,
	DWORD dwLogicalBlkCoef,
	DWORD dwPathTblSize,
	DWORD dwPathTblPos,
	PDIRECTORY_RECORD pDirRec,
	LPINT nDirPosNum
) {
	DWORD dwBufSize = DISC_RAW_READ_SIZE - (dwPathTblSize % DISC_RAW_READ_SIZE) + dwPathTblSize;
	BYTE byTransferLen = dwBufSize / DISC_RAW_READ_SIZE;

	LPBYTE lpBuf = (LPBYTE)calloc(dwBufSize, sizeof(BYTE));
	if (!lpBuf) {
		OutputLastErrorNumAndString(errno, _T(__FUNCTION__), __LINE__);
		return FALSE;
	}
	CONST DWORD dwMaxPTSize = 32768;
	BOOL bRet = TRUE;
	if (dwPathTblSize > dwMaxPTSize) {
		DWORD dwAdditionalTransferLen = dwPathTblSize / dwMaxPTSize;
		OutputMainInfoLogA("dwPathTblSize: %lu, byTransferLen: %d [L:%d]\n", dwMaxPTSize, byTransferLen, (INT)__LINE__);

		for (DWORD n = 0; n < dwAdditionalTransferLen; n++) {
			if (!ExecReadDisc(uid, (INT)dwPathTblPos, lpBuf + 65535 * n, byTransferLen)) {
				return FALSE;
			}
			for (BYTE i = 0; i < byTransferLen; i++) {
				OutputCDMain(fileMainInfo, lpBuf + DISC_RAW_READ_SIZE * i, (INT)dwPathTblPos + i, DISC_RAW_READ_SIZE);
			}
			dwPathTblPos += byTransferLen;
		}
		DWORD dwLastPathTblSize = dwPathTblSize % dwMaxPTSize;
		byTransferLen = dwLastPathTblSize / DISC_RAW_READ_SIZE;
		DWORD dwBufOfs = dwMaxPTSize * dwAdditionalTransferLen;

		OutputMainInfoLogA(
			"dwPathTblSize: %lu, byTransferLen: %d [L:%d]\n", dwLastPathTblSize, byTransferLen, (INT)__LINE__);
		if (!ExecReadDisc(uid, (INT)dwPathTblPos, lpBuf + dwBufOfs, byTransferLen)) {
			return FALSE;
		}
		for (BYTE i = 0; i < byTransferLen; i++) {
			OutputCDMain(fileMainInfo, lpBuf + dwBufOfs + DISC_RAW_READ_SIZE * i, (INT)dwPathTblPos + i, DISC_RAW_READ_SIZE);
		}
		if (!OutputFsPathTableRecord(lpBuf, dwLogicalBlkCoef, dwPathTblPos, dwPathTblSize, pDirRec, nDirPosNum)) {
			return FALSE;
		}
	}
	else {
		OutputMainInfoLogA(
			"dwPathTblSize: %lu, byTransferLen: %d [L:%d]\n", dwPathTblSize, byTransferLen, (INT)__LINE__);
		if (!ExecReadDisc(uid, (INT)dwPathTblPos, lpBuf, byTransferLen)) {
			return FALSE;
		}
		for (BYTE i = 0; i < byTransferLen; i++) {
			OutputCDMain(fileMainInfo, lpBuf + DISC_RAW_READ_SIZE * i, (INT)dwPathTblPos + i, DISC_RAW_READ_SIZE);
		}
		if (!OutputFsPathTableRecord(lpBuf, dwLogicalBlkCoef, dwPathTblPos, dwPathTblSize, pDirRec, nDirPosNum)) {
			return FALSE;
		}
	}
	OutputVolDescLogA("Directory Num: %u\n", *nDirPosNum);
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
				*nVolumeSpaceSize = GetSizeOrDwordForVolDesc(lpBuf + 80, DWORD(0xa0000 * DISC_RAW_READ_SIZE));
				DWORD dwLogicalBlkSize = GetSizeOrWordForVolDesc(lpBuf + 128);
				pVolDesc->ISO_9660.dwLogicalBlkCoef = (BYTE)(DISC_RAW_READ_SIZE / dwLogicalBlkSize);
				pVolDesc->ISO_9660.dwPathTblSize =
					GetSizeOrDwordForVolDesc(lpBuf + 132, (DWORD)(0xa0000 * DISC_RAW_READ_SIZE));
				pVolDesc->ISO_9660.dwPathTblPos = MAKEDWORD(MAKEWORD(lpBuf[140], lpBuf[141]),
					MAKEWORD(lpBuf[142], lpBuf[143])) / pVolDesc->ISO_9660.dwLogicalBlkCoef;
				if (pVolDesc->ISO_9660.dwPathTblPos == 0) {
					pVolDesc->ISO_9660.dwPathTblPos = MAKEDWORD(MAKEWORD(lpBuf[151], lpBuf[150]),
						MAKEWORD(lpBuf[149], lpBuf[148]));
				}
				pVolDesc->ISO_9660.dwRootDataLen =
					GetSizeOrDwordForVolDesc(lpBuf + 166, (DWORD)(0xa0000 * DISC_RAW_READ_SIZE));
				if (pVolDesc->ISO_9660.dwRootDataLen > 0) {
					pVolDesc->ISO_9660.dwRootDataLen = PadSizeForVolDesc(pVolDesc->ISO_9660.dwRootDataLen);
				}
				*lpReadVD = TRUE;
			}
			else if (lpBuf[0] == 2) {
				DWORD dwLogicalBlkSize = GetSizeOrWordForVolDesc(lpBuf + 128);
				pVolDesc->JOLIET.dwLogicalBlkCoef = (BYTE)(DISC_RAW_READ_SIZE / dwLogicalBlkSize);
				pVolDesc->JOLIET.dwPathTblSize =
					GetSizeOrDwordForVolDesc(lpBuf + 132, (DWORD)(0xa0000 * DISC_RAW_READ_SIZE));
				pVolDesc->JOLIET.dwPathTblPos = MAKEDWORD(MAKEWORD(lpBuf[140], lpBuf[141]),
					MAKEWORD(lpBuf[142], lpBuf[143])) / pVolDesc->JOLIET.dwLogicalBlkCoef;
				if (pVolDesc->JOLIET.dwPathTblPos == 0) {
					pVolDesc->JOLIET.dwPathTblPos = MAKEDWORD(MAKEWORD(lpBuf[151], lpBuf[150]),
						MAKEWORD(lpBuf[149], lpBuf[148]));
				}
				pVolDesc->JOLIET.dwRootDataLen =
					GetSizeOrDwordForVolDesc(lpBuf + 166, (DWORD)(0xa0000 * DISC_RAW_READ_SIZE));
				if (pVolDesc->JOLIET.dwRootDataLen > 0) {
					pVolDesc->JOLIET.dwRootDataLen = PadSizeForVolDesc(pVolDesc->JOLIET.dwRootDataLen);
				}
				*lpReadVD = TRUE;
			}
			OutputCDMain(fileMainInfo, lpBuf, nTmpLBA, DISC_RAW_READ_SIZE);
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
	BYTE buf[DISC_RAW_READ_SIZE] = { 0 };

	pspPrintf("Reading VolumeDescriptor\n");
	if (!ReadVolumeDescriptor(uid, buf, 16, &bPVD, &volDesc, nVolumeSpaceSize)) {
		return FALSE;
	}
	if (bPVD) {
		PDIRECTORY_RECORD pDirRec = (PDIRECTORY_RECORD)calloc(DIRECTORY_RECORD_SIZE, sizeof(DIRECTORY_RECORD));
		if (!pDirRec) {
			OutputLastErrorNumAndString(errno, _T(__FUNCTION__), __LINE__);
			return FALSE;
		}
		INT nDirPosNum = 0;
		pspPrintf("Reading PathTableRecord\n");
		if (!ReadPathTableRecord(uid
			, volDesc.ISO_9660.dwLogicalBlkCoef, volDesc.ISO_9660.dwPathTblSize
			, volDesc.ISO_9660.dwPathTblPos, pDirRec, &nDirPosNum)) {
			FreeAndNull(pDirRec);
			return FALSE;
		}
		if (!ReadDirectoryRecord(uid, buf
			, volDesc.ISO_9660.dwLogicalBlkCoef, volDesc.ISO_9660.dwRootDataLen, pDirRec, nDirPosNum)) {
			FreeAndNull(pDirRec);
			return FALSE;
		}
		FreeAndNull(pDirRec);
	}

	return TRUE;
}

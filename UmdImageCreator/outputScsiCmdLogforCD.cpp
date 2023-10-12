/**
 * Copyright 2018-2023 sarami
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
#include "get.h"
#include "output.h"
#include "outputScsiCmdLogforCD.h"
#include <string.h>
#include <pspdebug.h>
#include <ctype.h>

VOID OutputFsBootRecord(
	LPBYTE lpBuf
) {
	CHAR str[2][32] = { "0" };
	strncpy(str[0], (LPCH)&lpBuf[7], sizeof(str[0]));
	strncpy(str[1], (LPCH)&lpBuf[39], sizeof(str[1]));
	OutputVolDescLog(
		"\t                       Boot System Identifier: %s\n"
		"\t                              Boot Identifier: %s\n"
		"\t                              Boot System Use: "
		, str[0], str[1]);
	for (INT i = 71; i < 2048; i++) {
		OutputVolDescLog("%x", lpBuf[i]);
	}
	OutputVolDescLog("\n");
}

VOID OutputFsVolumeDescriptorFirst(
	LPBYTE lpBuf,
	CHAR str32[][32]
) {
	DWORD vss = GetSizeOrUintForVolDesc(lpBuf + 80, DWORD(0xa0000 * DISC_MAIN_DATA_SIZE));
	OutputVolDescLog(
		"\t                            System Identifier: %.32s\n"
		"\t                            Volume Identifier: %.32s\n"
		"\t                            Volume Space Size: %lu\n"
		, str32[0], str32[1], vss);
}

VOID OutputFsDirectoryRecord(
	LPBYTE lpBuf,
	UINT uiExtentPos,
	UINT uiDataLen,
	LPSTR fname,
	PPATH_TABLE_RECORD pPathTblRec,
	UINT uiPathTblIdx
) {
	if (!fname) {
		return;
	}
	CHAR str[128] = { 0 };
	INT nFileFlag = lpBuf[25];
	if (nFileFlag & 0x01) {
		strncat(str, "Invisible, ", 11);
	}
	else {
		strncat(str, "Visible, ", 9);
	}
	if (nFileFlag & 0x02) {
		strncat(str, "Directory, ", 11);
	}
	else {
		strncat(str, "File, ", 6);
	}
	if (nFileFlag & 0x04) {
		strncat(str, "Associated, ", 12);
	}
	else {
		strncat(str, "No Associated, ", 15);
	}
	if (nFileFlag & 0x08) {
		strncat(str, "Record Format, ", 15);
	}
	else {
		strncat(str, "No Record Format, ", 18);
	}
	if (nFileFlag & 0x10) {
		strncat(str, "Owner/Group ID, ", 16);
	}
	else {
		strncat(str, "No Owner/Group ID, ", 19);
	}
	if (nFileFlag & 0x80) {
		strncat(str, "No Final Directory Record", 25);
	}
	else {
		strncat(str, "Final Directory Record", 22);
	}
	WORD vsn = GetSizeOrWordForVolDesc(lpBuf + 28);
	OutputVolDescLog(
		"\t\t      Length of Directory Record: %u\n"
		"\t\tExtended Attribute Record Length: %u\n"
		"\t\t              Location of Extent: %u\n"
		"\t\t                     Data Length: %u\n"
		"\t\t         Recording Date and Time: %d-%02u-%02uT%02u:%02u:%02u%+03d:%02d\n"
		"\t\t                      File Flags: %u (%s)\n"
		"\t\t                  File Unit Size: %u\n"
		"\t\t             Interleave Gap Size: %u\n"
		"\t\t          Volume Sequence Number: %u\n"
		"\t\t       Length of File Identifier: %u\n"
		"\t\t                 File Identifier: "
		, lpBuf[0], lpBuf[1], uiExtentPos, uiDataLen, lpBuf[18] + 1900, lpBuf[19], lpBuf[20]
		, lpBuf[21], lpBuf[22], lpBuf[23], (CHAR)lpBuf[24] / 4, (CHAR)lpBuf[24] % 4 * 15
		, lpBuf[25], str, lpBuf[26], lpBuf[27], vsn, lpBuf[32]
	);

	BOOL bSkip = FALSE;
	for (INT n = 0; n < lpBuf[32]; n++) {
		OutputVolDescLog("%c", lpBuf[33 + n]);
		if (lpBuf[33 + n] == ';' && n == lpBuf[32] - 2) {
			// skip "file revision number"
			bSkip = TRUE;
		}
		if (!bSkip) {
			fname[n] = (CHAR)lpBuf[33 + n];
		}
	}
	OutputVolDescLog("\n");

	CHAR strTmpFull[FILENAME_MAX] = {};
	// not upper and current directory
	if (pPathTblRec &&
		!(lpBuf[32] == 1 && fname[0] == 0) &&
		!(lpBuf[32] == 1 && fname[0] == 1)) {
		LPCH pName[FILENAME_MAX] = {};
		INT fullIdx = 0;
		pName[fullIdx++] = fname;

		for (UINT idx = uiPathTblIdx; idx != 0;) {
			pName[fullIdx++] = &pPathTblRec[idx].szDirName[0];
			idx = pPathTblRec[idx].uiNumOfUpperDir - 1;
		}
		OutputVolDescLog("FullPath: ");
		CHAR strTmp[FILENAME_MAX] = {};
		for (INT i = fullIdx; 0 < i; i--) {
			if (pName[i - 1] != 0) {
				snprintf(strTmp, sizeof(strTmp), "/%s", pName[i - 1]);
				OutputVolDescLog("%s", strTmp);
				if (strlen(strTmpFull) + strlen(strTmp) + 1 <= sizeof(strTmpFull)) {
					strcat(strTmpFull, strTmp);
				}
			}
		}
		OutputVolDescLog("\n\n");
	}
}

VOID OutputFsVolumeDescriptorSecond(
	LPBYTE lpBuf,
	CHAR str128[][128],
	CHAR str37[][37],
	BOOL bTCHAR
) {
	WORD vss = GetSizeOrWordForVolDesc(lpBuf + 120);
	WORD vsn = GetSizeOrWordForVolDesc(lpBuf + 124);
	WORD lbs = GetSizeOrWordForVolDesc(lpBuf + 128);
	UINT pts = GetSizeOrUintForVolDesc(lpBuf + 132, UINT(0xa0000 * DISC_MAIN_DATA_SIZE));
	UINT lopt = MAKEUINT(MAKEWORD(lpBuf[140], lpBuf[141]),
		MAKEWORD(lpBuf[142], lpBuf[143]));
//	pDisc->MAIN.bPathType = lType;
	if (lopt == 0) {
		lopt = MAKEUINT(MAKEWORD(lpBuf[151], lpBuf[150]),
			MAKEWORD(lpBuf[149], lpBuf[148]));
//		pDisc->MAIN.bPathType = mType;
	}

	UINT loopt = MAKEUINT(MAKEWORD(lpBuf[144], lpBuf[145]),
		MAKEWORD(lpBuf[146], lpBuf[147]));
	if (loopt == 0) {
		loopt = MAKEUINT(MAKEWORD(lpBuf[155], lpBuf[154]),
			MAKEWORD(lpBuf[153], lpBuf[152]));
	}
	OutputVolDescLog(
		"\t                              Volume Set Size: %u\n"
		"\t                       Volume Sequence Number: %u\n"
		"\t                           Logical Block Size: %u\n"
		"\t                              Path Table Size: %u\n"
		"\t         Location of Occurrence of Path Table: %u\n"
		"\tLocation of Optional Occurrence of Path Table: %u\n"
		, vss, vsn, lbs, pts, lopt, loopt);

	UINT uiExtentPos = GetSizeOrUintForVolDesc(lpBuf + 158, UINT(0xa0000 * DISC_MAIN_DATA_SIZE));
	UINT uiDataLen = GetSizeOrUintForVolDesc(lpBuf + 166, UINT(0xa0000 * DISC_MAIN_DATA_SIZE));
	CHAR fname[64] = {};
	OutputFsDirectoryRecord(lpBuf + 156, uiExtentPos, uiDataLen, fname, NULL, 0);
	if (bTCHAR) {
		OutputVolDescLog(
			"\t                        Volume Set Identifier: %.64s\n"
			"\t                         Publisher Identifier: %.64s\n"
			"\t                     Data Preparer Identifier: %.64s\n"
			"\t                       Application Identifier: %.64s\n"
			"\t                    Copyright File Identifier: %.18s\n"
			"\t                     Abstract File Identifier: %.18s\n"
			"\t                Bibliographic File Identifier: %.18s\n"
			, str128[0], str128[1], str128[2], str128[3], str37[0], str37[1], str37[2]);
	}
	else {
		OutputVolDescLog(
			"\t                        Volume Set Identifier: %.128s\n"
			"\t                         Publisher Identifier: %.128s\n"
			"\t                     Data Preparer Identifier: %.128s\n"
			"\t                       Application Identifier: %.128s\n"
			"\t                    Copyright File Identifier: %.37s\n"
			"\t                     Abstract File Identifier: %.37s\n"
			"\t                Bibliographic File Identifier: %.37s\n"
			, str128[0], str128[1], str128[2], str128[3], str37[0], str37[1], str37[2]);
	}
	OutputVolDescLog(
		"\t                Volume Creation Date and Time: %.4s-%.2s-%.2sT%.2s:%.2s:%.2s.%.2s%+03d:%02d\n"
		"\t            Volume Modification Date and Time: %.4s-%.2s-%.2sT%.2s:%.2s:%.2s.%.2s%+03d:%02d\n"
		"\t              Volume Expiration Date and Time: %.4s-%.2s-%.2sT%.2s:%.2s:%.2s.%.2s%+03d:%02d\n"
		"\t               Volume Effective Date and Time: %.4s-%.2s-%.2sT%.2s:%.2s:%.2s.%.2s%+03d:%02d\n"
		"\t                       File Structure Version: %u\n"
		"\t                              Application Use: ",
		&lpBuf[813], &lpBuf[817], &lpBuf[819], &lpBuf[821], &lpBuf[823]
		, &lpBuf[825], &lpBuf[827], (CHAR)lpBuf[829] / 4, abs((CHAR)lpBuf[829] % 4 * 15),
		&lpBuf[830], &lpBuf[834], &lpBuf[836], &lpBuf[838], &lpBuf[840]
		, &lpBuf[842], &lpBuf[844], (CHAR)lpBuf[846] / 4, abs((CHAR)lpBuf[846] % 4 * 15),
		&lpBuf[847], &lpBuf[851], &lpBuf[853], &lpBuf[855], &lpBuf[857]
		, &lpBuf[859], &lpBuf[861], (CHAR)lpBuf[863] / 4, abs((CHAR)lpBuf[863] % 4 * 15),
		&lpBuf[864], &lpBuf[868], &lpBuf[870], &lpBuf[872], &lpBuf[874]
		, &lpBuf[876], &lpBuf[878], (CHAR)lpBuf[880] / 4, abs((CHAR)lpBuf[880] % 4 * 15),
		lpBuf[881]);
	for (INT i = 883; i <= 1394; i++) {
		OutputVolDescLog("%x", lpBuf[i]);
	}
	OutputVolDescLog("\n");
}

VOID OutputFsVolumeDescriptorForISO9660(
	LPBYTE lpBuf
) {
	CHAR str32[3][32] = { { 0 } };
	CHAR str128[4][128] = { { 0 } };
	CHAR str37[3][37] = { { 0 } };
	strncpy(str32[0], (LPCH)&lpBuf[8], sizeof(str32[0]));
	strncpy(str32[1], (LPCH)&lpBuf[40], sizeof(str32[1]));
	strncpy(str128[0], (LPCH)&lpBuf[190], sizeof(str128[0]));
	strncpy(str128[1], (LPCH)&lpBuf[318], sizeof(str128[1]));
	strncpy(str128[2], (LPCH)&lpBuf[446], sizeof(str128[2]));
	strncpy(str128[3], (LPCH)&lpBuf[574], sizeof(str128[3]));
	strncpy(str37[0], (LPCH)&lpBuf[702], sizeof(str37[0]));
	strncpy(str37[1], (LPCH)&lpBuf[739], sizeof(str37[1]));
	strncpy(str37[2], (LPCH)&lpBuf[776], sizeof(str37[2]));
	OutputFsVolumeDescriptorFirst(lpBuf, str32);
	if (lpBuf[0] == 2) {
		strncpy(str32[2], (LPCH)&lpBuf[88], sizeof(str32[2]));
		OutputVolDescLog(
			"\t                             Escape Sequences: %.32s\n", str32[2]);
	}
	OutputFsVolumeDescriptorSecond(lpBuf, str128, str37, FALSE);
}
#if 0
VOID OutputFsVolumeDescriptorForJoliet(
	PEXT_ARG pExtArg,
	PDISC pDisc,
	LPBYTE lpBuf
) {
	CHAR str32[3][32] = { 0 };
	CHAR str128[4][128] = { 0 };
	CHAR str37[3][37] = { 0 };
	WCHAR tmp16[3][16] = { 0 };
	WCHAR tmp64[4][64] = { 0 };
	WCHAR tmp18[3][18] = { 0 };
	BOOL bTCHAR = FALSE;
	if (lpBuf[8] == 0 && lpBuf[9] >= 0x20) {
		LittleToBig(tmp16[0], (LPWCH)&lpBuf[8], 16);
		bTCHAR = TRUE;
	}
	else if (lpBuf[8] >= 0x20 && lpBuf[9] == 0) {
		wcsncpy(tmp16[0], (LPWCH)&lpBuf[8], sizeof(tmp16[0]) / sizeof(tmp16[0][0]));
		bTCHAR = TRUE;
	}
	if (bTCHAR) {
		if (!WideCharToMultiByte(CP_ACP, 0,
			(LPCWSTR)&tmp16[0], 16, str32[0], sizeof(str32[0]), NULL, NULL)) {
			OutputLastErrorNumAndString(_T(__FUNCTION__), __LINE__);
		}
	}
	else {
		strncpy(str32[0], (LPCH)&lpBuf[8], sizeof(str32[0]));
	}

	if (lpBuf[40] == 0 && lpBuf[41] >= 0x20) {
		LittleToBig(tmp16[1], (LPWCH)&lpBuf[40], 16);
	}
	else if (lpBuf[40] >= 0x20 && lpBuf[41] == 0) {
		wcsncpy(tmp16[1], (LPWCH)&lpBuf[40], sizeof(tmp16[1]) / sizeof(tmp16[1][0]));
	}
	if (!WideCharToMultiByte(CP_ACP, 0,
		(LPCWSTR)&tmp16[1], 16, str32[1], sizeof(str32[1]), NULL, NULL)) {
		OutputLastErrorNumAndString(_T(__FUNCTION__), __LINE__);
	}

	if (lpBuf[190] == 0 && lpBuf[191] >= 0x20) {
		LittleToBig(tmp64[0], (LPWCH)&lpBuf[190], 64);
	}
	else if (lpBuf[190] >= 0x20 && lpBuf[191] == 0) {
		wcsncpy(tmp64[0], (LPWCH)&lpBuf[190], sizeof(tmp64[0]) / sizeof(tmp64[0][0]));
	}
	if (!WideCharToMultiByte(CP_ACP, 0,
		(LPCWSTR)&tmp64[0], 64, str128[0], sizeof(str128[0]), NULL, NULL)) {
		OutputLastErrorNumAndString(_T(__FUNCTION__), __LINE__);
	}

	if (lpBuf[318] == 0 && lpBuf[319] >= 0x20) {
		LittleToBig(tmp64[1], (LPWCH)&lpBuf[318], 64);
	}
	else if (lpBuf[318] >= 0x20 && lpBuf[319] == 0) {
		wcsncpy(tmp64[1], (LPWCH)&lpBuf[318], sizeof(tmp64[1]) / sizeof(tmp64[1][0]));
	}
	if (!WideCharToMultiByte(CP_ACP, 0,
		(LPCWSTR)&tmp64[1], 64, str128[1], sizeof(str128[1]), NULL, NULL)) {
		OutputLastErrorNumAndString(_T(__FUNCTION__), __LINE__);
	}

	if (lpBuf[446] == 0 && lpBuf[447] >= 0x20) {
		LittleToBig(tmp64[2], (LPWCH)&lpBuf[446], 64);
	}
	else if (lpBuf[446] >= 0x20 && lpBuf[447] == 0) {
		wcsncpy(tmp64[2], (LPWCH)&lpBuf[446], sizeof(tmp64[2]) / sizeof(tmp64[2][0]));
	}
	if (!WideCharToMultiByte(CP_ACP, 0,
		(LPCWSTR)&tmp64[2], 64, str128[2], sizeof(str128[2]), NULL, NULL)) {
		OutputLastErrorNumAndString(_T(__FUNCTION__), __LINE__);
	}

	if (lpBuf[574] == 0 && lpBuf[575] >= 0x20) {
		LittleToBig(tmp64[3], (LPWCH)&lpBuf[574], 64);
	}
	else if (lpBuf[574] >= 0x20 && lpBuf[575] == 0) {
		wcsncpy(tmp64[3], (LPWCH)&lpBuf[574], sizeof(tmp64[3]) / sizeof(tmp64[3][0]));
	}
	if (!WideCharToMultiByte(CP_ACP, 0,
		(LPCWSTR)&tmp64[3], 64, str128[3], sizeof(str128[3]), NULL, NULL)) {
		OutputLastErrorNumAndString(_T(__FUNCTION__), __LINE__);
	}

	if (lpBuf[702] == 0 && lpBuf[703] >= 0x20) {
		LittleToBig(tmp18[0], (LPWCH)&lpBuf[702], 18);
	}
	else if (lpBuf[702] >= 0x20 && lpBuf[703] == 0) {
		wcsncpy(tmp18[0], (LPWCH)&lpBuf[702], sizeof(tmp18[0]) / sizeof(tmp18[0][0]));
	}
	if (!WideCharToMultiByte(CP_ACP, 0,
		(LPCWSTR)&tmp18[0], 18, str37[0], sizeof(str37[0]) - 1, NULL, NULL)) {
		OutputLastErrorNumAndString(_T(__FUNCTION__), __LINE__);
	}

	if (lpBuf[739] == 0 && lpBuf[740] >= 0x20) {
		LittleToBig(tmp18[1], (LPWCH)&lpBuf[739], 18);
	}
	else if (lpBuf[739] >= 0x20 && lpBuf[740] == 0) {
		wcsncpy(tmp18[1], (LPWCH)&lpBuf[739], sizeof(tmp18[1]) / sizeof(tmp18[1][0]));
	}
	if (!WideCharToMultiByte(CP_ACP, 0,
		(LPCWSTR)&tmp18[1], 18, str37[1], sizeof(str37[1]) - 1, NULL, NULL)) {
		OutputLastErrorNumAndString(_T(__FUNCTION__), __LINE__);
	}

	if (lpBuf[776] == 0 && lpBuf[777] >= 0x20) {
		LittleToBig(tmp18[2], (LPWCH)&lpBuf[776], 18);
	}
	else if (lpBuf[776] >= 0x20 && lpBuf[777] == 0) {
		wcsncpy(tmp18[2], (LPWCH)&lpBuf[776], sizeof(tmp18[2]) / sizeof(tmp18[2][0]));
	}
	if (!WideCharToMultiByte(CP_ACP, 0,
		(LPCWSTR)&tmp18[2], 18, str37[2], sizeof(str37[2]) - 1, NULL, NULL)) {
		OutputLastErrorNumAndString(_T(__FUNCTION__), __LINE__);
	}

	OutputFsVolumeDescriptorFirst(pDisc, lpBuf, str32);

	strncpy(str32[2], (LPCH)&lpBuf[88], sizeof(str32[2]));
	OutputVolDescLog(
		"\t                             Escape Sequences: %.32s\n", str32[2]);
	OutputFsVolumeDescriptorSecond(pExtArg, pDisc, lpBuf, str128, str37, TRUE);
}
#endif
VOID OutputFsVolumePartitionDescriptor(
	LPBYTE lpBuf
) {
	CHAR str[2][32] = { { 0 } };
	strncpy(str[0], (LPCH)&lpBuf[8], sizeof(str[0]));
	strncpy(str[1], (LPCH)&lpBuf[40], sizeof(str[1]));
	OutputVolDescLog(
		"\t          System Identifier: %.32s\n"
		"\tVolume Partition Identifier: %.32s\n"
		"\t  Volume Partition Location: %u\n"
		"\t      Volume Partition Size: %u\n"
		"\t                 System Use: ",
		str[0],
		str[1],
		MAKEUINT(MAKEWORD(lpBuf[76], lpBuf[77]), MAKEWORD(lpBuf[78], lpBuf[79])),
		MAKEUINT(MAKEWORD(lpBuf[84], lpBuf[85]), MAKEWORD(lpBuf[86], lpBuf[87])));
	for (INT i = 88; i < 2048; i++) {
		OutputVolDescLog("%x", lpBuf[i]);
	}
	OutputVolDescLog("\n");
}

VOID OutputFsVolumeDescriptor(
	LPBYTE lpBuf,
	INT nLBA
) {
	// 0 is Boot Record.
	// 1 is Primary Volume Descriptor.
	// 2 is Supplementary Volume Descriptor.
	// 3 is Volume Partition Descriptor.
	// 4-254 is reserved.
	// 255 is Volume Descriptor Set Terminator.
	OutputVolDescWithLBALog2("Volume Descriptor",
		"\t                       Volume Descriptor Type: %u\n"
		"\t                          Standard Identifier: %.5s\n"
		"\t                    Volume Descriptor Version: %u\n"
		, nLBA, lpBuf[0], (LPCH)&lpBuf[1], lpBuf[6]);

	if (lpBuf[0] == 0) {
		OutputFsBootRecord(lpBuf);
	}
	else if (lpBuf[0] == 1) {
		OutputFsVolumeDescriptorForISO9660(lpBuf);
	}
	else if (lpBuf[0] == 2) {
//		OutputFsVolumeDescriptorForJoliet(pExtArg, pDisc, lpBuf);
	}
	else if (lpBuf[0] == 3) {
		OutputFsVolumePartitionDescriptor(lpBuf);
	}
}

BOOL OutputFsPathTableRecord(
	LPBYTE lpBuf,
	UINT uiLogicalBlkCoef,
	UINT uiPathTblPos,
	UINT uiPathTblSize,
	PPATH_TABLE_RECORD pPathTblRec,
	LPUINT uiDirPosNum
) {
	OutputVolDescLog(
		OUTPUT_DHYPHEN_PLUS_STR_WITH_LBA_F("Path Table Record"), (INT)uiPathTblPos, (INT)uiPathTblPos);
	for (UINT i = 0; i < uiPathTblSize;) {
		if (*uiDirPosNum > PATH_TABLE_RECORD_SIZE) {
			pspPrintf("Directory Record is over %d\n", PATH_TABLE_RECORD_SIZE);
			FlushLog();
			return FALSE;
		}
		pPathTblRec[*uiDirPosNum].uiDirNameLen = lpBuf[i];
		if (0 == lType) {
			pPathTblRec[*uiDirPosNum].uiPosOfDir = MAKEUINT(MAKEWORD(lpBuf[2 + i], lpBuf[3 + i]),
				MAKEWORD(lpBuf[4 + i], lpBuf[5 + i])) / uiLogicalBlkCoef;
		}
		else {
			pPathTblRec[*uiDirPosNum].uiPosOfDir = MAKEUINT(MAKEWORD(lpBuf[5 + i], lpBuf[4 + i]),
				MAKEWORD(lpBuf[3 + i], lpBuf[2 + i])) / uiLogicalBlkCoef;
		}
		if (pPathTblRec[*uiDirPosNum].uiDirNameLen > 0) {
			if (0 == lType) {
				pPathTblRec[*uiDirPosNum].uiNumOfUpperDir = MAKEWORD(lpBuf[6 + i], lpBuf[7 + i]);
			}
			else {
				pPathTblRec[*uiDirPosNum].uiNumOfUpperDir = MAKEWORD(lpBuf[7 + i], lpBuf[6 + i]);
			}
			OutputVolDescLog(
				"\t     Length of Directory Identifier: %u\n"
				"\tLength of Extended Attribute Record: %u\n"
				"\t                 Position of Extent: %u\n"
				"\t          Number of Upper Directory: %u\n"
				"\t               Directory Identifier: "
				, pPathTblRec[*uiDirPosNum].uiDirNameLen, lpBuf[1 + i]
				, pPathTblRec[*uiDirPosNum].uiPosOfDir, pPathTblRec[*uiDirPosNum].uiNumOfUpperDir);
			for (size_t n = 0; n < pPathTblRec[*uiDirPosNum].uiDirNameLen; n++) {
				if (lpBuf[8 + i + n] == 0) continue;
				OutputVolDescLog("%c", lpBuf[8 + i + n]);
				pPathTblRec[*uiDirPosNum].szDirName[n] = (CHAR)lpBuf[8 + i + n];
			}
			OutputVolDescLog("\n\n");

			i += 8 + pPathTblRec[*uiDirPosNum].uiDirNameLen;
			if ((i % 2) != 0) {
				i++;
			}
			*uiDirPosNum = *uiDirPosNum + 1;
		}
		else {
			OutputVolDescLog(
				"\t     Length of Directory Identifier: %u\n", pPathTblRec[*uiDirPosNum].uiDirNameLen);
			break;
		}
	}
	return TRUE;
}

VOID OutputMainChannel(
	LOG_TYPE type,
	LPBYTE lpBuf,
	INT nLBA,
	INT nSize
) {
#ifdef _DEBUG
	UNREFERENCED_PARAMETER(type);
#endif
	OutputLog(type, OUTPUT_DHYPHEN_PLUS_STR_WITH_LBA_F("Main Channel")
		"       +0 +1 +2 +3 +4 +5 +6 +7  +8 +9 +A +B +C +D +E +F\n", nLBA, nLBA);

	for (INT i = 0; i < nSize; i += 16) {
		OutputLog(type,
			"%04X : %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X   "
			, i, lpBuf[i], lpBuf[i + 1], lpBuf[i + 2], lpBuf[i + 3], lpBuf[i + 4], lpBuf[i + 5]
			, lpBuf[i + 6], lpBuf[i + 7], lpBuf[i + 8], lpBuf[i + 9], lpBuf[i + 10], lpBuf[i + 11]
			, lpBuf[i + 12], lpBuf[i + 13], lpBuf[i + 14], lpBuf[i + 15]);
		for (INT j = 0; j < 16; j++) {
			INT ch = isprint(lpBuf[i + j]) ? lpBuf[i + j] : '.';
			OutputLog(type, "%c", ch);
		}
		OutputLog(type, "\n");
	}
}

VOID OutputInquiry(
	PINQUIRYDATA pInquiry
) {
	OutputDriveLog(
		OUTPUT_DHYPHEN_PLUS_STR("InquiryData")
		"\t          DeviceType: ");
	switch (pInquiry->DeviceType) {
	case DIRECT_ACCESS_DEVICE:
		OutputDriveLog("DirectAccessDevice (Floppy etc)\n");
		break;
	case READ_ONLY_DIRECT_ACCESS_DEVICE:
		OutputDriveLog("ReadOnlyDirectAccessDevice (CD/DVD etc)\n");
		break;
	case OPTICAL_DEVICE:
		OutputDriveLog("OpticalDisk\n");
		break;
	default:
		OutputDriveLog("OtherDevice\n");
		break;
	}
	OutputDriveLog(
		"\t DeviceTypeQualifier: ");
	switch (pInquiry->DeviceTypeQualifier) {
	case DEVICE_QUALIFIER_ACTIVE:
		OutputDriveLog("Active\n");
		break;
	case DEVICE_QUALIFIER_NOT_ACTIVE:
		OutputDriveLog("NotActive\n");
		break;
	case DEVICE_QUALIFIER_NOT_SUPPORTED:
		OutputDriveLog("NotSupported\n");
		break;
	default:
		OutputDriveLog("\n");
		break;
	}

	OutputDriveLog(
		"\t  DeviceTypeModifier: %u\n"
		"\t      RemovableMedia: %s\n"
		"\t            Versions: %u\n"
		"\t  ResponseDataFormat: %u\n"
		"\t           HiSupport: %s\n"
		"\t             NormACA: %s\n"
		"\t       TerminateTask: %s\n"
		"\t                AERC: %s\n"
		"\t    AdditionalLength: %u\n"
		"\t       MediumChanger: %s\n"
		"\t           MultiPort: %s\n"
		"\t   EnclosureServices: %s\n"
		"\t           SoftReset: %s\n"
		"\t        CommandQueue: %s\n"
		"\t      LinkedCommands: %s\n"
		"\t  RelativeAddressing: %s\n",
		pInquiry->DeviceTypeModifier,
		BOOLEAN_TO_STRING_YES_NO(pInquiry->RemovableMedia),
		pInquiry->Versions,
		pInquiry->ResponseDataFormat,
		BOOLEAN_TO_STRING_YES_NO(pInquiry->HiSupport),
		BOOLEAN_TO_STRING_YES_NO(pInquiry->NormACA),
		BOOLEAN_TO_STRING_YES_NO(pInquiry->TerminateTask),
		BOOLEAN_TO_STRING_YES_NO(pInquiry->AERC),
		pInquiry->AdditionalLength,
		BOOLEAN_TO_STRING_YES_NO(pInquiry->MediumChanger),
		BOOLEAN_TO_STRING_YES_NO(pInquiry->MultiPort),
		BOOLEAN_TO_STRING_YES_NO(pInquiry->EnclosureServices),
		BOOLEAN_TO_STRING_YES_NO(pInquiry->SoftReset),
		BOOLEAN_TO_STRING_YES_NO(pInquiry->CommandQueue),
		BOOLEAN_TO_STRING_YES_NO(pInquiry->LinkedCommands),
		BOOLEAN_TO_STRING_YES_NO(pInquiry->RelativeAddressing));

	OutputDriveLog(
		"\t            VendorId: %.8s\n"
		"\t           ProductId: %.16s\n"
		"\tProductRevisionLevel: %.4s\n"
		"\t      VendorSpecific: %.20s\n"
		, pInquiry->VendorId
		, pInquiry->ProductId
		, pInquiry->ProductRevisionLevel
		, pInquiry->VendorSpecific);
}

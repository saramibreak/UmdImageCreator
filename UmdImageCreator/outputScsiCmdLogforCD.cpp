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
	OutputVolDescLogA(
		"\t                       Boot System Identifier: %s\n"
		"\t                              Boot Identifier: %s\n"
		"\t                              Boot System Use: "
		, str[0], str[1]);
	for (INT i = 71; i < 2048; i++) {
		OutputVolDescLogA("%x", lpBuf[i]);
	}
	OutputVolDescLogA("\n");
}

VOID OutputFsVolumeDescriptorFirst(
	LPBYTE lpBuf,
	CHAR str32[][32]
) {
	DWORD vss = GetSizeOrDwordForVolDesc(lpBuf + 80, DWORD(0xa0000 * DISC_RAW_READ_SIZE));
	OutputVolDescLogA(
		"\t                            System Identifier: %.32s\n"
		"\t                            Volume Identifier: %.32s\n"
		"\t                            Volume Space Size: %lu\n"
		, str32[0], str32[1], vss);
}

VOID OutputFsDirectoryRecord(
	LPBYTE lpBuf,
	DWORD dwExtentPos,
	DWORD dwDataLen,
	LPSTR fname
) {
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
		strncat(str, "Disassociated, ", 15);
	}
	if (nFileFlag & 0x08) {
		strncat(str, "File has record format, ", 24);
	}
	else {
		strncat(str, "File has't record format, ", 26);
	}
	if (nFileFlag & 0x10) {
		strncat(str, "Owner/Group ID has, ", 20);
	}
	else {
		strncat(str, "Owner/Group ID has't, ", 22);
	}
	if (nFileFlag & 0x80) {
		strncat(str, "Next Directory Record has", 25);
	}
	else {
		strncat(str, "Final Directory Record", 22);
	}
	WORD vsn = GetSizeOrWordForVolDesc(lpBuf + 28);
	OutputVolDescLogA(
		"\t\t      Length of Directory Record: %u\n"
		"\t\tExtended Attribute Record Length: %u\n"
		"\t\t              Location of Extent: %lu\n"
		"\t\t                     Data Length: %lu\n"
		"\t\t         Recording Date and Time: %u-%02u-%02u %02u:%02u:%02u %+03d:%02u\n"
		"\t\t                      File Flags: %u (%s)\n"
		"\t\t                  File Unit Size: %u\n"
		"\t\t             Interleave Gap Size: %u\n"
		"\t\t          Volume Sequence Number: %u\n"
		"\t\t       Length of File Identifier: %u\n"
		"\t\t                 File Identifier: "
		, lpBuf[0], lpBuf[1], dwExtentPos, dwDataLen, lpBuf[18] + 1900, lpBuf[19], lpBuf[20]
		, lpBuf[21], lpBuf[22], lpBuf[23], (CHAR)lpBuf[24] / 4, (CHAR)lpBuf[24] % 4 * 15
		, lpBuf[25], str, lpBuf[26], lpBuf[27], vsn, lpBuf[32]);
	for (INT n = 0; n < lpBuf[32]; n++) {
		OutputVolDescLogA("%c", lpBuf[33 + n]);
		fname[n] = (CHAR)lpBuf[33 + n];
	}
	OutputVolDescLogA("\n");
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
	DWORD pts = GetSizeOrDwordForVolDesc(lpBuf + 132, DWORD(0xa0000 * DISC_RAW_READ_SIZE));
	DWORD lopt = MAKEDWORD(MAKEWORD(lpBuf[140], lpBuf[141]),
		MAKEWORD(lpBuf[142], lpBuf[143]));
//	pDisc->MAIN.bPathType = lType;
	if (lopt == 0) {
		lopt = MAKEDWORD(MAKEWORD(lpBuf[151], lpBuf[150]),
			MAKEWORD(lpBuf[149], lpBuf[148]));
//		pDisc->MAIN.bPathType = mType;
	}

	DWORD loopt = MAKEDWORD(MAKEWORD(lpBuf[144], lpBuf[145]),
		MAKEWORD(lpBuf[146], lpBuf[147]));
	if (loopt == 0) {
		loopt = MAKEDWORD(MAKEWORD(lpBuf[155], lpBuf[154]),
			MAKEWORD(lpBuf[153], lpBuf[152]));
	}
	OutputVolDescLogA(
		"\t                              Volume Set Size: %u\n"
		"\t                       Volume Sequence Number: %u\n"
		"\t                           Logical Block Size: %u\n"
		"\t                              Path Table Size: %lu\n"
		"\t         Location of Occurrence of Path Table: %lu\n"
		"\tLocation of Optional Occurrence of Path Table: %lu\n"
		, vss, vsn, lbs, pts, lopt, loopt);

	DWORD dwExtentPos = GetSizeOrDwordForVolDesc(lpBuf + 158, DWORD(0xa0000 * DISC_RAW_READ_SIZE));
	DWORD dwDataLen = GetSizeOrDwordForVolDesc(lpBuf + 166, DWORD(0xa0000 * DISC_RAW_READ_SIZE));
	CHAR fname[64] = { 0 };
	OutputFsDirectoryRecord(lpBuf + 156, dwExtentPos, dwDataLen, fname);
	if (bTCHAR) {
		OutputVolDescLogA(
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
		OutputVolDescLogA(
			"\t                        Volume Set Identifier: %.128s\n"
			"\t                         Publisher Identifier: %.128s\n"
			"\t                     Data Preparer Identifier: %.128s\n"
			"\t                       Application Identifier: %.128s\n"
			"\t                    Copyright File Identifier: %.37s\n"
			"\t                     Abstract File Identifier: %.37s\n"
			"\t                Bibliographic File Identifier: %.37s\n"
			, str128[0], str128[1], str128[2], str128[3], str37[0], str37[1], str37[2]);
	}
}

VOID OutputFsVolumeDescriptorForTime(
	LPBYTE lpBuf
) {
	OutputVolDescLogA(
		"\t                Volume Creation Date and Time: %.4s-%.2s-%.2s %.2s:%.2s:%.2s.%.2s %+03d:%02d\n"
		"\t            Volume Modification Date and Time: %.4s-%.2s-%.2s %.2s:%.2s:%.2s.%.2s %+03d:%02d\n"
		"\t              Volume Expiration Date and Time: %.4s-%.2s-%.2s %.2s:%.2s:%.2s.%.2s %+03d:%02d\n"
		"\t               Volume Effective Date and Time: %.4s-%.2s-%.2s %.2s:%.2s:%.2s.%.2s %+03d:%02d\n"
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
		OutputVolDescLogA("%x", lpBuf[i]);
	}
	OutputVolDescLogA("\n");
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
		OutputVolDescLogA(
			"\t                             Escape Sequences: %.32s\n", str32[2]);
	}
	OutputFsVolumeDescriptorSecond(lpBuf, str128, str37, FALSE);
	OutputFsVolumeDescriptorForTime(lpBuf);
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
	OutputVolDescLogA(
		"\t                             Escape Sequences: %.32s\n", str32[2]);
	OutputFsVolumeDescriptorSecond(pExtArg, pDisc, lpBuf, str128, str37, TRUE);
	OutputFsVolumeDescriptorForTime(lpBuf);
}
#endif
VOID OutputFsVolumePartitionDescriptor(
	LPBYTE lpBuf
) {
	CHAR str[2][32] = { { 0 } };
	strncpy(str[0], (LPCH)&lpBuf[8], sizeof(str[0]));
	strncpy(str[1], (LPCH)&lpBuf[40], sizeof(str[1]));
	OutputVolDescLogA(
		"\t          System Identifier: %.32s\n"
		"\tVolume Partition Identifier: %.32s\n"
		"\t  Volume Partition Location: %lu\n"
		"\t      Volume Partition Size: %lu\n"
		"\t                 System Use: ",
		str[0],
		str[1],
		MAKELONG(MAKEWORD(lpBuf[76], lpBuf[77]), MAKEWORD(lpBuf[78], lpBuf[79])),
		MAKELONG(MAKEWORD(lpBuf[84], lpBuf[85]), MAKEWORD(lpBuf[86], lpBuf[87])));
	for (INT i = 88; i < 2048; i++) {
		OutputVolDescLogA("%x", lpBuf[i]);
	}
	OutputVolDescLogA("\n");
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
	OutputVolDescWithLBALogA(Volume Descriptor,
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
	DWORD dwLogicalBlkCoef,
	DWORD dwPathTblPos,
	DWORD dwPathTblSize,
	PDIRECTORY_RECORD pDirRec,
	LPINT nDirPosNum
) {
	OutputVolDescLogA(
		OUTPUT_DHYPHEN_PLUS_STR_WITH_LBA_F(Path Table Record), (INT)dwPathTblPos, (INT)dwPathTblPos);
	for (DWORD i = 0; i < dwPathTblSize;) {
		if (*nDirPosNum > DIRECTORY_RECORD_SIZE) {
			OutputErrorString(_T("Directory Record is over %d\n"), DIRECTORY_RECORD_SIZE);
			FlushLog();
			return FALSE;
		}
		pDirRec[*nDirPosNum].uiDirNameLen = lpBuf[i];
		if (0 == lType) {
			pDirRec[*nDirPosNum].uiPosOfDir = MAKEDWORD(MAKEWORD(lpBuf[2 + i], lpBuf[3 + i]),
				MAKEWORD(lpBuf[4 + i], lpBuf[5 + i])) / dwLogicalBlkCoef;
		}
		else {
			pDirRec[*nDirPosNum].uiPosOfDir = MAKEDWORD(MAKEWORD(lpBuf[5 + i], lpBuf[4 + i]),
				MAKEWORD(lpBuf[3 + i], lpBuf[2 + i])) / dwLogicalBlkCoef;
		}
		if (pDirRec[*nDirPosNum].uiDirNameLen > 0) {
			if (0 == lType) {
				pDirRec[*nDirPosNum].uiNumOfUpperDir = MAKEWORD(lpBuf[6 + i], lpBuf[7 + i]);
			}
			else {
				pDirRec[*nDirPosNum].uiNumOfUpperDir = MAKEWORD(lpBuf[7 + i], lpBuf[6 + i]);
			}
			OutputVolDescLogA(
				"\t     Length of Directory Identifier: %u\n"
				"\tLength of Extended Attribute Record: %u\n"
				"\t                 Position of Extent: %u\n"
				"\t          Number of Upper Directory: %u\n"
				"\t               Directory Identifier: "
				, pDirRec[*nDirPosNum].uiDirNameLen, lpBuf[1 + i]
				, pDirRec[*nDirPosNum].uiPosOfDir, pDirRec[*nDirPosNum].uiNumOfUpperDir);
			for (size_t n = 0; n < pDirRec[*nDirPosNum].uiDirNameLen; n++) {
				OutputVolDescLogA("%c", lpBuf[8 + i + n]);
				pDirRec[*nDirPosNum].szDirName[n] = (CHAR)lpBuf[8 + i + n];
			}
			OutputVolDescLogA("%c\n\n", ' ');
			i += 8 + pDirRec[*nDirPosNum].uiDirNameLen;
			if ((i % 2) != 0) {
				i++;
			}
			*nDirPosNum = *nDirPosNum + 1;
		}
		else {
			OutputVolDescLogA(
				"\t     Length of Directory Identifier: %u\n", pDirRec[*nDirPosNum].uiDirNameLen);
			break;
		}
	}
	return TRUE;
}

VOID OutputCDMain(
	LOG_TYPE type,
	LPBYTE lpBuf,
	INT nLBA,
	INT nSize
) {
#ifdef _DEBUG
	UNREFERENCED_PARAMETER(type);
#endif
	OutputLogA(type, OUTPUT_DHYPHEN_PLUS_STR_WITH_LBA_F(Main Channel)
		"       +0 +1 +2 +3 +4 +5 +6 +7  +8 +9 +A +B +C +D +E +F\n", nLBA, nLBA);

	for (INT i = 0; i < nSize; i += 16) {
		OutputLogA(type,
			"%04X : %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X   "
			, i, lpBuf[i], lpBuf[i + 1], lpBuf[i + 2], lpBuf[i + 3], lpBuf[i + 4], lpBuf[i + 5]
			, lpBuf[i + 6], lpBuf[i + 7], lpBuf[i + 8], lpBuf[i + 9], lpBuf[i + 10], lpBuf[i + 11]
			, lpBuf[i + 12], lpBuf[i + 13], lpBuf[i + 14], lpBuf[i + 15]);
		for (INT j = 0; j < 16; j++) {
			INT ch = isprint(lpBuf[i + j]) ? lpBuf[i + j] : '.';
			OutputLogA(type, "%c", ch);
		}
		OutputLogA(type, "\n");
	}
}

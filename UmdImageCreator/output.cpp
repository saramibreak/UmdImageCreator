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
#include <pspumd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "output.h"
#include "get.h"
#include "execScsiCmdforFileSystem.h"

VOID OutputLastErrorNumAndString(
	INT errnum,
	LPCTSTR pszFuncName,
	LONG lLineNum
) {
	OutputMainErrorLog("[F:%s][L:%lu] GetLastError: %d, %s\n"
		, pszFuncName, lLineNum, errnum, strerror(errnum));
	pspPrintf("[F:%s][L:%lu] GetLastError: %d, %s\n"
		, pszFuncName, lLineNum, errnum, strerror(errnum));
}

void OutputPspError(const char* string, const char* fname, int value, int errorCode)
{
	int code = errorCode & 0xffff;
	if ((errorCode >> 31) & 0x1) {
		if (value == 0) {
			pspPrintf("%s %s returns 0x%08x error\n -> ", string, fname, errorCode);
		}
		else {
			pspPrintf("%s %s (0x%08x) returns 0x%08x error\n -> ", string, fname, value, errorCode);
		}
		if ((errorCode >> 30) & 0x1) {
			pspPrintf("Critical, ");
		}
		else {
			pspPrintf("Normal, ");
		}
	}
	switch((errorCode >> 16) & 0xfff) {
	case 0x001:
		pspPrintf("File, ");
		switch(code) {
		case 0x0016:
			pspPrintf("Invalid argument\n");
			break;
		case 0x006e:
			pspPrintf("Timeout\n");
			break;
		case 0xb000:
			pspPrintf("Not support\n");
			break;
		default:
			pspPrintf("Other\n");
			break;
		}
		break;
	case 0x002:
		pspPrintf("Kernel, ");
		switch (code) {
		case 0x00d1:
			pspPrintf("Illegal permission\n");
			break;
		case 0x0149:
			pspPrintf("Illegal permission API Call\n");
			break;
		case 0x01a8:
			pspPrintf("Timeout\n");
			break;
		case 0x0321:
			pspPrintf("No device\n");
			break;
		case 0x0325:
			pspPrintf("Not support\n");
			break;
		default:
			pspPrintf("Other\n");
			break;
		}
		break;
	case 0x021:
		pspPrintf("UMD, ");
		switch(code) {
		case 0x0001:
			pspPrintf("Device not ready\n");
			break;
		case 0x0003:
			pspPrintf("No media\n");
			break;
		default:
			pspPrintf("Other\n");
			break;
		}
		break;
	default:
		pspPrintf("Other\n");
		break;
	}
}

int OutputParamSfo(const char* paramsfo)
{
	SceUID uidData = sceIoOpen(paramsfo, PSP_O_RDONLY, 0777);
	if (uidData < 0) {
		OutputPspError("sceIoOpen", paramsfo, 0, uidData);
		return FALSE;
	}
	SceIoStat stat;
	memset(&stat, 0, sizeof(SceIoStat));
	int ret = sceIoGetstat(paramsfo, &stat);
	if (ret < 0) {
		OutputLastErrorNumAndString(errno, __FUNCTION__, __LINE__);
		return FALSE;
	}

	char* data = (char*)malloc(stat.st_size);
	if (data == 0) {
		OutputLastErrorNumAndString(errno, __FUNCTION__, __LINE__);
		return FALSE;
	}
	sceIoRead(uidData, data, stat.st_size);

	ret = sceIoClose(uidData);
	if (ret < 0) {
		OutputPspError("sceIoClose", paramsfo, 0, ret);
		return FALSE;
	}

	// http://www.psdevwiki.com/ps3/PARAM.SFO
	typedef struct _sfo_header
	{
		uint32_t magic; /************ Always PSF */
		uint32_t version; /********** Usually 1.1 */
		uint32_t key_table_start; /** Start offset of key_table */
		uint32_t data_table_start; /* Start offset of data_table */
		uint32_t tables_entries; /*** Number of entries in all tables */
	} sfo_header, *psfo_header;

	typedef struct _sfo_index_table_entry
	{
		uint16_t key_offset; /*** param_key offset (relative to start offset of key_table) */
		uint16_t data_fmt; /***** param_data data type */
		uint32_t data_len; /***** param_data used bytes */
		uint32_t data_max_len; /* param_data total bytes */
		uint32_t data_offset; /** param_data offset (relative to start offset of data_table) */
	} sfo_index_table_entry, *psfo_index_table_entry;

	psfo_header header = (psfo_header)data;
	OutputDiscLog(
		"%s\n"
		"\tmagic: %c%c%c\n"
		"\tversion: %d.%02d\n"
#if 0
		"\tkey_table_start: %d\n"
		"\tdata_table_start: %d\n"
		"\ttables_entries: %d\n\n"
#endif
		, paramsfo, ((header->magic >> 8) & 0x000000ff)
		, ((header->magic >> 16) & 0x000000ff), ((header->magic >> 24) & 0x000000ff)
		, (header->version & 0x000000ff), ((header->version >> 8) & 0x000000ff)
#if 0
		, header->key_table_start, header->data_table_start, header->tables_entries
#endif
	);

	char* keytable = data + header->key_table_start;
	char* datatable = data + header->data_table_start;
	for (uint32_t i = 0; i < header->tables_entries; i++) {
		psfo_index_table_entry entry =
			(psfo_index_table_entry)(data + sizeof(sfo_header) + sizeof(sfo_index_table_entry) * i);
#if 0
		OutputDiscLog(
			"\tkey_offset[%d]: %d\n"
			"\tdata_fmt[%d]: %d\n"
			"\tdata_len[%d]: %d\n"
			"\tdata_max_len[%d]: %d\n"
			"\tdata_offset[%d]: %d\n"
			, i, entry[i]->key_offset, i, entry[i]->data_fmt, i
			, entry[i]->data_len, i, entry[i]->data_max_len, i, entry[i]->data_offset
		);
#endif
		if (entry->data_fmt == 516) {
			OutputDiscLog(
				"\t%s: %s\n", keytable + entry->key_offset, datatable + entry->data_offset);
		}
		else if (entry->data_fmt == 1028) {
			char* ofs = datatable + entry->data_offset;
			long data = MAKELONG(MAKEWORD(ofs[0], ofs[1]), MAKEWORD(ofs[2], ofs[3]));
			OutputDiscLog(
				"\t%s: %ld\n", keytable + entry->key_offset, data);
		}
	}
	FreeAndNull(data);
	return TRUE;
}

int CreateFile(char* id, unsigned int discType, const char* filename, FILE** fp, const char* type)
{
	char dir[16] = "ms0:/iso/";
	if ((discType & PSP_UMD_TYPE_VIDEO) == PSP_UMD_TYPE_VIDEO &&
		(discType & PSP_UMD_TYPE_GAME) != PSP_UMD_TYPE_GAME) {
		strncat(dir, "video/", 6);
	}
	if (sizeof(dir) + strlen(id) + strlen(filename) > 80) {
		pspPrintf("path is too long\n");
		return 0;
	}
	char path[80] = { 0 };
	strncpy(path, dir, sizeof(dir));
	strncat(path, id, strlen(id));
	strncat(path, filename, strlen(filename));

	*fp = fopen(path, type);
	if (*fp == NULL) {
		pspPrintf("Cannot create %s\n", path);
		return 0;
	}
	return  1;
}

void DumpIso(char* id, unsigned int discType, unsigned int discSize, int nDump)
{
	SceUID uid = sceIoOpen(DEVICE_UMD, PSP_O_RDONLY, 0);
	if (uid < 0) {
		OutputPspError("sceIoOpen", DEVICE_UMD, 0, uid);
		return;
	}
	if (!CreateFile(id, discType, "_mainInfo.txt", &g_LogFile.fpMainInfo, "w")) {
		return;
	}
	if (!CreateFile(id, discType, "_mainError.txt", &g_LogFile.fpMainError, "w")) {
		return;
	}
	if (!CreateFile(id, discType, "_volDesc.txt", &g_LogFile.fpVolDesc, "w")) {
		return;
	}

	int nVolumeSpaceSize = 0;
	if (!ReadDVDForFileSystem(uid, &nVolumeSpaceSize)) {
		return;
	}

	if (nDump) {
		MS_INFO info = { 0, 0, 0, 0, 0, 0, 0, 0 };
		if (!GetMSInfo(&info)) {
			return;
		}

#ifndef PRX
		pspPrintf(
			"\nMemoryStick size\n"
			"    Max: %11llu (0x%09llx)\n"
			" - Free: %11llu (0x%09llx)\n"
			" ---------------------------------\n"
			"   Used: %11llu (0x%09llx)\n\n"
			, info.smax, info.smax, info.sfree, info.sfree
			, info.smax - info.sfree, info.smax - info.sfree
		);
#ifdef PRX
		pspDebugScreenSetXY(0, pspDebugScreenGetY() + 5);
#endif
		if (info.sfree < discSize) {
			pspPrintf(
				"   Disc size: %11u (0x%09x)\n"
				" - Free size: %11llu (0x%09llx)\n"
				" ---------------------------------\n"
				"    Shortage: %11llu (0x%09llx)\n\n"
				, discSize, discSize, info.sfree, info.sfree
				, discSize - info.sfree, discSize - info.sfree
			);
#ifdef PRX
			pspDebugScreenSetXY(0, pspDebugScreenGetY() + 4);
#endif
		}
#endif

		FILE* fpIso = NULL;
		if (!CreateFile(id, discType, ".iso", &fpIso, "wb")) {
			return;
		}
#ifdef PRX
		size_t allocSize = DISC_MAIN_DATA_SIZE * 240;
		LPBYTE lpBuf = (LPBYTE)calloc(allocSize, sizeof(BYTE));
		if (!lpBuf) {
			pspPrintf("Failed to calloc\n");
			return;
		}
#else
		size_t allocSize = DISC_MAIN_DATA_SIZE * 128;
		SceUID mem = sceKernelAllocPartitionMemory(2, "Memory1", 0, allocSize, NULL);
		if (mem < 0) {
			pspPrintf("Failed to run sceKernelAllocPartitionMemory\n");
			return;
		}
		LPBYTE lpBuf = (LPBYTE)sceKernelGetBlockHeadAddr(mem);
#endif
		int nTransferLen = allocSize / DISC_MAIN_DATA_SIZE;

		for (int nLBA = 0; nLBA < nVolumeSpaceSize; nLBA += nTransferLen) {
			if (nTransferLen > nVolumeSpaceSize - nLBA) {
				nTransferLen = nVolumeSpaceSize - nLBA;
				allocSize = DISC_MAIN_DATA_SIZE * nTransferLen;
			}
			if (!ExecReadDisc(uid, nLBA, lpBuf, nTransferLen)) {
				break;
			}
			fwrite(lpBuf, sizeof(BYTE), allocSize, fpIso);
#ifdef PBP
			pspPrintf("\rCreating %s.iso (LBA) %6d/%6d", id, nLBA + nTransferLen, nVolumeSpaceSize);
#else
			pspDebugScreenSetXY(0, pspDebugScreenGetY());
			pspPrintf("Creating %s.iso (LBA) %6d/%6d", id, nLBA + nTransferLen, nVolumeSpaceSize);
#endif
		}
#ifdef PRX
		FreeAndNull(lpBuf);
#else
		sceKernelFreePartitionMemory(mem);
#endif
		FcloseAndNull(fpIso);
	}
	pspPrintf("\nDone\n");
	sceIoClose(uid);
	FcloseAndNull(g_LogFile.fpMainInfo);
	FcloseAndNull(g_LogFile.fpMainError);
	FcloseAndNull(g_LogFile.fpVolDesc);
}
